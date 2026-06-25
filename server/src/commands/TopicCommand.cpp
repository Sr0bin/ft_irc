#include "TopicCommand.hpp"
#include "ACommandError.hpp"
#include "Server.hpp"

TopicCommand::TopicCommand(Server &server) : ACommand(server) {}

TopicCommand::~TopicCommand() {}

size_t TopicCommand::minParams(void) const { return (1); }

void TopicCommand::execute(Client &client, Message &msg) {
	const std::string &chanName = msg.getParam(0);

	Channel *ch = _server.getChannelByName(chanName);
	if (!ch)
		throw(NoSuchChannel(chanName));

	std::string topic = ch->getTopic();

	const std::string serverName = _server.getServerName();
	const std::string &nick = client.getNickName();

	if (!ch->isMember(client))
		throw(NotOnChannel(chanName));

	if (msg.paramCount() < 2) {
		if (topic.empty())
			client.queueReply(
				RplNoTopic(chanName).toMessage(serverName, nick).serialize());
		else {
			client.queueReply(RplTopic(chanName, topic)
								  .toMessage(serverName, nick)
								  .serialize());
		}
	} else {
		if (ch->isTopicRestricted() && !ch->isOperator(client))
			throw(ChanOPrivsNeeded(chanName));
		else {
			topic = msg.getParam(1);
			ch->setTopic(topic);
			std::vector<std::string> p;
			p.push_back(chanName);
			p.push_back(topic);
			const std::string topicMsg =
				Message(client.prefix(), "TOPIC", p, true).serialize();
			ch->broadcast(topicMsg, client);
			client.queueReply(RplTopic(chanName, topic)
								  .toMessage(serverName, nick)
								  .serialize());
		}
	}
}
