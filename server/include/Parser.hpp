/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 19:46:39 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/17 19:48:30 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef PARSER_HPP
# define PARSER_HPP
# include "ft_irc.hpp"
#include "Message.hpp"

class Parser
{
public:
	static Message parseRawMessage(const std::string& raw);
};

#endif
