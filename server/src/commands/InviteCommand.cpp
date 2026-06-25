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

#include "../../include/commands/InviteCommand.hpp"

#include <stdexcept>
#include <sys/socket.h>

#include "../../include/Server.hpp"
#include "../../include/commands/ACommandError.hpp"

InviteCommand::InviteCommand(Server &server) : ACommand(server) {}

InviteCommand::~InviteCommand() {}

size_t InviteCommand::minParams() const {return 2;}

void InviteCommand::execute(Client &client, Message &msg)
{
	Client *invited = _server.getClientByNick(msg.getParam(0));
	if (!invited)
		throw NoSuchNick(msg.getParam(0));
	Channel *channel = _server.getChannelByName(msg.getParam(1));
	if (!channel)
		throw NoSuchChannel(msg.getParam(1));
	if (!channel->isMember(client))
		throw NotOnChannel(msg.getParam(1));
	if (channel->isInviteOnly())
		if (!channel->isOperator(client))
			throw NotChanOp(msg.getParam(1));
	if (channel->isMember(*invited))
		throw UserOnChannel(msg.getParam(1));
	channel->addInvited(*invited);
	invited->queueReply(":"+client.prefix()+" INVITE "+invited->getNickName()+" : "+ channel->getName()+"\r\n");
}

bool InviteCommand::requiresRegistration() const {return true;}