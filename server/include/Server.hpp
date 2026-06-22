/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 15:58:43 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/22 09:53:43 by prigaudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#include "Channel.hpp"
#include "Client.hpp"
#include "CommandDispatcher.hpp"
#include "IrcException.hpp"
#include "Parser.hpp"
#include "PollMultiplexer.hpp"
#include "Utils.hpp"
#include "ft_irc.hpp"
#include <map>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

class AMultiplexer;

class Server {
  public:
	Server(void);
	Server(serverConfig config);
	~Server(void);
	void run();
	void acceptClient();
	void disconnectClient(int fd);
	Client *getClientByNick(std::string nick);
	Channel *getChannelByName(std::string name);
	std::string getServerName(void) const;
	std::string getPassword(void) const;

  private:
	// No copy allowed
	Server(const Server &other);
	Server &operator=(const Server &other);

	serverConfig _config;
	std::map<int, Client *> _clients;
	std::map<std::string, Channel *> _channels;
	AMultiplexer *_mux;
	CommandDispatcher *_dispatcher;
};

#endif
