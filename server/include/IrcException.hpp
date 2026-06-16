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

class IrcException : public std::exception
{
public:
	IrcException(void);
	IrcException(j);
	IrcException(const IrcException &other);
	IrcException &operator=(const IrcException &other);
	~IrcException(void);

private:
	
};

#endif
