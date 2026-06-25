/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:23:20 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/25 20:42:40 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MESSAGE_HPP
# define MESSAGE_HPP
# include "ft_irc.hpp"

class Message
{
public:
	Message(void);
	Message(const std::string &prefix, const std::string &command,
		const std::vector<std::string> &params, bool trailing = false);
	Message(const Message &other);
	Message &operator=(const Message &other);
	~Message(void);

	std::string getPrefix(void) const;
	std::string getCommand(void) const;
	std::string getParam(size_t i) const;
	size_t paramCount(void) const;

	// Outbound: render this message to the wire ("[:prefix ]command params :trailing\r\n").
	// Symmetric counterpart of Parser::parseRawMessage.
	std::string serialize(void) const;

	// Build a numeric reply: prefix = server, command = 3-digit code, first
	// param = nick, then middle params, then (with text) the trailing text.
	static Message numeric(const std::string &server, int code,
		const std::string &nick, const std::vector<std::string> &middle,
		const std::string &text);
	static Message numeric(const std::string &server, int code,
		const std::string &nick, const std::vector<std::string> &middle);

private:
	std::string _prefix;
	std::string _command;
	std::vector<std::string> _params;
	bool _trailing;
};

std::ostream& operator<<(std::ostream &os, const Message &msg);

#endif
