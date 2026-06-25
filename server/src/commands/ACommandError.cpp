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

ACommandError::ACommandError(int code, const std::string &text) throw()
	: IrcException(text), _code(code), _text(text) {}

ACommandError::ACommandError(int code, const std::string &param,
							 const std::string &text) throw()
	: IrcException(text), _code(code), _text(text) {
	_params.push_back(param);
}

ACommandError::ACommandError(int code, const std::string &param1,
							 const std::string &param2,
							 const std::string &text) throw()
	: IrcException(text), _code(code), _text(text) {
	_params.push_back(param1);
	_params.push_back(param2);
}

ACommandError::~ACommandError(void) throw() {}

Message ACommandError::toMessage(const std::string &server,
								 const std::string &nick) const {
	return (Message::numeric(server, _code, nick, _params, _text));
}

NeedMoreParams::NeedMoreParams(const std::string &command) throw()
	: ACommandError(461, command, "Not enough parameters") {}

NoSuchNick::NoSuchNick(const std::string &nick) throw()
	: ACommandError(401, nick, "No such nick/channel") {}

NoSuchChannel::NoSuchChannel(const std::string &channel) throw()
	: ACommandError(403, channel, "No such channel") {}

UnknownCommand::UnknownCommand(const std::string &command) throw()
	: ACommandError(421, command, "Unknown command") {}

NotRegistered::NotRegistered(void) throw()
	: ACommandError(451, "You have not registered") {}

AlreadyRegistered::AlreadyRegistered(void) throw()
	: ACommandError(462, "You may not reregister") {}

PasswordMismatch::PasswordMismatch(void) throw()
	: ACommandError(464, "Password incorrect") {}

NicknameInUse::NicknameInUse(const std::string &nick) throw()
	: ACommandError(433, nick, "Nickname is already in use") {}

NoNicknameGiven::NoNicknameGiven(void) throw()
	: ACommandError(431, "No nickname given") {}

ErroneousNickname::ErroneousNickname(const std::string &nick) throw()
	: ACommandError(432, nick, "Erroneous nickname") {}

NotOnChannel::NotOnChannel(const std::string &channel) throw()
	: ACommandError(442, channel, "You're not on that channel") {}

ChannelIsFull::ChannelIsFull(const std::string &channel) throw()
	: ACommandError(471, channel, "Cannot join channel (+l)") {}

InviteOnly::InviteOnly(const std::string &channel) throw()
	: ACommandError(473, channel, "Cannot join channel (+i)") {}

BadChannelKey::BadChannelKey(const std::string &channel) throw()
	: ACommandError(475, channel, "Cannot join channel (+k)") {}

NotChanOp::NotChanOp(const std::string &channel) throw()
	: ACommandError(482, channel, "You're not channel operator") {}

UserNotInChannel::UserNotInChannel(const std::string &nick,
								   const std::string &channel) throw()
	: ACommandError(441, nick, channel, "They aren't on that channel") {}
