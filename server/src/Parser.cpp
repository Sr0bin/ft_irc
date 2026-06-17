/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*                                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpirotti <vpirotti@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   GitHub : @Nave20                                  #+#    #+#             */
/*   28 is the new 42                                 ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "Parser.hpp"
#include <cstring>
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>

std::vector<std::string> Parser::parseParams(const std::string& params, const std::string& last_param)
{
	std::vector<std::string> result;
	std::string token;
	std::stringstream ss(params);

	while (ss >> token)
		result.push_back(token);

	if (!last_param.empty())
		result.push_back(last_param);

	return result;
}

void Parser::parseRawMessage(const std::string& raw)
{
	std::cout<<raw<<std::endl;
	if (raw.empty())
		return;

	std::string newRaw = raw;
	std::string prefix;
	std::string cmd;
	std::string last_param;
	std::vector<std::string> params;

	newRaw.erase(newRaw.find("\r\n"));
	if (newRaw[0] == ':')
	{
		int i = 0;
		while (newRaw[i] != 0 && newRaw[i] != ' ')
			i++;

		prefix = newRaw.substr(1, i);
		newRaw.erase(newRaw.begin(), newRaw.begin() + i + 1);
	}
	else
		prefix = "";
	int i = 0;
	while (newRaw[i] != 0 && newRaw[i] != ' ')
		i++;
	cmd = newRaw.substr(0, i);
	newRaw.erase(newRaw.begin(), newRaw.begin() + i + 1);
	last_param = &newRaw[newRaw.find(':') + 1];
	size_t len = last_param.length();
	newRaw.erase(newRaw.find(':'), newRaw.find(':') + len);
	params = parseParams(newRaw, last_param);

	// testing for ":Bob!bob@localhost PRIVMSG Alice Param1 Param2 Param3 :Salut\r\n tetstststtstst"
	// std::cout<< "prefix | " << prefix<< std::endl;
	// std::cout<< "cmd | " <<cmd<<std::endl;
	// std::cout<< "last param | " << last_param<<std::endl;
	// std::cout<< "param | " << params[0] <<std::endl;
	// std::cout<< "param | " << params[1] <<std::endl;
	// std::cout<< "param | " << params[2] <<std::endl;
	// std::cout<< "param | " << params[3] <<std::endl;
	// std::cout<< "param | " << params[4] <<std::endl;
}
