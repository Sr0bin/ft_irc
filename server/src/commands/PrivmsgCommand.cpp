/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrivmsgCommand.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/01 00:00:00 by                   #+#    #+#             */
/*   Updated: 2026/06/25 14:03:03 by prigaudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/PrivmsgCommand.hpp"

#include <stdexcept>

#include "ACommandError.hpp"
#include "Server.hpp"
#include <iostream>

PrivmsgCommand::PrivmsgCommand(Server &server) : ACommand(server) {}

PrivmsgCommand::~PrivmsgCommand() {}

size_t PrivmsgCommand::minParams() const { return 2; }

void PrivmsgCommand::execute(Client &client, Message &msg) {
	if (msg.getParam(0)[0] != '#') {
		Client *target = this->_server.getClientByNick(msg.getParam(0));
		if (target == NULL)
			throw NoSuchNick(msg.getParam(0));
		std::string content = msg.getParam(1);

		if (content.empty())
			throw NoRecipient(msg.getParam(1));

		std::vector<std::string> p;
		p.push_back(msg.getParam(0));
		p.push_back(content);

		const std::string privMsg =
			Message(client.prefix(), "PRIVMSG", p).serialize();
		target->queueReply(privMsg);
	} else {
		Channel *channel = this->_server.getChannelByName(msg.getParam(0));
		if (channel == NULL)
			throw NoSuchChannel(msg.getParam(0));
		if (channel->isMember(client) == false)
			throw CannotSendToChan(msg.getParam(0));
		std::string content = msg.getParam(1);
		if (content.empty())
			throw NoRecipient(msg.getParam(1));
		std::set<Client *> client_list = channel->getMembers();
		std::set<Client *>::iterator it;
		for (it = client_list.begin(); it != client_list.end(); ++it) {
			Client *target = *it;
			if (*it == &client)
				continue;
			target->queueReply(":" + client.prefix() + " PRIVMSG " +
							   msg.getParam(0) + ":" + content + "\r\n");
		}
	}
}

bool PrivmsgCommand::requiresRegistration() const { return true; }