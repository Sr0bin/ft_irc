/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ACommandReply.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 14:42:51 by prigaudi          #+#    #+#             */
/*   Updated: 2026/06/22 15:36:18 by prigaudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ACommandReply.hpp"
#include <sstream>

ACommandReply::ACommandReply(int code, const std::string &text) throw()
	: _code(code), _text(text) {}

ACommandReply::ACommandReply(int code, const std::string &param,
							 const std::string &text) throw()
	: _code(code), _text(text) {
	_params.push_back(param);
}

ACommandReply::~ACommandReply(void) throw() {}

// "<code> <nick> <params...> :<text>" — the dispatcher prepends ":<server> ".
std::string ACommandReply::buildReply(const std::string &nick) const {
	std::ostringstream os;

	os << _code << " " << nick;
	for (size_t i = 0; i < _params.size(); ++i)
		os << " " << _params[i];
	os << " :" << _text << "\r\n";
	return (os.str());
}

RplNoTopic::RplNoTopic(const std::string &channel) throw()
	: ACommandReply(331, channel, "No topic is set") {}

RplTopic::RplTopic(const std::string &channel, const std::string &topic) throw()
	: ACommandReply(332, channel, topic) {}
