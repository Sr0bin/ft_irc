/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcException.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:45:09 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 16:45:39 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef IRCEXCEPTION_HPP
# define IRCEXCEPTION_HPP
# include <exception>
# include <string>

class IrcException : public std::exception
{
public:
	IrcException(void) throw();
	IrcException(const std::string &msg) throw();
	IrcException(const IrcException &other) throw();
	IrcException &operator=(const IrcException &other) throw();
	virtual ~IrcException(void) throw();

	virtual const char *what(void) const throw();

private:
	std::string _msg;
};

#endif
