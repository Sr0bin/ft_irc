#ifndef PARTCOMMAND_HPP
#define PARTCOMMAND_HPP
#include "ACommand.hpp"

class PartCommand : public ACommand {
  public:
	PartCommand(Server &server);
	~PartCommand();

	void execute(Client &client, Message &msg);
	size_t minParams(void) const;
};

#endif
