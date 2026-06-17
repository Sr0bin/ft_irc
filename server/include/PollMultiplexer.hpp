/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollMultiplexer.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 19:44:06 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/17 19:45:46 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef POLLMULTIPLEXER_HPP
# define POLLMULTIPLEXER_HPP
# include "AMultiplexer.hpp"

class PollMultiplexer : public AMultiplexer
{
public:
	PollMultiplexer(void);
	virtual void watch(int fd);
	virtual void unwatch(int fd);
	virtual void setWriteInterest(int fd, bool on);
	virtual int wait(std::vector<Event> &out);
	virtual ~PollMultiplexer(void);

private:
	// No Copy Allowed
	PollMultiplexer(const PollMultiplexer &other);
	PollMultiplexer &operator=(const PollMultiplexer &other);
	
};

#endif
