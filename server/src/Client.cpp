/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:38:10 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/22 12:54:04 by prigaudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(void) : _fd(-1) { _clientInfo._state = CONNECTING; }

Client::Client(int fd) : _fd(fd) { _clientInfo._state = CONNECTING; }

Client::~Client(void) {}

void Client::appendInput(const std::string &input) { _inBuffer += input; }

bool Client::extractLine(std::string &out) {
	std::string::size_type pos = _inBuffer.find("\r\n");

	if (pos == std::string::npos)
		return (false);
	out = _inBuffer.substr(0, pos);
	_inBuffer.erase(0, pos + 2);
	return (true);
}

void Client::queueReply(const std::string &msg) { _outBuffer += msg; }

bool Client::hasPendingOutput(void) const { return (!_outBuffer.empty()); }

bool Client::isRegistered(void) const {
	// ponytail: stays false until the command layer sets _state = REGISTERED
	// (PASS+NICK+USER)
	return (_clientInfo._state == REGISTERED);
}

std::string Client::prefix(void) const {
	// ponytail: hardcoded host, replace once Server captures the peer address
	// (getpeername)
	return (_clientInfo._nickname + "!" + _clientInfo._username + "@localhost");
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

clientState Client::getState(void) const { return (_clientInfo._state); }

void Client::setState(clientState state) { _clientInfo._state = state; }

std::string &Client::getOutBuffer(void) { return (_outBuffer); }

void Client::addChannel(Channel *channel) { _channels.insert(channel); }

void Client::removeChannel(Channel *channel) { _channels.erase(channel); }