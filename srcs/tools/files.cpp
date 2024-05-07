/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 23:03:52 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/07 23:17:56 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "files.hpp"

namespace	tool
{

namespace	files
{

std::string	readFile(const std::string &filePath)
{
	std::ifstream	file(filePath.c_str());
	std::string		content;

	if (!file.is_open())
		throw std::runtime_error(filePath + ": " + std::strerror(errno));

	content.assign((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	file.close();

	return (content);
}

} // namespace files

} // namespace webserv