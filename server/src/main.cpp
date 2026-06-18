/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 14:17:32 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/18 12:00:22 by prigaudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IrcException.hpp"
#include "../include/Server.hpp"
#include "../include/ft_irc.hpp"
#include <iostream>

int main(int argc, char **argv) {
  serverConfig config;

  config._port = atoi(argv[1]);
  config._password = argv[2];
  config._listenFd = -1;

  if (config._port <= 0 || config._port > 65535) {
    std::cout << "Error: invalid port" << std::endl;
    return (1);
  }

  try {
    Server server(config);
    server.run();
  } catch (const IrcException &e) {
    std::cout << "IRC Error: " << e.what() << std::endl;
    return (1);
  } catch (const std::exception &e) {
    std::cout << "Error: " << e.what() << std::endl;
    return (1);
  }
  std::cout << "ft_irc\n";
}
