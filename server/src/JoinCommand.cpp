#include "JoinCommand.hpp"
#include "ACommandError.hpp"
#include "Server.hpp"

JoinCommand::JoinCommand(Server &server) : ACommand(server) {}

JoinCommand::~JoinCommand() {}

size_t JoinCommand::minParams(void) const { return (1); }

void JoinCommand::execute(Client &client, Message &msg) {
	const std::string &name = msg.getParam(0);
	std::string pass = "";

	if (msg.paramCount() >= 2)
		pass = msg.getParam(1);

	Channel *ch = _server.getChannelByName(name);

	if (ch != 0 && ch->isMember(client))
		return;

	if (ch != 0 && !ch->canJoin(client, pass))
		throw NoSuchChannel(name);

	_server.addClientToChannel(client, name);

	Channel *safeCh = _server.getChannelByName(name);
	const std::string joinMsg =
		":" + client.prefix() + " JOIN " + name + "\r\n";
	safeCh->broadcast(joinMsg, client);
	client.queueReply(joinMsg);

	const std::string serverPrefix = ":" + _server.getServerName() + " ";

	sendTopic(client, safeCh, name, serverPrefix);
	sendNames(client, safeCh, name, serverPrefix);
}

void JoinCommand::sendTopic(Client &client, Channel *ch,
							const std::string &name,
							const std::string &serverPrefix) {
	std::string topic = ch->getTopic();
	if (topic.empty())
		client.queueReply(serverPrefix +
						  RplNoTopic(name).buildReply(client.getNickName()));
	else
		client.queueReply(
			serverPrefix +
			RplTopic(name, topic).buildReply(client.getNickName()));
}

void JoinCommand::sendNames(Client &client, Channel *ch,
							const std::string &name,
							const std::string &serverPrefix) {
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

	client.queueReply(
		serverPrefix +
		RplNamReply(name, namesList).buildReply(client.getNickName()));
	client.queueReply(serverPrefix +
					  RplEndOfNames(name).buildReply(client.getNickName()));
}