/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:15:56 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/25 10:48:27 by prigaudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include "ACommandError.hpp"
#include "Client.hpp"
#include "Utils.hpp"
#include "ft_irc.hpp"

class Channel {
  public:
	Channel(void);
	Channel(const std::string &name);
	~Channel(void);

	void addMember(Client &client);
	void removeMember(Client &client);
	bool isMember(Client &client) const;
	bool isOperator(Client &client) const;
	void promote(Client &client);
	void demote(Client &client);

	// Mode setters: return true if the channel state actually changed (used to
	// build the RFC "changes which resulted" echo). Setters never do any I/O.
	bool setInviteOnly(bool v);
	bool setTopicRestricted(bool v);
	bool setKey(const std::string &key);
	bool removeKey(void);
	bool setUserLimit(size_t limit);
	bool removeUserLimit(void);

	bool isInviteOnly(void) const;
	bool isTopicRestricted(void) const;
	const std::string &getKey(void) const;
	size_t getUserLimit(void) const;
	void canJoin(Client &client, const std::string &pass);
	void broadcast(const std::string &msg, Client &except);
	std::string getName(void) const;
	bool isEmpty(void) const;
	std::string getTopic(void) const;
	const std::set<Client *> &getMembers(void) const;
	const std::set<Client *> &getOperators(void) const;
	bool isTopicRestricted(void) const;
	void setTopic(std::string &topic);

  private:
	// No copy allowed
	Channel(const Channel &other);
	Channel &operator=(const Channel &other);

	std::string _name;
	std::string _topic;
	channelParam _parameters;
	std::set<Client *> _members;
	std::set<Client *> _operators;
	std::set<Client *> _invited;
};

#endif
