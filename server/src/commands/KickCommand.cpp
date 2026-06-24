#include "KickCommand.hpp"
#include "ACommandError.hpp"
#include "Server.hpp"

KickCommand::KickCommand(Server &server) : ACommand(server) {}

KickCommand::~KickCommand() {}

size_t KickCommand::minParams(void) const { return (2); }

void KickCommand::execute(Client &client, Message &msg) {
	const std::string &chanName = msg.getParam(0);
	const std::string &userToKick = msg.getParam(0);

	Channel *ch = _server.getChannelByName(chanName);
	if (!ch)
		throw(NoSuchChannel(chanName));

	if (!ch->isMember(client))
		throw(NotOnChannel(chanName));
}

/*Numeric Replies:

		   ERR_NEEDMOREPARAMS 461 OK
		   ERR_NOSUCHCHANNEL 403 OK
		   ERR_BADCHANMASK => A IGNORER?
		   ERR_CHANOPRIVSNEEDED 482 OK
		   ERR_NOTONCHANNEL 442 OK
		 ERR_USERNOTINCHANNEL 441

		   */