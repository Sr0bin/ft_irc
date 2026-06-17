/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:38:07 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 16:38:23 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Channel.hpp"

Channel::Channel(void)
{
}

Channel::Channel(std::string name) : _name(name)
{
}

Channel::~Channel(void)
{
}

void Channel::addMember(Client &client)
{
	(void)client;
}

void Channel::removeMember(Client &client)
{
	(void)client;
}

bool Channel::isMember(Client &client) const
{
	(void)client;
	return (false);
}

bool Channel::isOperator(Client &client) const
{
	(void)client;
	return (false);
}

void Channel::promote(Client &client)
{
	(void)client;
}

void Channel::demote(Client &client)
{
	(void)client;
}

bool Channel::canJoin(Client &client, std::string pass)
{
	(void)client;
	(void)pass;
	return (false);
}

void Channel::broadcast(std::string msg, Client &except)
{
	(void)msg;
	(void)except;
}
