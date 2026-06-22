#ifndef PARTCOMMAND_HPP
#define PARTCOMMAND_HPP
#include "ACommand.hpp"

// PASS <password> — must arrive before registration completes. Sets the client
// to PASSWORD_OK on a correct password; 462 if already registered, 464 if
// wrong.
class PartCommand : public ACommand {
  public:
	PartCommand(Server &server);
	~PartCommand();

	void execute(Client &client, Message &msg);
	size_t minParams(void) const;
};

#endif
