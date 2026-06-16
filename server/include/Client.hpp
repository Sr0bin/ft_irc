/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:08:46 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 16:19:58 by rorollin         ###   ########.fr       */
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
	Client(const Client &other);
	Client &operator=(const Client &other);
	~Client(void);

private:
	int _fd;
	std::string _inBuffer;
	std::string _outBuffer;
	clientInfo _clientInfo;
	std::set<Channel *> _channels;
	
};

#endif
