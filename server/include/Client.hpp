/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:08:46 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 20:44:37 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "ft_irc.hpp"
# include <set>

class Channel;

class Client
{
public:
	Client(void);
	Client(int fd);
	~Client(void);
	void appendInput(const std::string &input);
	bool extractLine(std::string &out);
	void queueReply(const std::string &msg);
	bool hasPendingOutput(void);
	bool isRegistered(void);
	std::string &prefix();


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
