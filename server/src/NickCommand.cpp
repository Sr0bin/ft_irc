#include "NickCommand.hpp"
#include "Server.hpp"
#include "ACommandError.hpp"

NickCommand::NickCommand(Server &server) : ACommand(server)
{
}

NickCommand::~NickCommand()
{
}

bool NickCommand::requiresRegistration(void) const
{
	return (false);
}

size_t NickCommand::minParams(void) const
{
	return (1);
}

void NickCommand::execute(Client &client, Message &msg)
{
	const std::string nick = msg.getParam(0);

	// TODO: no nickname validation yet — add 432 ERR_ERRONEUSNICKNAME (illegal
	// chars/length) and 431 ERR_NONICKNAMEGIVEN (empty) before accepting it.
	// getClientByNick is still a stub (returns 0): uniqueness is a no-op until
	// Server's real implementation lands.
	Client *existing = _server.getClientByNick(nick);
	if (existing != 0 && existing != &client)
		throw NicknameInUse(nick);

	client.setNickName(nick);
	completeRegistrationIfReady(client);
}
