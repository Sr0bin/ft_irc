/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 15:58:43 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/22 16:46:40 by rorollin         ###   ########.fr       */
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
	Channel *addClientToChannel(Client &client, const std::string &name);
	void removeClientFromChannel(Client &client, const std::string &name);
	void removeClientFromAllChannels(Client &client);

  private:
	// No copy allowed
	Server(const Server &other);
	Server &operator=(const Server &other);

	// Removes a channel from the registry by pointer (case-insensitive lookup
	// means the map key may not match a caller's name), frees it.
	void destroyChannel(Channel *channel);

	serverConfig _config;
	std::map<int, Client *> _clients;
	std::map<std::string, Channel *> _channels;
	AMultiplexer *_mux;
	CommandDispatcher *_dispatcher;
};

#endif
