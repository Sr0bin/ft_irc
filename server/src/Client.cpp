/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:38:10 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/22 16:49:47 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Utils.hpp"
#include <sstream>

Client::Client(int fd) : _fd(fd) { _clientInfo._state = CONNECTING; }

Client::~Client(void) {}

void Client::appendInput(const std::string &input) {
	_inBuffer += input;

	// RFC 1459 caps a message at 512 bytes (CRLF included). A client that streams
	// more than that with no line terminator would grow _inBuffer without bound
	// (memory-exhaustion DoS). The line is invalid anyway, so drop the unterminated
	// overflow — but keep the client connected.
	if (_inBuffer.size() > 512 && _inBuffer.find("\r\n") == std::string::npos)
		_inBuffer.clear();
}

bool Client::extractLine(std::string &out) {
	std::string::size_type pos = _inBuffer.find("\r\n");

	if (pos == std::string::npos)
		return (false);
	out = _inBuffer.substr(0, pos);
	_inBuffer.erase(0, pos + 2);
	return (true);
}

std::string Client::tag(void) const {
	std::ostringstream os;
	os << "[fd " << _fd;
	if (!_clientInfo._nickname.empty())
		os << " | " << _clientInfo._nickname;
	os << "]";
	return (os.str());
}

void Client::queueReply(const std::string &msg) {
	_outBuffer += msg;

	// queueReply is the single chokepoint for everything we send (direct replies
	// and channel broadcasts), so log here. A burst may carry several CRLF lines
	// (e.g. the 001-004 welcome): frame each one on its own.
	std::istringstream stream(msg);
	std::string line;
	while (std::getline(stream, line)) {
		std::string::size_type end = line.find_last_not_of("\r\n");
		if (end == std::string::npos)
			continue; // blank line
		line.erase(end + 1);
		Utils::log(LOG_OUT, tag() + " " + line);
	}
}

bool Client::hasPendingOutput(void) const { return (!_outBuffer.empty()); }

bool Client::isRegistered(void) const {
	// ponytail: stays false until the command layer sets _state = REGISTERED
	// (PASS+NICK+USER)
	return (_clientInfo._state == REGISTERED);
}

std::string Client::prefix(void) const {
	// Host is captured at accept; fall back to "localhost" before it's set.
	const std::string host =
		_clientInfo._host.empty() ? "localhost" : _clientInfo._host;
	return (_clientInfo._nickname + "!" + _clientInfo._username + "@" + host);
}

std::string Client::getNickName(void) const { return (_clientInfo._nickname); }

std::string Client::getUserName(void) const { return (_clientInfo._username); }

void Client::setNickName(const std::string &nick) {
	_clientInfo._nickname = nick;
}

void Client::setUserName(const std::string &user) {
	_clientInfo._username = user;
}

void Client::setRealName(const std::string &real) {
	_clientInfo._realname = real;
}

void Client::setHost(const std::string &host) { _clientInfo._host = host; }

clientState Client::getState(void) const { return (_clientInfo._state); }


const std::set<Channel *> &Client::getChannels(void) const
{
	return (_channels);
}
void Client::setState(clientState state) { _clientInfo._state = state; }

std::string &Client::getOutBuffer(void) { return (_outBuffer); }

void Client::addChannel(Channel *channel) { _channels.insert(channel); }

void Client::removeChannel(Channel *channel) { _channels.erase(channel); }

int Client::getFd() const { return (_fd); }