/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcException.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:45:09 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 16:45:39 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "IrcException.hpp"

IrcException::IrcException(void) throw() : _msg("IRC error")
{
}

IrcException::IrcException(const std::string &msg) throw() : _msg(msg)
{
}

IrcException::IrcException(const IrcException &other) throw()
	: std::exception(other), _msg(other._msg)
{
}

IrcException &IrcException::operator=(const IrcException &other) throw()
{
	if (this != &other)
		_msg = other._msg;
	return (*this);
}

IrcException::~IrcException(void) throw()
{
}

const char *IrcException::what(void) const throw()
{
	return (_msg.c_str());
}
