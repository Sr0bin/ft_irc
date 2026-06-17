/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:38:10 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 16:38:23 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Client.hpp"

Client::Client(void) : _fd(-1)
{
}

Client::Client(int fd) : _fd(fd)
{
}

Client::~Client(void)
{
}

void Client::appendInput(const std::string &input)
{
	(void)input;
}

bool Client::extractLine(std::string &out)
{
	(void)out;
	return (false);
}

void Client::queueReply(const std::string &msg)
{
	(void)msg;
}

bool Client::hasPendingOutput(void) const
{
	return (false);
}

bool Client::isRegistered(void) const
{
	return (false);
}

std::string Client::prefix(void) const
{
	return (std::string());
}
