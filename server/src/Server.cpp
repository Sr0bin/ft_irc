/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:38:13 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/19 11:12:40 by prigaudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(void) : _mux(0), _dispatcher(0) {}

Server::Server(serverConfig config)
	: _config(config), _mux(0), _dispatcher(0) {}

Server::~Server(void) { delete _mux; }

void Server::run() {
	_mux = new PollMultiplexer();

	_config._listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_config._listenFd == -1)
		throw IrcException("socket() failed");

	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_config._port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(_config._listenFd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		throw IrcException("bind() failed");

	if (listen(_config._listenFd, 10) == -1)
		throw IrcException("listen() failed");

	_mux->watch(_config._listenFd);

	while (1) {
		std::vector<Event> events;

		if (_mux->wait(events) == -1)
			throw IrcException("poll() failed");

		for (size_t i = 0; i < events.size(); i++) {
			Event &e = events[i];

			if (e.closed)
				disconnectClient(e.fd);
			else if (e.readable && e.fd == _config._listenFd)
				acceptClient();
			else if (e.readable) {
				std::map<int, Client *>::iterator it = _clients.find(e.fd);
				if (it == _clients.end())
					continue;

				Client *client = it->second;
				char buffer[512];
				ssize_t dataIn = recv(e.fd, buffer, sizeof(buffer), 0);

				if (dataIn <= 0)
					disconnectClient(e.fd);
				else {
					std::string sDataIn(buffer, dataIn);
					client->appendInput(sDataIn);

					// gestion input???
				}
			} else if (e.writable) {
				std::map<int, Client *>::iterator it = _clients.find(e.fd);
				if (it == _clients.end())
					continue;

				Client *client = it->second;
				char buffer[512];
				ssize_t dataOut = send(e.fd, buffer, sizeof(buffer), 0);
				if (dataOut == -1)
					disconnectClient(e.fd);
				else {
					// gestion outPut
				}
			}
		}
	}
}

void Server::acceptClient() {
	struct sockaddr_in clientAddr = {};
	socklen_t clientLen = sizeof(clientAddr);

	int clientFd =
		accept(_config._listenFd, (struct sockaddr *)&clientAddr, &clientLen);
	if (clientFd == -1)
		throw IrcException("accept() failed");

	_clients[clientFd] = new Client(clientFd);

	_mux->watch(clientFd);
}

void Server::disconnectClient(int fd) {
	std::map<int, Client *>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;

	_mux->unwatch(fd);
	close(fd);

	delete it->second;
	_clients.erase(it);
}

Client *Server::getClientByNick(std::string nick) {
	(void)nick;
	return (0);
}

Channel *Server::getChannelByName(std::string name) {
	(void)name;
	return (0);
}