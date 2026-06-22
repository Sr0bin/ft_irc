/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ACommandError.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 by rorollin                  #+#    #+#             */
/*                                                                            */
/* ************************************************************************** */

#include "ACommandError.hpp"
#include <sstream>

ACommandError::ACommandError(int code, const std::string &text) throw()
	: IrcException(text), _code(code), _text(text)
{
}

ACommandError::ACommandError(int code, const std::string &param,
		const std::string &text) throw()
	: IrcException(text), _code(code), _text(text)
{
	_params.push_back(param);
}

ACommandError::~ACommandError(void) throw()
{
}

// "<code> <nick> <params...> :<text>" — the dispatcher prepends ":<server> ".
std::string ACommandError::buildReply(const std::string &nick) const
{
	std::ostringstream os;

	os << _code << " " << nick;
	for (size_t i = 0; i < _params.size(); ++i)
		os << " " << _params[i];
	os << " :" << _text;
	return (os.str());
}

NeedMoreParams::NeedMoreParams(const std::string &command) throw()
	: ACommandError(461, command, "Not enough parameters")
{
}

NoSuchNick::NoSuchNick(const std::string &nick) throw()
	: ACommandError(401, nick, "No such nick/channel")
{
}

NoSuchChannel::NoSuchChannel(const std::string &channel) throw()
	: ACommandError(403, channel, "No such channel")
{
}
