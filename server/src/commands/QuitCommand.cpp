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

#include "../../include/commands/QuitCommand.hpp"

#include "Server.hpp"

QuitCommand::QuitCommand(Server &server) : ACommand(server) {}

QuitCommand::~QuitCommand() {}

size_t QuitCommand::minParams() const {return 0;}

void QuitCommand::execute(Client &client, Message &msg)
{
	// RFC QUIT relay is ":prefix QUIT :reason" — no channel param, reason is a
	// trailing arg (so multi-word reasons survive). A peer sharing several
	// channels with the quitter must receive the message only once.
	std::vector<std::string> p;
	p.push_back(msg.getParam(0).empty() ? "Client Quit" : msg.getParam(0));
	const std::string content = Message(client.prefix(), "QUIT", p, true).serialize();

	std::set<Client *> notified;
	std::set<Channel *> channels = client.getChannels();
	for (std::set<Channel *>::iterator it = channels.begin();
		 it != channels.end(); ++it)
	{
		const std::set<Client *> &members = (*it)->getMembers();
		for (std::set<Client *>::const_iterator it2 = members.begin();
			 it2 != members.end(); ++it2)
		{
			if (*it2 == &client)
				continue;
			if (notified.insert(*it2).second)
				(*it2)->queueReply(content);
		}
	}
	this->_server.disconnectClient(client.getFd());
}

bool QuitCommand::requiresRegistration() const {return false;}
