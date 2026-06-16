/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 14:19:43 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 20:36:23 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef FT_IRC_HPP
# define FT_IRC_HPP
# include <string>
# include <set>
# include <map>
# include <vector>

typedef struct serverConfig 
{
	int	_port;
	std::string _password;
	int	_listenFd;

} serverConfig;

enum clientState
{
	CONNECTING

};

typedef struct clientInfo
{
	std::string _nickname;
	std::string _username;
	std::string _realname;
	clientState _state;
} clientInfo;

typedef struct channelParam
{
	std::string _pass;
	size_t _userLimit;
	bool _inviteOnly;
	bool _topicRestricted;
} channelParam;

typedef struct Event
{
	int fd;
	bool readable;
	bool writable;
	bool closed;

} Event;

#endif
