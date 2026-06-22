#include "PartCommand.hpp"
#include "ACommandError.hpp"
#include "Server.hpp"

PartCommand::PartCommand(Server &server) : ACommand(server) {}

PartCommand::~PartCommand() {}

size_t PartCommand::minParams(void) const { return (1); }

void PartCommand::execute(Client &client, Message &msg) {
	const std::string &name = msg.getParam(0);
	std::string reason = "";

	if (msg.paramCount() >= 2)
		reason = msg.getParam(1);

	Channel *ch = _server.getChannelByName(name);

	if (ch == 0)
		throw NoSuchChannel(name);

	if (!ch->isMember(client))
		throw NotOnChannel(name);

	std::string partMsg = ":" + client.prefix() + " PART " + name;
	if (!reason.empty())
		partMsg += " :" + reason;
	partMsg += "\r\n";

	ch->broadcast(partMsg, client);
	client.queueReply(partMsg);

	_server.removeClientFromChannel(client, name);
}
