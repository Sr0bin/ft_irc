/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FatalException.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 by rorollin                  #+#    #+#             */
/*                                                                            */
/* ************************************************************************** */

#ifndef FATALEXCEPTION_HPP
# define FATALEXCEPTION_HPP
# include "IrcException.hpp"

// Unrecoverable failure (socket/bind/listen/poll). Caught only in main() to
// shut the whole server down cleanly. Never thrown from command execution.
class FatalException : public IrcException
{
public:
	FatalException(const std::string &msg) throw();
	virtual ~FatalException(void) throw();
};

#endif
