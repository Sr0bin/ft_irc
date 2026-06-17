/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:38:14 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 16:38:23 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Message.hpp"

Message::Message(void)
{
}

Message::Message(const std::string &prefix, const std::string &command,
	const std::vector<std::string> &params)
	: _prefix(prefix), _command(command), _params(params)
{
}

Message::Message(const Message &other)
{
	*this = other;
}

Message &Message::operator=(const Message &other)
{
	if (this != &other)
	{
		_prefix = other._prefix;
		_command = other._command;
		_params = other._params;
	}
	return (*this);
}

Message::~Message(void)
{
}

std::string Message::getCommand(void) const
{
	return (_command);
}

std::string Message::getParam(size_t i) const
{
	return (_params[i]);
}

size_t Message::paramCount(void) const
{
	return (_params.size());
}
