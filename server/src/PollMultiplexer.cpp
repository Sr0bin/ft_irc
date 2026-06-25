/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollMultiplexer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 19:44:12 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/25 11:38:30 by prigaudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PollMultiplexer.hpp"

PollMultiplexer::PollMultiplexer(void) {}

PollMultiplexer::~PollMultiplexer(void) {}

void PollMultiplexer::watch(int fd) {
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_fds.push_back(pfd);
}

void PollMultiplexer::unwatch(int fd) {
	for (size_t i = 0; i < _fds.size(); i++) {
		if (_fds[i].fd == fd) {
			_fds.erase(_fds.begin() + i);
			return;
		}
	}
}

void PollMultiplexer::setWriteInterest(int fd, bool on) {
	for (size_t i = 0; i < _fds.size(); i++) {
		if (_fds[i].fd == fd) {
			if (on)
				_fds[i].events |= POLLOUT;
			else
				_fds[i].events &= ~POLLOUT;
			return;
		}
	}
}

int PollMultiplexer::wait(std::vector<Event> &out) {
	// std::vector::data() is C++11; &_fds[0] is the C++98 equivalent. Safe
	// here: poll() is never reached with an empty set (the listen fd is always
	// watched).
	int ret = poll(&_fds[0], _fds.size(), -1);
	if (ret == -1)
		return (-1);

	for (size_t i = 0; i < _fds.size(); i++) {
		if (_fds[i].revents == 0)
			continue;

		Event e;
		e.fd = _fds[i].fd;
		e.readable = _fds[i].revents & POLLIN;
		e.writable = _fds[i].revents & POLLOUT;
		e.closed = _fds[i].revents & (POLLHUP | POLLERR | POLLNVAL);
		out.push_back(e);

		_fds[i].revents = 0;
	}
	return (ret);
}
