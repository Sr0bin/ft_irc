/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:38:13 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/22 16:50:24 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "CommandDispatcher.hpp"

Server::Server(void) : _mux(0), _dispatcher(0) {}

Server::Server(serverConfig config)
	: _config(config), _mux(0), _dispatcher(0) {}

Server::~Server(void) {
	delete _mux;
	delete _dispatcher;
	for (std::map<int, Client *>::iterator it = _clients.begin();
		 it != _clients.end(); ++it)
		delete it->second;
	for (std::map<std::string, Channel *>::iterator it = _channels.begin();
		 it != _channels.end(); ++it)
		delete it->second;
}

void Server::run() {
	_mux = new PollMultiplexer();
	_dispatcher = new CommandDispatcher(*this);

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

					std::string line;

					while (client->extractLine(line)) {
						Message msg = Parser::parseRawMessage(line);
						// Parser => Renvoyer un Message
						_dispatcher->dispatch(*client, msg);
					}

					if (client->hasPendingOutput())
						_mux->setWriteInterest(e.fd, true);
				}
			} else if (e.writable) {
				std::map<int, Client *>::iterator it = _clients.find(e.fd);
				if (it == _clients.end())
					continue;

				Client *client = it->second;
				std::string &out = client->getOutBuffer();

				ssize_t dataOut = send(e.fd, out.c_str(), out.size(), 0);
				if (dataOut == -1)
					disconnectClient(e.fd);
				else {
					out.erase(0, static_cast<size_t>(dataOut));
					if (!client->hasPendingOutput())
						_mux->setWriteInterest(e.fd, false);
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

	// Remove from every channel first, otherwise channels keep a dangling
	// Client* in their member/operator/invite sets after the delete below.
	removeClientFromAllChannels(*it->second);
	delete it->second;
	_clients.erase(it);
}

Client *Server::getClientByNick(std::string nick) {
	std::map<int, Client *>::iterator it;

	for (it = _clients.begin(); it != _clients.end(); ++it) {
		Client *client = it->second;

		if (Utils::ircEquals(client->getNickName(), nick)) {
			return (client);
		}
	}
	return (0);
}

Channel *Server::getChannelByName(std::string name) {
	std::map<std::string, Channel *>::iterator it;

	for (it = _channels.begin(); it != _channels.end(); ++it)
		if (Utils::ircEquals(it->first, name))
			return (it->second);

	return (0);
}

std::string Server::getServerName(void) const { return (_config._serverName); }

std::string Server::getPassword(void) const { return (_config._password); }

Channel *Server::addClientToChannel(Client &client, const std::string &name) {
	Channel *ch = getChannelByName(name);
	bool created = false;

	if (ch == 0) {
		ch = new Channel(name);
		_channels[name] = ch;
		created = true;
	}
	ch->addMember(client);
	client.addChannel(ch);
	if (created) // first joiner of a brand-new channel becomes its operator
		ch->promote(client);
	return (ch);
}

void Server::removeClientFromChannel(Client &client, const std::string &name) {
	Channel *ch = getChannelByName(name);
	if (ch == 0)
		return;

	ch->removeMember(client);
	client.removeChannel(ch);
	if (ch->isEmpty())
		destroyChannel(ch);
}

void Server::removeClientFromAllChannels(Client &client) {
	// Copy: removeChannel() mutates the client's channel set as we iterate.
	std::set<Channel *> channels = client.getChannels();

	for (std::set<Channel *>::iterator it = channels.begin();
		 it != channels.end(); ++it) {
		Channel *ch = *it;
		ch->removeMember(client);
		client.removeChannel(ch);
		if (ch->isEmpty())
			destroyChannel(ch);
	}
}

void Server::destroyChannel(Channel *channel) {
	for (std::map<std::string, Channel *>::iterator it = _channels.begin();
		 it != _channels.end(); ++it) {
		if (it->second == channel) {
			delete channel;
			_channels.erase(it);
			return;
		}
	}
}
