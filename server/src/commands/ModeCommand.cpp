/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ModeCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 15:36:19 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/23 16:21:23 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ModeCommand.hpp"
#include "ACommandError.hpp"
#include "ACommandReply.hpp"
#include "Server.hpp"
#include <cstdlib>
#include <sstream>

// Channel modes this server understands. Anything else => ERR_UNKNOWNMODE.
static const std::string KNOWN_MODES = "itkol";

// 'o' always carries an argument (the target nick), both on + and -.
// 'k' and 'l' carry an argument only when set (+); -k and -l take none.
static bool consumesArg(char mode, bool sign)
{
	return (mode == 'o' || (sign && (mode == 'k' || mode == 'l')));
}


ModeCommand::ModeCommand(Server &server) : ACommand(server)
{
	
}
size_t ModeCommand::minParams() const
{
	return (1);
}

bool ModeCommand::applyOne(Channel &ch, const ModeChange &change)
{
	switch (change.mode)
	{
	case 'i':
		return (ch.setInviteOnly(change.sign));
	case 't':
		return (ch.setTopicRestricted(change.sign));
	case 'k':
		return (change.sign ? ch.setKey(change.arg) : ch.removeKey());
	case 'l':
	{
		if (!change.sign)
			return (ch.removeUserLimit());
		char *end = 0;
		long n = std::strtol(change.arg.c_str(), &end, 10);
		if (*end != '\0' || n <= 0) // invalid limit: ignore this change
			return (false);
		return (ch.setUserLimit(static_cast<size_t>(n)));
	}
	case 'o':
	{
		Client *target = _server.getClientByNick(change.arg);
		if (target == 0 || !ch.isMember(*target))
			throw UserNotInChannel(change.arg, ch.getName());
		bool wasOp = ch.isOperator(*target);
		if (change.sign)
			ch.promote(*target);
		else
			ch.demote(*target);
		return (wasOp != change.sign);
	}
	}
	return (false); // unreachable: parser already rejected unknown modes
}

std::string ModeCommand::currentModeString(Channel &ch)
{
	std::string modes = "+";
	std::vector<std::string> args;

	if (ch.isInviteOnly())
		modes += "i";
	if (ch.isTopicRestricted())
		modes += "t";
	if (!ch.getKey().empty())
	{
		modes += "k";
		args.push_back(ch.getKey());
	}
	if (ch.getUserLimit() != 0)
	{
		modes += "l";
		std::ostringstream os;
		os << ch.getUserLimit();
		args.push_back(os.str());
	}
	for (size_t i = 0; i < args.size(); ++i)
		modes += " " + args[i];
	return (modes);
}

void ModeCommand::execute(Client &client, Message &msg)
{
	const std::string name = msg.getParam(0);

	// User-target MODE (e.g. MODE <nick>) is out of the subject's scope: ignore.
	if (name.empty() || name[0] != '#')
		return;

	Channel *ch = _server.getChannelByName(name);
	if (ch == 0)
		throw NoSuchChannel(name);

	// No modestring => query: reply with the channel's current modes (324).
	if (msg.paramCount() < 2)
	{
		client.queueReply(RplChannelModeIs(name, currentModeString(*ch))
							   .toMessage(_server.getServerName(),
										  client.getNickName())
							   .serialize());
		return;
	}

	if (!ch->isOperator(client))
		throw ChanOPrivsNeeded(name);

	std::vector<ModeChange> changes = parseModeChange(msg);

	// Apply each change, accumulating only the ones that resulted in a real
	// state change into the echo modestring (RFC "changes which resulted").
	std::string modeStr;
	char lastSign = 0;
	std::vector<std::string> echoArgs;
	for (size_t i = 0; i < changes.size(); ++i)
	{
		if (!applyOne(*ch, changes[i]))
			continue;
		char s = changes[i].sign ? '+' : '-';
		if (s != lastSign)
		{
			modeStr += s;
			lastSign = s;
		}
		modeStr += changes[i].mode;
		if (!changes[i].arg.empty())
			echoArgs.push_back(changes[i].arg);
	}

	if (modeStr.empty())
		return; // nothing actually changed => no echo

	std::vector<std::string> p;
	p.push_back(name);
	p.push_back(modeStr);
	for (size_t i = 0; i < echoArgs.size(); ++i)
		p.push_back(echoArgs[i]);

	const std::string echo = Message(client.prefix(), "MODE", p).serialize();
	ch->broadcast(echo, client);
	client.queueReply(echo);
}

std::vector<ModeChange> ModeCommand::parseModeChange(Message &msg)
{
	std::vector<ModeChange> modeList;
	const std::string modestring = msg.getParam(1);
	size_t argIdx = 2; // param 0 = target, 1 = modestring, 2+ = mode args
	bool sign = true;

	for (size_t i = 0; i < modestring.size(); ++i)
	{
		char c = modestring[i];
		if (c == '+' || c == '-')
		{
			sign = (c == '+');
			continue;
		}
		if (KNOWN_MODES.find(c) == std::string::npos)
			throw UnknownMode(std::string(1, c));

		ModeChange change;
		change.sign = sign;
		change.mode = c;
		if (consumesArg(c, sign))
		{
			if (argIdx >= msg.paramCount())
				throw NeedMoreParams("MODE");
			change.arg = msg.getParam(argIdx++);
		}
		modeList.push_back(change);
	}
	return (modeList);
}


ModeCommand::~ModeCommand(void)
{
	
}
