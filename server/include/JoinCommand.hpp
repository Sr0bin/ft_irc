#ifndef JOINCOMMAND_HPP
#define JOINCOMMAND_HPP
#include "ACommand.hpp"

// PASS <password> — must arrive before registration completes. Sets the client
// to PASSWORD_OK on a correct password; 462 if already registered, 464 if
// wrong.
class JoinCommand : public ACommand {
  public:
	JoinCommand(Server &server);
	~JoinCommand();

	void execute(Client &client, Message &msg);
	size_t minParams(void) const;
};

#endif
