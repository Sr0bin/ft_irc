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

#include "../include/randomCmd.hpp"

#include <iostream>
#include <ostream>

randomCmd::randomCmd(Server &server)
	: ACommand(server)
{
}

randomCmd::~randomCmd()
{
}

void randomCmd::execute(Client &client, Message &msg)
{
	(void)client;
	(void)msg;

	std::cout << "test randomCmd" << std::endl;
}