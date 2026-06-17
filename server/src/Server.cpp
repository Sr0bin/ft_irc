/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:38:13 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 16:38:23 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"

Server::Server(void) : _mux(0), _dispatcher(0)
{
}

Server::Server(serverConfig config) : _config(config), _mux(0), _dispatcher(0)
{
}

Server::~Server(void)
{
}

void Server::run()
{
}

void Server::acceptClient()
{
}

void Server::disconnectClient(int fd)
{
	(void)fd;
}

Client *Server::getClientByNick(std::string nick)
{
	(void)nick;
	return (0);
}

Channel *Server::getChannelByName(std::string name)
{
	(void)name;
	return (0);
}
