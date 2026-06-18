/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:38:13 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/18 12:37:18 by prigaudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(void) : _mux(0), _dispatcher(0) {}

Server::Server(serverConfig config)
    : _config(config), _mux(0), _dispatcher(0) {}

Server::~Server(void) {}

void Server::run() {
  _config._listenFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_config._listenFd == -1)
    throw IrcException("socket() failed");

  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(_config._port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(_config._listenFd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    throw IrcException("bind() failed");
}

void Server::acceptClient() {}

void Server::disconnectClient(int fd) { (void)fd; }

Client *Server::getClientByNick(std::string nick) {
  (void)nick;
  return (0);
}

Channel *Server::getChannelByName(std::string name) {
  (void)name;
  return (0);
}