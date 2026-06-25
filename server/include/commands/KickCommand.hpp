#ifndef KICKCOMMAND_HPP
#define KICKCOMMAND_HPP
#include "ACommand.hpp"
#include "ACommandReply.hpp"

class KickCommand : public ACommand {
  public:
	KickCommand(Server &server);
	~KickCommand();

	void execute(Client &client, Message &msg);
	size_t minParams(void) const;
};

#endif
