#include "NickCommand.hpp"
#include "ACommandError.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include <cctype>
#include <set>

// IRC special chars allowed in a nick (RFC 1459/2812).
static bool isNickSpecial(char c) {
	return (std::string("[]\\`_^{|}").find(c) != std::string::npos);
}

static bool isValidNick(const std::string &nick) {
	// RFC 2812: nickname = (letter/special) *8(...) -> max 9 chars.
	if (nick.empty() || nick.size() > 9)
		return (false);
	if (!std::isalpha(static_cast<unsigned char>(nick[0])) &&
		!isNickSpecial(nick[0]))
		return (false);
	for (std::string::size_type i = 1; i < nick.size(); ++i) {
		char c = nick[i];
		if (!std::isalnum(static_cast<unsigned char>(c)) && !isNickSpecial(c) &&
			c != '-')
			return (false);
	}
	return (true);
}

NickCommand::NickCommand(Server &server) : ACommand(server) {}

NickCommand::~NickCommand() {}

bool NickCommand::requiresRegistration(void) const { return (false); }

size_t NickCommand::minParams(void) const { return (1); }

void NickCommand::execute(Client &client, Message &msg) {
	const std::string nick = msg.getParam(0);

	if (nick.empty())
		throw NoNicknameGiven();
	if (!isValidNick(nick))
		throw ErroneousNickname(nick);

	Client *existing = _server.getClientByNick(nick);
	if (existing != 0 && existing != &client)
		throw NicknameInUse(nick);

	// A registered client renaming itself must tell everyone who shares a
	// channel with it (and itself), once each. Pre-registration there is no one
	// to notify, so we only set the nick.
	const bool announce = client.isRegistered() &&
						  !client.getNickName().empty() &&
						  client.getNickName() != nick;
	const std::string oldPrefix = client.prefix();

	client.setNickName(nick);

	if (announce) {
		std::vector<std::string> p;
		p.push_back(nick);
		const std::string relay = Message(oldPrefix, "NICK", p).serialize();
		client.queueReply(relay);

		std::set<Client *> notified;
		const std::set<Channel *> &channels = client.getChannels();
		for (std::set<Channel *>::const_iterator it = channels.begin();
			 it != channels.end(); ++it) {
			const std::set<Client *> &members = (*it)->getMembers();
			for (std::set<Client *>::const_iterator it2 = members.begin();
				 it2 != members.end(); ++it2) {
				if (*it2 == &client)
					continue;
				if (notified.insert(*it2).second)
					(*it2)->queueReply(relay);
			}
		}
	}

	completeRegistrationIfReady(client);
}
