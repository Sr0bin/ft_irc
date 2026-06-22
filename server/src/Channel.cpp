/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:38:07 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/22 16:48:22 by rorollin         ###   ########.fr       */
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

bool Channel::canJoin(Client &client, const std::string &pass) {
	// IL FAUT RETOURNER PLUSIEURS CODES DEFAUTS SUIVANT LES CAS
	if (_parameters._inviteOnly && _invited.count(&client) == 0)
		return (false);
	if (!_parameters._pass.empty() && pass != _parameters._pass)
		return (false);
	if (_parameters._userLimit != 0 &&
		_members.size() >= _parameters._userLimit)
		return (false);
	return (true);
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
