/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strings.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 12:31:15 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/26 13:01:06 by hnogared         ###   ########.fr       */
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

std::string	itos(int num)
{
	std::ostringstream oss;

	oss << num;
	return (oss.str());
}

} // namespace strings

} // namespace tool