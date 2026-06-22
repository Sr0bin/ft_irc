/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:15:56 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/22 15:50:22 by prigaudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include "Client.hpp"
#include "ft_irc.hpp"

class Channel {
  public:
	Channel(void);
	Channel(std::string name);
	~Channel(void);

	void addMember(Client &client);
	void removeMember(Client &client);
	bool isMember(Client &client) const;
	bool isOperator(Client &client) const;
	void promote(Client &client);
	void demote(Client &client);
	bool canJoin(Client &client, std::string pass);
	void broadcast(std::string msg, Client &except);
	bool isEmpty(void) const;
	std::string getTopic(void) const;
	const std::set<Client *> &getMembers(void) const;
	const std::set<Client *> &getOperators(void) const;

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
