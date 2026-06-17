/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:15:56 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 20:48:52 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include "ft_irc.hpp"
# include "Client.hpp"

class Channel
{
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

private:

	//No copy allowed
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
