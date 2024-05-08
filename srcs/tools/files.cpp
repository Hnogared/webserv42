/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 23:03:52 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/08 16:42:23 by hnogared         ###   ########.fr       */
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

std::string	joinPaths(const std::string &path1, const std::string &path2)
{
	std::string	joinedPath;

	if (path1.empty())
		return (path2);
	if (path2.empty())
		return (path1);

	joinedPath = path1;
	if (path1[path1.size() - 1] != '/')
		joinedPath += '/';

	if (path2[0] == '/')
		joinedPath += path2.substr(1);
	else
		joinedPath += path2;

	return (joinedPath);
}

} // namespace files

} // namespace webserv