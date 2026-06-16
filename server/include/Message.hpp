/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:23:20 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 16:24:48 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MESSAGE_HPP
# define MESSAGE_HPP
# include "ft_irc.hpp"

class Message
{
public:
	Message(void);
	Message(params);
	Message(const Message &other);
	Message &operator=(const Message &other);
	~Message(void);

private:
	std::string _prefix;
	std::string _command;
	std::vector<std::string> _params;
};

#endif
