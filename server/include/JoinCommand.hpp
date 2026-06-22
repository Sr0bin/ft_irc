#ifndef JOINCOMMAND_HPP
#define JOINCOMMAND_HPP
#include "ACommand.hpp"
#include "ACommandReply.hpp"

class JoinCommand : public ACommand {
  public:
	JoinCommand(Server &server);
	~JoinCommand();

	void execute(Client &client, Message &msg);
	size_t minParams(void) const;
};

#endif
