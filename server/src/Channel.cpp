/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:38:07 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/23 18:28:47 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

static channelParam defaultParam(void) {
	channelParam p;

	p._pass = "";
	p._userLimit = 0;
	p._inviteOnly = false;
	p._topicRestricted = false;
	return (p);
}

Channel::Channel(void) : _parameters(defaultParam()) {}

Channel::Channel(const std::string &name)
	: _name(name), _parameters(defaultParam()) {}

Channel::~Channel(void) {}

void Channel::addMember(Client &client) { _members.insert(&client); }

void Channel::removeMember(Client &client) {
	_members.erase(&client);
	_operators.erase(&client);
	_invited.erase(&client);
}

bool Channel::isMember(Client &client) const {
	return (_members.count(&client) != 0);
}

bool Channel::isOperator(Client &client) const {
	return (_operators.count(&client) != 0);
}

void Channel::promote(Client &client) { _operators.insert(&client); }

void Channel::demote(Client &client) { _operators.erase(&client); }

bool Channel::setInviteOnly(bool v) {
	if (_parameters._inviteOnly == v)
		return (false);
	_parameters._inviteOnly = v;
	return (true);
}

bool Channel::setTopicRestricted(bool v) {
	if (_parameters._topicRestricted == v)
		return (false);
	_parameters._topicRestricted = v;
	return (true);
}

bool Channel::setKey(const std::string &key) {
	if (_parameters._pass == key)
		return (false);
	_parameters._pass = key;
	return (true);
}

bool Channel::removeKey(void) {
	if (_parameters._pass.empty())
		return (false);
	_parameters._pass = "";
	return (true);
}

bool Channel::setUserLimit(size_t limit) {
	if (_parameters._userLimit == limit)
		return (false);
	_parameters._userLimit = limit;
	return (true);
}

bool Channel::removeUserLimit(void) {
	if (_parameters._userLimit == 0)
		return (false);
	_parameters._userLimit = 0;
	return (true);
}

bool Channel::isInviteOnly(void) const { return (_parameters._inviteOnly); }

bool Channel::isTopicRestricted(void) const {
	return (_parameters._topicRestricted);
}

const std::string &Channel::getKey(void) const { return (_parameters._pass); }

size_t Channel::getUserLimit(void) const { return (_parameters._userLimit); }

void Channel::canJoin(Client &client, const std::string &pass) {
	if (_parameters._userLimit != 0 &&
		_members.size() >= _parameters._userLimit)
		throw(ChannelIsFull(_name));
	if (_parameters._inviteOnly && _invited.count(&client) == 0)
		throw(InviteOnly(_name));
	if (!_parameters._pass.empty() && pass != _parameters._pass)
		throw(BadChannelKey(_name));
}

void Channel::broadcast(const std::string &msg, Client &except) {
	for (std::set<Client *>::iterator it = _members.begin();
		 it != _members.end(); ++it) {
		if (*it != &except)
			(*it)->queueReply(msg);
	}
}

bool Channel::isEmpty(void) const { return (_members.empty()); }

std::string Channel::getName(void) const { return (_name); }

std::string Channel::getTopic(void) const { return (_topic); }

const std::set<Client *> &Channel::getMembers(void) const { return (_members); }

const std::set<Client *> &Channel::getOperators(void) const {
	return (_operators);
}
