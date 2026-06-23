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

// int main()
// {
// 	Message test;
//
// 	std::string raw1 = "PING";
// 	std::string raw2 = "NICK Alice";
// 	std::string raw3 = "MODE #42 +i";
// 	std::string raw4 = "PRIVMSG Bob :Hello";
// 	std::string raw5 = "PRIVMSG Bob :Hello how are you ?";
// 	std::string raw6 = "COMMAND a b c :d e f g";
// 	std::string raw7 = "PRIVMSG Bob :";
// 	std::string raw8 = ":irc.42.fr PING";
// 	std::string raw9 = ":Alice!alice@localhost PRIVMSG Bob :Salut";
// 	std::string raw10 = ":Bob!user@host MODE #42 +o Alice";
// 	std::string raw11 = "JOIN    #42";
// 	std::string raw12 = "PRIVMSG    Bob     :Hello world";
// 	std::string raw13 = "PRIVMSG Bob :   hello";
// 	std::string raw14 = "001 Alice :Welcome to the server";
// 	std::string raw15 = ":server.example.com ERROR";
// 	std::string raw16 = "COMMAND :hello world";
// 	std::string raw17 = "COMMAND a b c d e f g h i j k l m n :last parameter";
// 	std::string raw18 = "USER alice 0 * :Alice Dupont";
// 	std::string raw19 = "TOPIC #42 :Projet IRC de 42";
// 	std::string raw20 = "PRIVMSG #42 :hello :this is not a new parameter";
//
// 	std::string tests[] = {
// 		raw1, raw2, raw3, raw4, raw5,
// 		raw6, raw7, raw8, raw9, raw10,
// 		raw11, raw12, raw13, raw14, raw15,
// 		raw16, raw17, raw18, raw19, raw20
// 	};
//
// 	for (int i = 0; i < 20; i++)
// 	{
// 		std::cout << "========== TEST " << i + 1 << " ==========" << std::endl;
// 		std::cout << "RAW: " << tests[i] << std::endl;
//
// 		test = Parser::parseRawMessage(tests[i]);
//
// 		std::cout << test << std::endl;
// 		std::cout << std::endl;
// 	}
//
// 	return 0;
// }

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
