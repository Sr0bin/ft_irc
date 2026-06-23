#include "JoinCommand.hpp"
#include "ACommandError.hpp"
#include "Server.hpp"

JoinCommand::JoinCommand(Server &server) : ACommand(server) {}

JoinCommand::~JoinCommand() {}

size_t JoinCommand::minParams(void) const { return (1); }

void JoinCommand::execute(Client &client, Message &msg) {
	const std::string &name = msg.getParam(0);
	std::string pass = "";

	// Channel names must start with '#' (subject scope). Reject otherwise so a
	// non-'#' channel can never be created — MODE/PART assume the '#' prefix.
	if (name.empty() || name[0] != '#')
		throw NoSuchChannel(name);

	if (msg.paramCount() >= 2)
		pass = msg.getParam(1);

	Channel *ch = _server.getChannelByName(name);
	if (!ch)
		ch = _server.addClientToChannel(client, name);
	else if (ch->isMember(client))
		return;
	else {
		ch->canJoin(client, pass);

		_server.addClientToChannel(client, name);
	}

	std::vector<std::string> p;
	p.push_back(name);
	const std::string joinMsg = Message(client.prefix(), "JOIN", p).serialize();
	ch->broadcast(joinMsg, client);
	client.queueReply(joinMsg);

	const std::string serverName = _server.getServerName();

	sendTopic(client, ch, name, serverName);
	sendNames(client, ch, name, serverName);
}

void JoinCommand::sendTopic(Client &client, Channel *ch,
							const std::string &name,
							const std::string &serverName) {
	std::string topic = ch->getTopic();
	const std::string &nick = client.getNickName();
	if (topic.empty())
		client.queueReply(
			RplNoTopic(name).toMessage(serverName, nick).serialize());
	else
		client.queueReply(
			RplTopic(name, topic).toMessage(serverName, nick).serialize());
}

void JoinCommand::sendNames(Client &client, Channel *ch,
							const std::string &name,
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
	client.queueReply(
		RplNamReply(name, namesList).toMessage(serverName, nick).serialize());
	client.queueReply(
		RplEndOfNames(name).toMessage(serverName, nick).serialize());
}