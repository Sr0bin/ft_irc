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
	std::set<Client *> invite_list = channel->getInvited();
	std::set<Client *>::iterator it;
	for (it = invite_list.begin(); it != invite_list.end(); ++it)
		if (*it == invited)
			return;
	invite_list.insert(invited);
	invited->queueReply(":"+client.prefix()+" INVITE "+invited->getNickName()+" : "+ channel->getName());
}

bool InviteCommand::requiresRegistration() const {return true;}