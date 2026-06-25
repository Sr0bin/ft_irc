/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*                                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpirotti <vpirotti@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   GitHub : @Nave20                                  #+#    #+#             */
/*   28 is the new 42                                 ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRIVMSGCOMMAND_HPP
#define PRIVMSGCOMMAND_HPP

#include "ACommand.hpp"
#include "../Client.hpp"
#include "../Message.hpp"

class PrivmsgCommand : public ACommand
{
	public:
		PrivmsgCommand(Server &server);
		~PrivmsgCommand();

		void execute(Client &client, Message &msg);
		size_t minParams() const;
		bool requiresRegistration() const;
};

#endif