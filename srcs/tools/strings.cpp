/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strings.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 12:31:15 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/26 14:38:58 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "strings.hpp"

namespace	tool
{

namespace	strings
{

int	stoi(const std::string &str)
{
	int					nbr;
	std::istringstream	iss(str);

	iss >> nbr;
	return (nbr * (!iss.fail()));
}

std::string	join(const std::vector<std::string> &strings,
	const std::string &sep)
{
	size_t		i = 0;
	std::string	joined;

	if (strings.empty())
		return (joined);

	while (i < strings.size() - 1)
		joined += strings[i++] + sep;
	joined += strings[i];
	return (joined);
}

} // namespace strings

} // namespace tool