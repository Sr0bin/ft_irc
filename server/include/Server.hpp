/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 15:58:43 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 16:41:03 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_HPP
# define SERVER_HPP
# include "ft_irc.hpp"
# include <map>
# include "Client.hpp"
# include "Channel.hpp"
# include "CommandDispatcher.hpp"

class AMultiplexer;

class Server
{
public:
	Server(void);
	Server(serverConfig config);
	Server(const Server &other);
	Server &operator=(const Server &other);
	~Server(void);

private:
	serverConfig _config;
	std::map<int, Client*> _clients;
	std::map<std::string, Channel*> _channels;
	AMultiplexer *_mux;
	CommandDispatcher *_dispatcher;
};

#endif
