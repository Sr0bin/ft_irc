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

#include <sys/socket.h>

PingCommand::PingCommand(Server &server) : ACommand(server) {}

PingCommand::~PingCommand() {}

size_t PingCommand::minParams() const {return 0;}

void PingCommand::execute(Client &client, Message &msg)
{
	if (msg.paramCount() == 0)
		client.queueReply("PONG\r\n");
	else
		client.queueReply("PONG :" + msg.getParam(0) + "\r\n");
}

bool PingCommand::requiresRegistration() const {return false;}