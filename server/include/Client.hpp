/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:08:46 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/19 11:24:37 by prigaudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "ft_irc.hpp"
#include <set>

class Channel;

class Client {
  public:
	Client(void);
	Client(int fd);
	~Client(void);
	void appendInput(const std::string &input);
	bool extractLine(std::string &out);
	void queueReply(const std::string &msg);
	bool hasPendingOutput(void) const;
	bool isRegistered(void) const;
	std::string prefix(void) const;
	std::string getNickName(void) const;
	std::string getUserName(void) const;
	void setNickName(const std::string &nick);
	void setUserName(const std::string &user);
	void setRealName(const std::string &real);
	clientState getState(void) const;
	void setState(clientState state);
	void addChannel(Channel *channel);
	void removeChannel(Channel *channel);
	const std::set<Channel *> &getChannels(void) const;

  private:
	// No copy allowed
	Client(const Client &other);
	Client &operator=(const Client &other);

	int _fd;
	std::string _inBuffer;
	std::string _outBuffer;
	clientInfo _clientInfo;
	std::set<Channel *> _channels;
};

#endif
