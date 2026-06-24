#include "TopicCommand.hpp"
#include "ACommandError.hpp"
#include "Server.hpp"

TopicCommand::TopicCommand(Server &server) : ACommand(server) {}

TopicCommand::~TopicCommand() {}

size_t TopicCommand::minParams(void) const { return (1); }

void TopicCommand::execute(Client &client, Message &msg) {
	const std::string &name = msg.getParam(0);

	Channel *ch = _server.getChannelByName(name);
	if (!ch)
		throw(NoSuchChannel(name));

	std::string topic = ch->getTopic();

	const std::string serverName = _server.getServerName();
	const std::string &nick = client.getNickName();

	std::set<Client *> clients = ch->getMembers();
	std::set<Client *>::iterator it = clients.find(&client);
	if (it == clients.end())
		throw(NotOnChannel(name));

	if (msg.paramCount() < 2) {
		if (topic.empty())
			client.queueReply(
				RplNoTopic(name).toMessage(serverName, nick).serialize());
		else {
			client.queueReply(
				RplTopic(name, topic).toMessage(serverName, nick).serialize());
		}
	} else {
		if (ch->isTopicRestricted() && !ch->isOperator(client))
			throw(NotChanOp(name));
		else {
			topic = msg.getParam(1);
			ch->setTopic(topic);
			std::vector<std::string> p;
			p.push_back(name);
			p.push_back(topic);
			const std::string topicMsg =
				Message(client.prefix(), "TOPIC", p).serialize();
			ch->broadcast(topicMsg, client);
			client.queueReply(
				RplTopic(name, topic).toMessage(serverName, nick).serialize());
		}
	}
}