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
#include "Server.hpp"
#include  "ACommandError.hpp"
#include "Utils.hpp"
#include <iostream>
#include <ostream>

#include "randomCmd.hpp"
#include "PassCommand.hpp"
#include "NickCommand.hpp"
#include "UserCommand.hpp"
#include "JoinCommand.hpp"
#include "PartCommand.hpp"

CommandDispatcher::CommandDispatcher(Server &server) : _server(server)
{
	// Keys are lower-case; dispatch() normalises the incoming command with
	// Utils::toLower so the lookup is case-insensitive (IRC commands are).
	registerCommand("random", new randomCmd(_server));
	registerCommand("pass", new PassCommand(_server));
	registerCommand("nick", new NickCommand(_server));
	registerCommand("user", new UserCommand(_server));
	registerCommand("join", new JoinCommand(_server));
	registerCommand("part", new PartCommand(_server));
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
	try
	{
		std::map<std::string, ACommand*>::iterator it = _commands.find(Utils::toLower(msg.getCommand()));

		if (it == _commands.end())
			throw UnknownCommand(msg.getCommand());

		ACommand* cmd = it->second;

		if (cmd->requiresRegistration() && !client.isRegistered())
			throw NotRegistered();
		if (msg.paramCount() < cmd->minParams())
			throw NeedMoreParams(msg.getCommand());

		cmd->execute(client, msg);
	}
	catch (ACommandError& e)
	{
		// Unregistered clients have no nick yet: numerics use '*' as placeholder.
		std::string nick = client.getNickName();
		if (nick.empty())
			nick = "*";
		client.queueReply(
			e.toMessage(_server.getServerName(), nick).serialize());
	}
	catch (std::exception& e)
	{
		// TODO: careful — this also swallows FatalException. By design it's never
		// thrown from a command, but once main() is wired, add an explicit
		// `catch (FatalException&) { throw; }` before this net so a real fatal
		// error propagates up instead of being logged & ignored here.
		// Safety net: a non-ACommandError must not kill the server. Log & continue.
		std::cerr << "dispatch: " << e.what() << std::endl;
	}
}
