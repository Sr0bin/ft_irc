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

#include "../../include/commands/PrivmsgCommand.hpp"

#include <stdexcept>

#include "ACommandError.hpp"
#include "Server.hpp"

PrivmsgCommand::PrivmsgCommand(Server &server) : ACommand(server) {}

PrivmsgCommand::~PrivmsgCommand() {}

size_t PrivmsgCommand::minParams() const {return 2;}

void PrivmsgCommand::execute(Client &client, Message &msg)
{
	if (msg.getParam(0)[0] != '#')
	{
		Client *target =  this->_server.getClientByNick(msg.getParam(0));
		if (target == NULL)
			throw NoSuchNick(msg.getParam(0));
		std::string content = msg.getParam(1);
		if (content.empty())
			throw NoRecipient(msg.getParam(1));
		target->queueReply(":" + client.prefix() + " PRIVMSG " + msg.getParam(0) + ":" + content + "\r\n");
	}
	else
	{
		Channel *channel = this->_server.getChannelByName(msg.getParam(0));
		if (channel == NULL)
			throw NoSuchChannel(msg.getParam(0));
		if (channel->isMember(client) == false)
			throw CannotSendToChan(msg.getParam(0));
		std::string content = msg.getParam(1);
		if (content.empty())
			throw NoRecipient(msg.getParam(1));
		std::set<Client *> client_list = channel->getMembers();
		std::set<Client*>::iterator it;
		for (it = client_list.begin(); it != client_list.end(); ++it)
		{
			Client* target = *it;
			if (*it == &client)
				continue;
			target->queueReply(":" + client.prefix() + " PRIVMSG " + msg.getParam(0) + ":" + content + "\r\n");
		}
	}
}

bool PrivmsgCommand::requiresRegistration() const {return true;}