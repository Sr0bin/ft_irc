#include "JoinCommand.hpp"
#include "ACommandError.hpp"
#include "Server.hpp"

JoinCommand::JoinCommand(Server &server) : ACommand(server) {}

JoinCommand::~JoinCommand() {}

size_t JoinCommand::minParams(void) const { return (1); }

void JoinCommand::execute(Client &client, Message &msg) {
	const std::string &chanName = msg.getParam(0);
	std::string pass = "";

	// Channel names must start with '#' (subject scope). Reject otherwise so a
	// non-'#' channel can never be created — MODE/PART assume the '#' prefix.
	if (chanName.empty() || chanName[0] != '#')
		throw NoSuchChannel(chanName);

	if (msg.paramCount() >= 2)
		pass = msg.getParam(1);

	Channel *ch = _server.getChannelByName(chanName);
	if (!ch)
		ch = _server.addClientToChannel(client, chanName);
	else if (ch->isMember(client))
		return;
	else {
		ch->canJoin(client, pass);

		_server.addClientToChannel(client, chanName);
	}

	std::vector<std::string> p;
	p.push_back(chanName);
	const std::string joinMsg = Message(client.prefix(), "JOIN", p).serialize();
	ch->broadcast(joinMsg, client);
	client.queueReply(joinMsg);

	const std::string serverName = _server.getServerName();

	sendTopic(client, ch, chanName, serverName);
	sendNames(client, ch, chanName, serverName);
}

void JoinCommand::sendTopic(Client &client, Channel *ch,
							const std::string &chanName,
							const std::string &serverName) {
	std::string topic = ch->getTopic();
	const std::string &nick = client.getNickName();
	if (topic.empty())
		client.queueReply(
			RplNoTopic(chanName).toMessage(serverName, nick).serialize());
	else
		client.queueReply(
			RplTopic(chanName, topic).toMessage(serverName, nick).serialize());
}

void JoinCommand::sendNames(Client &client, Channel *ch,
							const std::string &chanName,
							const std::string &serverName) {
	std::string namesList = "";
	const std::set<Client *> &members = ch->getMembers();
	const std::set<Client *> &operators = ch->getOperators();

	for (std::set<Client *>::const_iterator it = members.begin();
		 it != members.end(); ++it) {
		if (!namesList.empty())
			namesList += " ";
		if (operators.count(*it))
			namesList += "@";
		namesList += (*it)->getNickName();
	}

	const std::string &nick = client.getNickName();
	client.queueReply(RplNamReply(chanName, namesList)
						  .toMessage(serverName, nick)
						  .serialize());
	client.queueReply(
		RplEndOfNames(chanName).toMessage(serverName, nick).serialize());
}