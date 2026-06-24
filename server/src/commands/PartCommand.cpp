#include "PartCommand.hpp"
#include "ACommandError.hpp"
#include "Server.hpp"

PartCommand::PartCommand(Server &server) : ACommand(server) {}

PartCommand::~PartCommand() {}

size_t PartCommand::minParams(void) const { return (1); }

void PartCommand::execute(Client &client, Message &msg) {
	const std::string &ChanName = msg.getParam(0);
	std::string reason = "";

	if (msg.paramCount() >= 2)
		reason = msg.getParam(1);

	Channel *ch = _server.getChannelByName(ChanName);

	if (ch == 0)
		throw NoSuchChannel(ChanName);

	if (!ch->isMember(client))
		throw NotOnChannel(ChanName);

	std::vector<std::string> p;
	p.push_back(ChanName);
	if (!reason.empty())
		p.push_back(reason);
	const std::string partMsg =
		Message(client.prefix(), "PART", p, !reason.empty()).serialize();

	ch->broadcast(partMsg, client);
	client.queueReply(partMsg);

	_server.removeClientFromChannel(client, ChanName);
}
