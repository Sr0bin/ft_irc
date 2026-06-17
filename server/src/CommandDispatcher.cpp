/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandDispatcher.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:38:12 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 16:38:23 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "CommandDispatcher.hpp"

CommandDispatcher::CommandDispatcher(Server &server) : _server(server)
{
}

CommandDispatcher::~CommandDispatcher(void)
{
}

void CommandDispatcher::registerCommand(const std::string &name, ACommand *cmd)
{
	(void)name;
	(void)cmd;
}

void CommandDispatcher::dispatch(Client &client, Message &msg)
{
	(void)client;
	(void)msg;
}
