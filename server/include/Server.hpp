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
#include <fcntl.h>
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
	Client *getClientByNick(const std::string &nick);
	Channel *getChannelByName(const std::string &name);
	std::string getServerName(void) const;
	std::string getPassword(void) const;
	Channel *addClientToChannel(Client &client, const std::string &name);
	void removeClientFromChannel(Client &client, const std::string &name);
	void removeClientFromAllChannels(Client &client);

  private:
	// No copy allowed
	Server(const Server &other);
	Server &operator=(const Server &other);

	// Erases a channel from the registry by its lower-cased name key, frees it.
	void destroyChannel(Channel *channel);

	// run() broken into its logical phases (see run() for the boot sequence).
	void setupListenSocket();      // socket -> bind -> listen -> watch
	void eventLoop();              // the single poll() loop
	void handleReadable(int fd);   // recv -> buffer -> extract lines -> dispatch
	void handleWritable(int fd);   // flush a client's pending output
	void updateWriteInterest();    // (re)arm POLLOUT for clients with output

	serverConfig _config;
	std::map<int, Client *> _clients;
	std::map<std::string, Channel *> _channels;
	AMultiplexer *_mux;
	CommandDispatcher *_dispatcher;
};

#endif
