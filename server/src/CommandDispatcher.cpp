/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandDispatcher.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:38:12 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/20 16:31:59 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "CommandDispatcher.hpp"
#include  "ACommandError.hpp"
#include <iostream>
#include <ostream>

#include "randomCmd.hpp"

CommandDispatcher::CommandDispatcher(Server &server) : _server(server)
{
	registerCommand("random", new randomCmd(_server));
}

CommandDispatcher::~CommandDispatcher(void)
{
	for (std::map<std::string, ACommand *>::iterator it = _commands.begin(); it != _commands.end(); ++it)
	{
		delete it->second;
	}
}

void CommandDispatcher::registerCommand(const std::string &name, ACommand *cmd)
{
	_commands[name] = cmd;
}

void CommandDispatcher::dispatch(Client& client, Message& msg)
{
	std::map<std::string, ACommand*>::iterator it = _commands.find(msg.getCommand());

	if (it == _commands.end())
		return;

	ACommand* cmd = it->second;

	cmd->execute(client, msg);
	// try
	// {
	// 	if (cmd->requiresRegistration() && !client.isRegistered())
	// 		// throw NotRegistered();
	//
	// 	if (msg.paramCount() < cmd->minParams())
	// 		throw NeedMoreParams(msg.getCommand());
	//
	// 	cmd->execute(client, msg);
	// }
	// catch (ACommandError& e)
	// {
	// 	std::cout << "error" << std::endl;
	// }
}
