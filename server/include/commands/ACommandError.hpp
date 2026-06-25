/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ACommandError.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 by rorollin                  #+#    #+#             */
/*                                                                            */
/* ************************************************************************** */

#ifndef ACOMMANDERROR_HPP
#define ACOMMANDERROR_HPP
#include "IrcException.hpp"
#include "Message.hpp"
#include <string>
#include <vector>

// Base for all IRC numeric-reply errors. Thrown from command execution, caught
// in CommandDispatcher::dispatch. Carries the numeric code + its params + the
// trailing text. toMessage(server, nick) turns that data into a Message; the
// dispatcher serializes it. server/nick are passed in (plain strings), so this
// class stays decoupled from Server and Client.
class ACommandError : public IrcException {
  public:
	ACommandError(int code, const std::string &text) throw();
	ACommandError(int code, const std::string &param,
				  const std::string &text) throw();
	ACommandError(int code, const std::string &param1,
				  const std::string &param2, const std::string &text) throw();

	virtual ~ACommandError(void) throw();

	// Pure data: builds the numeric Message from code/params/text. The caller
	// (dispatcher) supplies server + nick, so the exception stays decoupled
	// from Server/Client. Formatting itself lives in Message::serialize.
	Message toMessage(const std::string &server, const std::string &nick) const;

  protected:
	int _code;
	std::vector<std::string> _params;
	std::string _text;
};

class NeedMoreParams : public ACommandError {
  public:
	NeedMoreParams(const std::string &command) throw();
};

class NoSuchNick : public ACommandError {
  public:
	NoSuchNick(const std::string &nick) throw();
};

class NoSuchChannel : public ACommandError {
  public:
	NoSuchChannel(const std::string &channel) throw();
};

class UnknownCommand : public ACommandError {
  public:
	UnknownCommand(const std::string &command) throw();
};

class NotRegistered : public ACommandError {
  public:
	NotRegistered(void) throw();
};

class AlreadyRegistered : public ACommandError {
  public:
	AlreadyRegistered(void) throw();
};

class PasswordMismatch : public ACommandError {
  public:
	PasswordMismatch(void) throw();
};

class NicknameInUse : public ACommandError {
  public:
	NicknameInUse(const std::string &nick) throw();
};

class NoNicknameGiven : public ACommandError {
  public:
	NoNicknameGiven(void) throw();
};

class ErroneousNickname : public ACommandError {
  public:
	ErroneousNickname(const std::string &nick) throw();
};

class NotOnChannel : public ACommandError {
  public:
	NotOnChannel(const std::string &channel) throw();
};

class UnknownMode : public ACommandError {
  public:
	UnknownMode(const std::string &mode) throw();
};

class ChanOPrivsNeeded : public ACommandError {
  public:
	ChanOPrivsNeeded(const std::string &channel) throw();
};

class UserNotInChannel : public ACommandError {
  public:
	UserNotInChannel(const std::string &nick,
					 const std::string &channel) throw();
};

class ChannelIsFull : public ACommandError {
  public:
	ChannelIsFull(const std::string &channel) throw();
};

class InviteOnly : public ACommandError {
  public:
	InviteOnly(const std::string &channel) throw();
};

class BadChannelKey : public ACommandError {
  public:
	BadChannelKey(const std::string &channel) throw();
};

class NoRecipient : public ACommandError {
  public:
	NoRecipient(const std::string &command) throw();
};

class CannotSendToChan : public ACommandError {
  public:
	CannotSendToChan(const std::string &command) throw();
};

class UserOnChannel : public ACommandError {
  public:
	UserOnChannel(const std::string &nick, const std::string &channel) throw();
};

class NoTextToSend : public ACommandError {
  public:
	NoTextToSend(const std::string &command) throw();
};

#endif
