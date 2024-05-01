/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strings.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 12:31:15 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/01 13:50:02 by hnogared         ###   ########.fr       */
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

unsigned long int	bytestoul(const std::string &str)
{
	unsigned long int	nbr;
	char				factor = 0;
	std::istringstream	iss(str);

	if (str[0] == '-')
		throw std::invalid_argument("Negative byte string `" + str + "`");
	
	iss >> nbr;

	if (!iss.eof())
		iss >> factor >> std::ws;

	if (iss.fail() || !iss.eof())
		throw std::invalid_argument("Invalid byte string `" + str + "`");

	switch (factor)
	{
		case 'K': case 'k':
			nbr *= 1024;
			break;
		case 'M': case 'm':
			nbr *= 1024 * 1024;
			break;
		case 'G': case 'g':
			nbr *= 1024 * 1024 * 1024;
			break;
		case 0:
			break;
		default:
			throw std::invalid_argument("Invalid byte factor `"
				+ tool::strings::toStr(factor) + "`");
	}

	return (nbr);
}

} // namespace strings

} // namespace tool