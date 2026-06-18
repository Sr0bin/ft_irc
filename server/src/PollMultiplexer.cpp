/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollMultiplexer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 19:44:12 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/18 14:27:24 by prigaudi         ###   ########.fr       */
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
  (void)fd;
  (void)on;
}

int PollMultiplexer::wait(std::vector<Event> &out) {
  (void)out;
  return (0);
}
