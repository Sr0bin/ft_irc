/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 14:19:43 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/16 16:49:42 by rorollin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef FT_IRC_HPP
# define FT_IRC_HPP
# include <string>
# include <set>
# include <map>
# include <vector>

using serverConfig = struct serverConfig 
{
	int	_port;
	std::string _password;
	int	_listenFd;

};

enum clientState
{
	CONNECTING

};

using clientInfo = struct clientInfo
{
	std::string _nickname;
	std::string _username;
	std::string _realname;
	clientState _state;
};

typedef struct channelParam
{
	std::string _pass;
	size_t _userLimit;
	bool _inviteOnly;
	bool _topicRestricted;
} channelParam;

using Event = struct Event
{
	int fd;
	bool readable;
	bool writable;
	bool closed;

};

#endif
