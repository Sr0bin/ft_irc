/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prigaudi <prigaudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 13:45:03 by prigaudi          #+#    #+#             */
/*   Updated: 2026/06/19 13:45:05 by prigaudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP
#include "ft_irc.hpp"

class Utils {
  public:
	static std::string toLower(const std::string &s);
	static bool ircEquals(const std::string &a, const std::string &b);
};

#endif