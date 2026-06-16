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
	virtual void execute(Client, Message) = 0;
	virtual size_t requiresRegistration();
protected:
	Server &_server;
};

#endif
