/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ACommand.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:38:05 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 16:38:23 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "ACommand.hpp"
#include "Server.hpp"

ACommand::ACommand(Server &server) : _server(server)
{
}

ACommand::~ACommand(void)
{
}

bool ACommand::requiresRegistration(void) const
{
	return (true);
}

size_t ACommand::minParams(void) const
{
	return (0);
}

void ACommand::completeRegistrationIfReady(Client &client)
{
	if (client.getState() != PASSWORD_OK)
		return;
	if (client.getNickName().empty() || client.getUserName().empty())
		return;

	client.setState(REGISTERED);

	const std::string server = _server.getServerName();
	const std::string nick = client.getNickName();
	// ponytail: version/creation/mode strings are cosmetic placeholders; refine if the reference client complains
	std::vector<std::string> none;
	std::string welcome;

	welcome += Message::numeric(server, 1, nick, none,
		"Welcome to the Internet Relay Network " + client.prefix()).serialize();
	welcome += Message::numeric(server, 2, nick, none,
		"Your host is " + server + ", running version ft_irc-1.0").serialize();
	welcome += Message::numeric(server, 3, nick, none,
		"This server was created at startup").serialize();

	std::vector<std::string> p004;
	p004.push_back(server);
	p004.push_back("ft_irc-1.0");
	p004.push_back("o");
	p004.push_back("itkol");
	welcome += Message::numeric(server, 4, nick, p004).serialize();

	client.queueReply(welcome);
}
