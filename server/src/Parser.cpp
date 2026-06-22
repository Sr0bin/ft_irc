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

// #include "ft_irc.hpp"
#include "../include/Parser.hpp"
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>

std::vector<std::string> Parser::parseParams(const std::string& params, const std::string& last_param, bool lptrue)
{
	std::vector<std::string> result;
	std::string token;
	std::stringstream ss(params);

	if (lptrue)
	{
		while (ss >> token)
			result.push_back(token);
		result.push_back(last_param);
	}
	else
	{
		while (ss >> token)
			result.push_back(token);
		if (!last_param.empty())
			result.push_back(last_param);
	}
	return result;
}

static bool findLastParam(std::string line)
{
	int j = 0;
	while (line[j] != 0 && line[j] != ':')
		j++;
	if (line[j] == 0)
		return false;
	return true;
}

Message Parser::parseRawMessage(const std::string& raw)
{
	std::cout<<raw<<std::endl;
	if (raw.empty())
		return Message();

	std::string newRaw = raw;
	std::string prefix;
	std::string cmd;
	std::string last_param;
	std::vector<std::string> params;

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
	if (newRaw[i] == 0)
	{
		cmd = newRaw.substr(0, i);
		return (Message(prefix, cmd, params));
	}
	else if (findLastParam(newRaw) == true)
	{
		cmd = newRaw.substr(0, i);
		newRaw.erase(newRaw.begin(), newRaw.begin() + i + 1);
		last_param = &newRaw[newRaw.find(':') + 1];
		size_t len = last_param.length();
		newRaw.erase(newRaw.find(':'), newRaw.find(':') + len + 1);
		// std::cout<< "+" <<newRaw<< "+" <<std::endl;
		params = parseParams(newRaw, last_param, true);
	}
	else
	{
		cmd = newRaw.substr(0, i);
		newRaw.erase(newRaw.begin(), newRaw.begin() + i + 1);
		params = parseParams(newRaw, last_param, false);
	}
	return (Message(prefix, cmd, params));
}
