/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 14:19:43 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/22 10:48:26 by prigaudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_HPP
#define FT_IRC_HPP
#include <map>
#include <set>
#include <string>
#include <vector>

typedef struct serverConfig {
	int _port;
	std::string _password;
	int _listenFd;

} serverConfig;

enum clientState {
	CONNECTING,	 // socket accepted, nothing validated yet
	PASSWORD_OK, // valid PASS received
	REGISTERED	 // PASS + NICK + USER complete
};

typedef struct clientInfo {
	std::string _nickname;
	std::string _username;
	std::string _realname;
	clientState _state;
} clientInfo;

typedef struct channelParam {
	std::string _pass;
	size_t _userLimit;
	bool _inviteOnly;
	bool _topicRestricted;
} channelParam;

typedef struct Event {
	int fd;
	bool readable;
	bool writable;
	bool closed;

} Event;

/*A SUPPRIMER, MEMO PA
struct pollfd {
	int   fd;       // le file descriptor à surveiller
	short events;   // ce qu'on veut surveiller (input)
	short revents;  // ce qui s'est passé réellement (output, rempli par le
kernel)
};

struct sockaddr_in {
	sa_family_t    sin_family; // AF_INET (IPv4)
	in_port_t      sin_port;   // port (en network byte order)
	struct in_addr sin_addr;   // adresse IP
	char           sin_zero[8]; // padding (inutilisé, mettre à 0)
};

struct in_addr {
	uint32_t s_addr; // adresse IP (en network byte order)
};
*/

#endif
