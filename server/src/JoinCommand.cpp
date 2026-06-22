#include "JoinCommand.hpp"
#include "ACommandError.hpp"
#include "Server.hpp"

JoinCommand::JoinCommand(Server &server) : ACommand(server) {}

JoinCommand::~JoinCommand() {}

size_t JoinCommand::minParams(void) const { return (1); }

void JoinCommand::execute(Client &client, Message &msg) {
	const std::string &name = msg.getParam(0);
	std::string pass = "";

	if (msg.paramCount() >= 2)
		pass = msg.getParam(1);

	Channel *ch = _server.getChannelByName(name);

	if (ch != 0 && ch->isMember(client))
		return;

	if (ch != 0 && !ch->canJoin(client, pass))
		throw NoSuchChannel(name);

	_server.addClientToChannel(client, name);

	const std::string joinMsg =
		":" + client.prefix() + " JOIN " + name + "\r\n";
	ch->broadcast(joinMsg, client);
	client.queueReply(joinMsg);
}
