#include "KickCommand.hpp"
#include "ACommandError.hpp"
#include "Server.hpp"

KickCommand::KickCommand(Server &server) : ACommand(server) {}

KickCommand::~KickCommand() {}

size_t KickCommand::minParams(void) const { return (2); }

void KickCommand::execute(Client &client, Message &msg) {
	const std::string &chanName = msg.getParam(0);
	const std::string &nicknameToKick = msg.getParam(1);
	std::string reason = "";

	if (msg.paramCount() >= 2)
		reason = msg.getParam(2);

	Channel *ch = _server.getChannelByName(chanName);
	if (!ch)
		throw(NoSuchChannel(chanName));

	if (!ch->isMember(client))
		throw(NotOnChannel(chanName));

	if (!ch->isOperator(client))
		throw(NotChanOp(chanName));

	Client *clientToKick = _server.getClientByNick(nicknameToKick);
	if (!clientToKick || !ch->isMember(*clientToKick))
		throw(UserNotInChannel(nicknameToKick, chanName));

	const std::string serverName = _server.getServerName();

	std::string kickMsg = ":" + client.getNickName() + "!" +
						  client.getUserName() + "@" + serverName + " KICK " +
						  chanName + " " + nicknameToKick + " :" +
						  (reason.empty() ? nicknameToKick : reason);

	ch->broadcast(kickMsg, *clientToKick);
	clientToKick->queueReply(kickMsg);

	_server.removeClientFromChannel(*clientToKick, chanName);
}