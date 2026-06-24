#include "TopicCommand.hpp"
#include "ACommandError.hpp"
#include "Server.hpp"

TopicCommand::TopicCommand(Server &server) : ACommand(server) {}

TopicCommand::~TopicCommand() {}

size_t TopicCommand::minParams(void) const { return (1); }

void TopicCommand::execute(Client &client, Message &msg) {
	const std::string &ChanName = msg.getParam(0);

	Channel *ch = _server.getChannelByName(ChanName);
	if (!ch)
		throw(NoSuchChannel(ChanName));

	std::string topic = ch->getTopic();

	const std::string serverName = _server.getServerName();
	const std::string &nick = client.getNickName();

	if (!ch->isOperator(client))
		throw(NotOnChannel(ChanName));

	if (msg.paramCount() < 2) {
		if (topic.empty())
			client.queueReply(
				RplNoTopic(ChanName).toMessage(serverName, nick).serialize());
		else {
			client.queueReply(RplTopic(ChanName, topic)
								  .toMessage(serverName, nick)
								  .serialize());
		}
	} else {
		if (ch->isTopicRestricted() && !ch->isOperator(client))
			throw(NotChanOp(ChanName));
		else {
			topic = msg.getParam(1);
			ch->setTopic(topic);
			std::vector<std::string> p;
			p.push_back(ChanName);
			p.push_back(topic);
			const std::string topicMsg =
				Message(client.prefix(), "TOPIC", p).serialize();
			ch->broadcast(topicMsg, client);
			client.queueReply(RplTopic(ChanName, topic)
								  .toMessage(serverName, nick)
								  .serialize());
		}
	}
}