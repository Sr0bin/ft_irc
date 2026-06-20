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
	std::string welcome;

	// TODO: this 001-004 string-building is ugly, rework later (helper/formatter?).
	// ponytail: version/creation/mode strings are cosmetic placeholders; refine if the reference client complains
	welcome += ":" + server + " 001 " + nick + " :Welcome to the Internet Relay Network " + client.prefix() + "\r\n";
	welcome += ":" + server + " 002 " + nick + " :Your host is " + server + ", running version ft_irc-1.0\r\n";
	welcome += ":" + server + " 003 " + nick + " :This server was created at startup\r\n";
	welcome += ":" + server + " 004 " + nick + " " + server + " ft_irc-1.0 o itkol\r\n";
	client.queueReply(welcome);
}
