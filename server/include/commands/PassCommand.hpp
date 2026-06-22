#ifndef PASSCOMMAND_HPP
# define PASSCOMMAND_HPP
# include "ACommand.hpp"

// PASS <password> — must arrive before registration completes. Sets the client
// to PASSWORD_OK on a correct password; 462 if already registered, 464 if wrong.
class PassCommand : public ACommand
{
public:
	PassCommand(Server &server);
	~PassCommand();

	void execute(Client &client, Message &msg);
	bool requiresRegistration(void) const;
	size_t minParams(void) const;
};

#endif
