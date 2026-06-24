/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:15:56 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/24 16:20:35 by prigaudi         ###   ########.fr       */
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
