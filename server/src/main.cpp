/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 14:17:32 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/23 15:47:49 by prigaudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FatalException.hpp"
#include "IrcException.hpp"
#include "Server.hpp"
#include "ft_irc.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return (1);
	}

	serverConfig config;

	config._port = atoi(argv[1]);
	config._password = argv[2];
	config._listenFd = -1;
	config._serverName = "ircserv.42.fr";

	if (config._port <= 0 || config._port > 65535) {
		std::cerr << "Error: invalid port" << std::endl;
		return (1);
	}

	try {
		Server server(config);
		server.run();
	} catch (const FatalException &e) {
		std::cerr << "Fatal: " << e.what() << std::endl;
		return (1);
	} catch (const IrcException &e) {
		std::cerr << "IRC Error: " << e.what() << std::endl;
		return (1);
	} catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
