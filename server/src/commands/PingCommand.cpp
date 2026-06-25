/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*                                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpirotti <vpirotti@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   GitHub : @Nave20                                  #+#    #+#             */
/*   28 is the new 42                                 ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/PingCommand.hpp"

#include "Server.hpp"
#include <vector>

PingCommand::PingCommand(Server &server) : ACommand(server) {}

PingCommand::~PingCommand() {}

size_t PingCommand::minParams() const {return 0;}

void PingCommand::execute(Client &client, Message &msg)
{
	// RFC: a server PONG is prefixed with the server name as origin and carries
	// it back; the token (if any) is the trailing arg.
	const std::string server = _server.getServerName();
	std::vector<std::string> p;
	p.push_back(server);
	if (msg.paramCount() == 0) {
		client.queueReply(Message(server, "PONG", p).serialize());
	} else {
		p.push_back(msg.getParam(0));
		client.queueReply(Message(server, "PONG", p, true).serialize());
	}
}

bool PingCommand::requiresRegistration() const {return false;}