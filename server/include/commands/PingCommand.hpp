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

#pragma once
#include "ACommand.hpp"
#include "../Client.hpp"
#include "../Message.hpp"

class PingCommand : public ACommand
{
	public:
		PingCommand(Server &server);
		~PingCommand();

		void execute(Client &client, Message &msg);
		size_t minParams() const;
		bool requiresRegistration() const;
};
