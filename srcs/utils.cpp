/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 19:19:34 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/09 19:35:26 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

namespace	utils
{

std::string	trim(const std::string &str, const std::string &set)
{
	std::string::size_type	start = str.find_first_not_of(set);
	std::string::size_type	end = str.find_last_not_of(set);

	if (start == std::string::npos)
		return ("");
	return (str.substr(start, end - start + 1));
}

} // namespace utils