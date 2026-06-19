/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:38:10 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/19 12:20:31 by prigaudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(void) : _fd(-1) {}

Client::Client(int fd) : _fd(fd) {}

Client::~Client(void) {}

void Client::appendInput(const std::string &input) { (void)input; }

bool Client::extractLine(std::string &out) {
	(void)out;
	return (false);
}

void Client::queueReply(const std::string &msg) { (void)msg; }

bool Client::hasPendingOutput(void) const { return (false); }

bool Client::isRegistered(void) const { return (false); }

std::string Client::prefix(void) const { return (std::string()); }

std::string Client::getNickName(void) const { return (_clientInfo._nickname); }