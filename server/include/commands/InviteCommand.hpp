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

#ifndef INVITECOMMAND_HPP
#define INVITECOMMAND_HPP

#include "ACommand.hpp"
#include "../Client.hpp"
#include "../Message.hpp"

class InviteCommand : public ACommand
{
	public:
		InviteCommand(Server &server);
		~InviteCommand();

		void execute(Client &client, Message &msg);
		size_t minParams() const;
};

#endif
