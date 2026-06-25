/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrivmsgCommand.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/01 00:00:00 by                   #+#    #+#             */
/*   Updated: 2026/06/25 18:36:07 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/commands/PrivmsgCommand.hpp"

#include "ACommandError.hpp"
#include "Server.hpp"

PrivmsgCommand::PrivmsgCommand(Server &server) : ACommand(server) {}

PrivmsgCommand::~PrivmsgCommand() {}

size_t PrivmsgCommand::minParams() const { return 2; }

void PrivmsgCommand::execute(Client &client, Message &msg) {
	const std::string target = msg.getParam(0);
	if (target.empty() || target[0] != '#') {
		std::string targetNick = msg.getParam(0);
		Client *target = _server.getClientByNick(targetNick);
		if (target == NULL)
			throw NoSuchNick(targetNick);

		std::string content = msg.getParam(1);
		if (content.empty())
			throw NoTextToSend("PRIVMSG");

		std::vector<std::string> p;
		p.push_back(targetNick);
		p.push_back(content);

		const std::string privMsg =
			Message(client.prefix(), "PRIVMSG", p, true).serialize();
		target->queueReply(privMsg);
	} else {
		std::string chanName = msg.getParam(0);
		Channel *channel = _server.getChannelByName(chanName);
		if (channel == NULL)
			throw NoSuchChannel(msg.getParam(0));
		if (!channel->isMember(client))
			throw CannotSendToChan(msg.getParam(0));
		std::string content = msg.getParam(1);
		if (content.empty())
			throw NoTextToSend("PRIVMSG");

		std::vector<std::string> p;
		p.push_back(chanName);
		p.push_back(content);

		const std::string chanMsg =
			Message(client.prefix(), "PRIVMSG", p, true).serialize();

		channel->broadcast(chanMsg, client);
	}
}
