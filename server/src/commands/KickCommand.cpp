#include "KickCommand.hpp"
#include "ACommandError.hpp"
#include "Server.hpp"

KickCommand::KickCommand(Server &server) : ACommand(server) {}

KickCommand::~KickCommand() {}

size_t KickCommand::minParams(void) const { return (2); }

void KickCommand::execute(Client &client, Message &msg) {
	const std::string &ChanName = msg.getParam(0);
	const std::string &userToKick = msg.getParam(0);

	Channel *ch = _server.getChannelByName(ChanName);
	if (!ch)
		throw(NoSuchChannel(ChanName));

	std::set<Client *> clients = ch->getMembers();
	std::set<Client *>::iterator it = clients.find(&client);
	if (it == clients.end())
		throw(NotOnChannel(ChanName));

	std::set<Client *> operators = ch->getOperators();
	std::set<Client *>::iterator it = operators.find(&client);
	if (it == operators.end())
		throw(NotChanOp(ChanName));
}

/*Numeric Replies:

		   ERR_NEEDMOREPARAMS 461 OK
		   ERR_NOSUCHCHANNEL 403 OK
		   ERR_BADCHANMASK => A IGNORER?
		   ERR_CHANOPRIVSNEEDED 482 OK
		   ERR_NOTONCHANNEL* 442 OK
		 ERR_USERNOTINCHANNEL 441  
		   /