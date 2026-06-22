/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandDispatcher.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:25:26 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 16:29:48 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef COMMANDDISPATCHER_HPP
# define COMMANDDISPATCHER_HPP
# include "ft_irc.hpp"
# include "ACommand.hpp"

class Server;

class CommandDispatcher
{
public:
	CommandDispatcher(Server &server);
	~CommandDispatcher(void);

	void registerCommand(const std::string &name, ACommand *cmd);
	void dispatch(Client &client, Message &msg);

private:
	// No copy allowed
	CommandDispatcher(const CommandDispatcher &other);
	CommandDispatcher &operator=(const CommandDispatcher &other);

	Server &_server;
	std::map<std::string, ACommand *> _commands;

};

#endif
