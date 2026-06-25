/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandDispatcher.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:38:12 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/25 15:29:20 by prigaudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandDispatcher.hpp"
#include "ACommandError.hpp"
#include "FatalException.hpp"
#include "ModeCommand.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include <iostream>
#include <ostream>

#include "InviteCommand.hpp"
#include "JoinCommand.hpp"
#include "KickCommand.hpp"
#include "NickCommand.hpp"
#include "PartCommand.hpp"
#include "PassCommand.hpp"
#include "PingCommand.hpp"
#include "PrivmsgCommand.hpp"
#include "QuitCommand.hpp"
#include "TopicCommand.hpp"
#include "UserCommand.hpp"

CommandDispatcher::CommandDispatcher(Server &server) : _server(server) {
	// Keys are lower-case; dispatch() normalises the incoming command with
	// Utils::toLower so the lookup is case-insensitive (IRC commands are).
	registerCommand("pass", new PassCommand(_server));
	registerCommand("nick", new NickCommand(_server));
	registerCommand("user", new UserCommand(_server));
	registerCommand("join", new JoinCommand(_server));
	registerCommand("part", new PartCommand(_server));
	registerCommand("topic", new TopicCommand(_server));
	registerCommand("kick", new KickCommand(_server));
	registerCommand("mode", new ModeCommand(_server));
	registerCommand("ping", new PingCommand(_server));
	registerCommand("privmsg", new PrivmsgCommand(_server));
	registerCommand("quit", new QuitCommand(_server));
	registerCommand("invite", new InviteCommand(_server));
}

CommandDispatcher::~CommandDispatcher(void) {
	for (std::map<std::string, ACommand *>::iterator it = _commands.begin();
		 it != _commands.end(); ++it) {
		delete it->second;
	}
}

void CommandDispatcher::registerCommand(const std::string &name,
										ACommand *cmd) {
	_commands[name] = cmd;
}

void CommandDispatcher::dispatch(Client &client, Message &msg) {
	try {
		std::map<std::string, ACommand *>::iterator it =
			_commands.find(Utils::toLower(msg.getCommand()));

		if (it == _commands.end())
			throw UnknownCommand(msg.getCommand());

		ACommand *cmd = it->second;

		if (cmd->requiresRegistration() && !client.isRegistered())
			throw NotRegistered();
		if (msg.paramCount() < cmd->minParams())
			throw NeedMoreParams(msg.getCommand());

		cmd->execute(client, msg);
	} catch (FatalException &) {
		// A genuine fatal error must reach main(), not be swallowed by the
		// std::exception safety net below.
		throw;
	} catch (ACommandError &e) {
		// Unregistered clients have no nick yet: numerics use '*' as
		// placeholder.
		std::string nick = client.getNickName();
		if (nick.empty())
			nick = "*";
		client.queueReply(
			e.toMessage(_server.getServerName(), nick).serialize());
	} catch (std::exception &e) {
		// Safety net: a non-ACommandError must not kill the server — log &
		// continue. (FatalException is rethrown by the catch above, so it
		// reaches main() instead of being swallowed here.)
		Utils::log(LOG_ERR, client.tag() + " dispatch: " + e.what());
	}
}
