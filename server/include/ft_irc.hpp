/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 14:19:43 by rorollin          #+#    #+#             */
/*   Updated: 2026/06/18 15:05:43 by prigaudi         ###   ########.fr       */
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
  CONNECTING

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
*/

#endif
