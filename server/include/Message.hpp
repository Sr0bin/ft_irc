/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:23:20 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 20:53:25 by rorollin         ###   ########.fr       */
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
		const std::vector<std::string> &params);
	Message(const Message &other);
	Message &operator=(const Message &other);
	~Message(void);

	std::string getCommand(void) const;
	std::string getParam(size_t i) const;
	size_t paramCount(void) const;

private:
	std::string _prefix;
	std::string _command;
	std::vector<std::string> _params;
};

#endif
