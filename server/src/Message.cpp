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
#include <iomanip>
#include <iostream>
#include <sstream>

Message::Message(void) : _trailing(false)
{
}

Message::Message(const std::string &prefix, const std::string &command,
	const std::vector<std::string> &params, bool trailing)
	: _prefix(prefix), _command(command), _params(params), _trailing(trailing)
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
		_trailing = other._trailing;
	}
	return (*this);
}

Message::~Message(void)
{
}

std::string Message::getPrefix() const
{
	return (_prefix);
}

std::string Message::getCommand(void) const
{
	return (_command);
}

std::string Message::getParam(size_t i) const
{
	if (i >= _params.size())
		return ("");
	return (_params[i]);
}

size_t Message::paramCount(void) const
{
	return (_params.size());
}

std::string Message::serialize(void) const
{
	std::string out;

	if (!_prefix.empty())
		out += ":" + _prefix + " ";
	out += _command;
	for (size_t i = 0; i < _params.size(); ++i)
	{
		out += " ";
		if (_trailing && i + 1 == _params.size())
			out += ":";
		out += _params[i];
	}
	return (out + "\r\n");
}

Message Message::numeric(const std::string &server, int code,
	const std::string &nick, const std::vector<std::string> &middle,
	const std::string &text)
{
	std::ostringstream codeStr;
	codeStr << std::setw(3) << std::setfill('0') << code;

	std::vector<std::string> params;
	params.push_back(nick);
	for (size_t i = 0; i < middle.size(); ++i)
		params.push_back(middle[i]);
	params.push_back(text);

	return (Message(server, codeStr.str(), params, true));
}

Message Message::numeric(const std::string &server, int code,
	const std::string &nick, const std::vector<std::string> &middle)
{
	std::ostringstream codeStr;
	codeStr << std::setw(3) << std::setfill('0') << code;

	std::vector<std::string> params;
	params.push_back(nick);
	for (size_t i = 0; i < middle.size(); ++i)
		params.push_back(middle[i]);

	return (Message(server, codeStr.str(), params, false));
}

std::ostream& operator<<(std::ostream &os, const Message &msg)
{
	os << "Message {\n";
	os << "  prefix  : " << msg.getPrefix() << "\n";
	os << "  command : " << msg.getCommand() << "\n";
	os << "  params  : [";

	for (size_t i = 0; i < msg.paramCount(); i++)
	{
		os << "\"" << msg.getParam(i) << "\"";

		if (i + 1 < msg.paramCount())
			os << ", ";
	}

	os << "]\n}";
	return os;
}