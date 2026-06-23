/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*                                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpirotti <vpirotti@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   GitHub : @Nave20                                  #+#    #+#             */
/*   28 is the new 42                                 ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

// Parses one IRC line (already stripped of CRLF by Client::extractLine) into a
// Message, following RFC 1459/2812:
//   [ ":" prefix SPACE ] command *( SPACE middle ) [ SPACE ":" trailing ]
// Index-based with bounds checks throughout: no operator[] past the end, no
// assumption that a separating space exists. Malformed input yields an empty
// Message rather than undefined behaviour.
Message Parser::parseRawMessage(const std::string& raw)
{
	std::string prefix;
	std::string command;
	std::vector<std::string> params;
	bool trailing = false;

	const std::string::size_type len = raw.size();
	std::string::size_type pos = 0;

	// Optional ":prefix" terminated by a space.
	if (pos < len && raw[pos] == ':')
	{
		std::string::size_type sp = raw.find(' ', pos);
		if (sp == std::string::npos)
			return Message(); // prefix with no command: malformed
		prefix = raw.substr(pos + 1, sp - pos - 1);
		pos = sp;
	}

	while (pos < len && raw[pos] == ' ')
		++pos;

	// Command (one word). An empty / space-only line has none.
	if (pos >= len)
		return Message();
	{
		std::string::size_type sp = raw.find(' ', pos);
		if (sp == std::string::npos)
		{
			command = raw.substr(pos);
			return Message(prefix, command, params);
		}
		command = raw.substr(pos, sp - pos);
		pos = sp;
	}

	// Params: middles split on spaces, until a param starting with ':' which is
	// the trailing param (the rest of the line, spaces included).
	while (pos < len)
	{
		while (pos < len && raw[pos] == ' ')
			++pos;
		if (pos >= len)
			break;
		if (raw[pos] == ':')
		{
			params.push_back(raw.substr(pos + 1));
			trailing = true;
			break;
		}
		std::string::size_type sp = raw.find(' ', pos);
		if (sp == std::string::npos)
		{
			params.push_back(raw.substr(pos));
			break;
		}
		params.push_back(raw.substr(pos, sp - pos));
		pos = sp;
	}

	return Message(prefix, command, params, trailing);
}
