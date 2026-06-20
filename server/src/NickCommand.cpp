#include "NickCommand.hpp"
#include "Server.hpp"
#include "ACommandError.hpp"
#include <cctype>

// IRC special chars allowed in a nick (RFC 1459/2812).
static bool isNickSpecial(char c)
{
	return (std::string("[]\\`_^{|}").find(c) != std::string::npos);
}

static bool isValidNick(const std::string &nick)
{
	// RFC 2812: nickname = (letter/special) *8(...) -> max 9 chars.
	if (nick.empty() || nick.size() > 9)
		return (false);
	if (!std::isalpha(static_cast<unsigned char>(nick[0])) && !isNickSpecial(nick[0]))
		return (false);
	for (std::string::size_type i = 1; i < nick.size(); ++i)
	{
		char c = nick[i];
		if (!std::isalnum(static_cast<unsigned char>(c)) && !isNickSpecial(c) && c != '-')
			return (false);
	}
	return (true);
}

NickCommand::NickCommand(Server &server) : ACommand(server)
{
}

NickCommand::~NickCommand()
{
}

bool NickCommand::requiresRegistration(void) const
{
	return (false);
}

size_t NickCommand::minParams(void) const
{
	return (1);
}

void NickCommand::execute(Client &client, Message &msg)
{
	const std::string nick = msg.getParam(0);

	if (nick.empty())
		throw NoNicknameGiven();
	if (!isValidNick(nick))
		throw ErroneousNickname(nick);

	// getClientByNick is real now (Utils::ircEquals): reject a nick already held
	// by another client.
	Client *existing = _server.getClientByNick(nick);
	if (existing != 0 && existing != &client)
		throw NicknameInUse(nick);

	client.setNickName(nick);
	completeRegistrationIfReady(client);
}
