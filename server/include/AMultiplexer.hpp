/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AMultiplexer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:41:47 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 19:14:32 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef AMULTIPLEXER_HPP
# define AMULTIPLEXER_HPP
# include "ft_irc.hpp"

class AMultiplexer
{
public:
	virtual ~AMultiplexer(void);

	virtual void watch(int fd) = 0;
	virtual void unwatch(int fd) = 0;
	virtual void setWriteInterest(int fd, bool on) = 0;
	virtual int wait(std::vector<Event> &out) = 0;
};

#endif
