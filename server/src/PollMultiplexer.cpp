/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollMultiplexer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 19:44:12 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/17 19:45:03 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "PollMultiplexer.hpp"

PollMultiplexer::PollMultiplexer(void)
{
}

PollMultiplexer::~PollMultiplexer(void)
{
}

void PollMultiplexer::watch(int fd)
{
	(void)fd;
}

void PollMultiplexer::unwatch(int fd)
{
	(void)fd;
}

void PollMultiplexer::setWriteInterest(int fd, bool on)
{
	(void)fd;
	(void)on;
}

int PollMultiplexer::wait(std::vector<Event> &out)
{
	(void)out;
	return (0);
}
