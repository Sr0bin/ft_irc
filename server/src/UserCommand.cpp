#include "UserCommand.hpp"
#include "Server.hpp"
#include "ACommandError.hpp"

UserCommand::UserCommand(Server &server) : ACommand(server)
{
}

UserCommand::~UserCommand()
{
}

bool UserCommand::requiresRegistration(void) const
{
	return (false);
}

size_t UserCommand::minParams(void) const
{
	return (4);
}

void UserCommand::execute(Client &client, Message &msg)
{
	if (client.isRegistered())
		throw AlreadyRegistered();

	client.setUserName(msg.getParam(0));
	client.setRealName(msg.getParam(3));
	completeRegistrationIfReady(client);
}
