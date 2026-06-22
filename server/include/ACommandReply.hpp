/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ACommandError.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rorollin <rorollin@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 by rorollin                  #+#    #+#             */
/*                                                                            */
/* ************************************************************************** */

#ifndef ACOMMANDREPLY_HPP
#define ACOMMANDREPLY_HPP
#include <string>
#include <vector>

class ACommandReply {
  public:
	ACommandReply(int code, const std::string &text) throw();
	ACommandReply(int code, const std::string &param,
				  const std::string &text) throw();
	virtual ~ACommandReply(void) throw();

	std::string buildReply(const std::string &nick) const;

  protected:
	int _code;
	std::vector<std::string> _params;
	std::string _text;
};

// 331
class RplNoTopic : public ACommandReply {
  public:
	RplNoTopic(const std::string &channel) throw();
};

// 332
class RplTopic : public ACommandReply {
  public:
	RplTopic(const std::string &channel, const std::string &topic) throw();
};

// 353
//  class RplNamReply : public ACommandReply {
//  	...
//  };

// 366
//  class RplEndOfNames : public ACommandReply {
//  	...
//  };

#endif
