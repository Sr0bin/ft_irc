/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ACommand.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:20:19 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 19:14:32 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef ACOMMAND_HPP
# define ACOMMAND_HPP
# include "ft_irc.hpp"
# include "Client.hpp"
# include "Message.hpp"

class Server;

class ACommand
{
public:
	ACommand(Server &server);
	virtual ~ACommand(void);

	virtual void execute(Client &client, Message &msg) = 0;
	virtual bool requiresRegistration(void) const;
	virtual size_t minParams(void) const;

protected:
	Server &_server;

private:
	// No copy allowed
	ACommand(const ACommand &other);
	ACommand &operator=(const ACommand &other);
};

#endif
