#ifndef USERCOMMAND_HPP
# define USERCOMMAND_HPP
# include "ACommand.hpp"

// USER <user> <mode> <unused> :<realname> — pre-registration only. Stores
// username/realname; 462 if already registered. May complete registration.
class UserCommand : public ACommand
{
public:
	UserCommand(Server &server);
	~UserCommand();

	void execute(Client &client, Message &msg);
	bool requiresRegistration(void) const;
	size_t minParams(void) const;
};

#endif
