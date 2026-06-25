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
	std::set<Channel *> channels = client.getChannels();
	std::set<Channel *>::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it)
	{
		Channel *current_channel = *it;
		std::set<Client *> client_list = current_channel->getMembers();
		std::set<Client *>::iterator it2;
		for (it2 = client_list.begin(); it2 != client_list.end(); ++it2)
		{
			Client *current_client = *it2;
			if (*it2 == &client)
				continue;
			std::string content;
			std::vector<std::string> p;
			//TODO: check the format of the quit message
			p.push_back(current_channel->getName());
			p.push_back(msg.getParam(0).empty() ? "Client Quit" : msg.getParam(0));
			content = Message(client.prefix(), "QUIT", p).serialize();

			current_client->queueReply(content);
		}
	}
	this->_server.disconnectClient(client.getFd());
}

bool QuitCommand::requiresRegistration() const {return false;}
