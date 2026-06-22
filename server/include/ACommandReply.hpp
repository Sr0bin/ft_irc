/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ACommandReply.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 16:17:35 by prigaudi          #+#    #+#             */
/*   Updated: 2026/06/22 16:17:39 by prigaudi         ###   ########.fr       */
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
	ACommandReply(int code, const std::string &param1,
				  const std::string &param2, const std::string &text) throw();

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
class RplNamReply : public ACommandReply {
  public:
	RplNamReply(const std::string &channel, const std::string &names) throw();
};

// 366
class RplEndOfNames : public ACommandReply {
  public:
	RplEndOfNames(const std::string &channel) throw();
};

#endif
