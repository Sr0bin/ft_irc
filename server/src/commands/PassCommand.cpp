#include "PassCommand.hpp"
#include "Server.hpp"
#include "ACommandError.hpp"

PassCommand::PassCommand(Server &server) : ACommand(server)
{
}

PassCommand::~PassCommand()
{
}

bool PassCommand::requiresRegistration(void) const
{
	return (false);
}

size_t PassCommand::minParams(void) const
{
	return (1);
}

void PassCommand::execute(Client &client, Message &msg)
{
	if (client.isRegistered())
		throw AlreadyRegistered();
	if (msg.getParam(0) != _server.getPassword())
		throw PasswordMismatch();

	client.setState(PASSWORD_OK);
	completeRegistrationIfReady(client);
}
