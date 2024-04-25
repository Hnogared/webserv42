/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpTools.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 16:42:19 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/18 16:45:50 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpTools.hpp"

namespace	http
{

std::string	trimLWS(const std::string &str, const std::string &set)
{
	std::string::size_type	start = str.find_first_not_of(set);
	std::string::size_type	end = str.find_last_not_of(set);

	if (start == std::string::npos)
		return ("");
	return (str.substr(start, end - start + 1));
}

} // namespace http