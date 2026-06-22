#ifndef NICKCOMMAND_HPP
# define NICKCOMMAND_HPP
# include "ACommand.hpp"

// NICK <nickname> — allowed before and after registration. Rejects a nick that
// is already in use (433), otherwise stores it and may complete registration.
class NickCommand : public ACommand
{
public:
	NickCommand(Server &server);
	~NickCommand();

	void execute(Client &client, Message &msg);
	bool requiresRegistration(void) const;
	size_t minParams(void) const;
};

#endif
