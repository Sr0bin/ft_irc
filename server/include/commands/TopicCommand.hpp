#ifndef TOPICCOMMAND_HPP
#define TOPICCOMMAND_HPP
#include "ACommand.hpp"
#include "ACommandReply.hpp"

class TopicCommand : public ACommand {
  public:
	TopicCommand(Server &server);
	~TopicCommand();

	void execute(Client &client, Message &msg);
	size_t minParams(void) const;
};

#endif
