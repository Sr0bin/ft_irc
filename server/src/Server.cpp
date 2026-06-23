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
#include <arpa/inet.h>
#include <sstream>

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
	std::ostringstream boot;
	boot << "starting '" << _config._serverName << "' on port "
		 << _config._port;
	Utils::log(LOG_BOOT, boot.str());

	_mux = new PollMultiplexer();
	_dispatcher = new CommandDispatcher(*this);
	Utils::log(LOG_BOOT, "poll multiplexer + command dispatcher ready");

	setupListenSocket();

	Utils::log(LOG_BOOT, "entering event loop");
	eventLoop();
}

void Server::setupListenSocket() {
	_config._listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_config._listenFd == -1)
		throw IrcException("socket() failed");
	fcntl(_config._listenFd, F_SETFL, O_NONBLOCK);

	std::ostringstream sock;
	sock << "listen socket created (fd " << _config._listenFd
		 << "), non-blocking";
	Utils::log(LOG_BOOT, sock.str());

	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_config._port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(_config._listenFd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		throw IrcException("bind() failed");
	Utils::log(LOG_BOOT, "bound to 0.0.0.0");

	if (listen(_config._listenFd, 10) == -1)
		throw IrcException("listen() failed");
	Utils::log(LOG_BOOT, "listening (backlog 10)");

	_mux->watch(_config._listenFd);
}

void Server::eventLoop() {
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
			else if (e.readable)
				handleReadable(e.fd);
			else if (e.writable)
				handleWritable(e.fd);
		}

		updateWriteInterest();
	}
}

void Server::handleReadable(int fd) {
	std::map<int, Client *>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;

	Client *client = it->second;
	char buffer[512];
	ssize_t dataIn = recv(fd, buffer, sizeof(buffer), 0);

	if (dataIn <= 0) {
		disconnectClient(fd);
		return;
	}

	client->appendInput(std::string(buffer, dataIn));

	std::string line;
	while (client->extractLine(line)) {
		Utils::log(LOG_IN, client->tag() + " " + line);
		Message msg = Parser::parseRawMessage(line);
		_dispatcher->dispatch(*client, msg);
	}
}

void Server::handleWritable(int fd) {
	std::map<int, Client *>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;

	Client *client = it->second;
	std::string &out = client->getOutBuffer();

	ssize_t dataOut = send(fd, out.c_str(), out.size(), 0);
	if (dataOut == -1)
		disconnectClient(fd);
	else
		out.erase(0, static_cast<size_t>(dataOut));
}

void Server::updateWriteInterest() {
	// A command may have queued output on clients other than the one we just
	// read from (e.g. broadcast to channel members). Single pass: (re)arm
	// POLLOUT for every client with pending output, disarm the rest. Covers
	// broadcasts and direct replies with one mechanism.
	// NOTE: O(n^2) per cycle (setWriteInterest scans _fds linearly for each
	// client) — fine at our scale, worth revisiting if client count grows
	// (e.g. an fd->index map in PollMultiplexer).
	for (std::map<int, Client *>::iterator it = _clients.begin();
		 it != _clients.end(); ++it)
		_mux->setWriteInterest(it->first, it->second->hasPendingOutput());
}

void Server::acceptClient() {
	struct sockaddr_in clientAddr = {};
	socklen_t clientLen = sizeof(clientAddr);

	int clientFd =
		accept(_config._listenFd, (struct sockaddr *)&clientAddr, &clientLen);
	// poll() reporting the listen fd readable is a point-in-time hint, not a
	// guarantee: the pending connection may be aborted (ECONNABORTED) or gone
	// (EAGAIN) by the time we accept, or we may be out of fds (EMFILE). None of
	// these is fatal — skip and retry on the next poll cycle instead of killing
	// the server. (errno can't be checked here per the subject anyway.)
	if (clientFd == -1)
		return;
	fcntl(clientFd, F_SETFL, O_NONBLOCK);

	_clients[clientFd] = new Client(clientFd);

	_mux->watch(clientFd);

	std::ostringstream tag;
	tag << "[fd " << clientFd << "]";
	Utils::log(LOG_CONN, tag.str() + " connect from " +
							  inet_ntoa(clientAddr.sin_addr));
}

void Server::disconnectClient(int fd) {
	std::map<int, Client *>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;

	Utils::log(LOG_CONN, it->second->tag() + " disconnect");

	_mux->unwatch(fd);
	close(fd);

	// Remove from every channel first, otherwise channels keep a dangling
	// Client* in their member/operator/invite sets after the delete below.
	removeClientFromAllChannels(*it->second);
	delete it->second;
	_clients.erase(it);
}

Client *Server::getClientByNick(const std::string &nick) {
	std::map<int, Client *>::iterator it;

	for (it = _clients.begin(); it != _clients.end(); ++it) {
		Client *client = it->second;

		if (Utils::ircEquals(client->getNickName(), nick)) {
			return (client);
		}
	}
	return (0);
}

Channel *Server::getChannelByName(const std::string &name) {
	// Keys are stored lower-cased (see addClientToChannel) so an O(log n)
	// map lookup is enough for case-insensitive matching.
	std::map<std::string, Channel *>::iterator it =
		_channels.find(Utils::toLower(name));

	if (it == _channels.end())
		return (0);
	return (it->second);
}

std::string Server::getServerName(void) const { return (_config._serverName); }

std::string Server::getPassword(void) const { return (_config._password); }

Channel *Server::addClientToChannel(Client &client, const std::string &name) {
	Channel *ch = getChannelByName(name);
	bool created = false;

	if (ch == 0) {
		ch = new Channel(name);
		_channels[Utils::toLower(name)] = ch;
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
	_channels.erase(Utils::toLower(channel->getName()));
	delete channel;
}
