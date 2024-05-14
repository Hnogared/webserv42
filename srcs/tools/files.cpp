/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 23:03:52 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/14 11:20:56 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "files.hpp"

namespace	tool
{

namespace	files
{

std::string	readFile(const std::string &filePath)
{
	try
	{
		std::ifstream	fileStream(filePath.c_str());
		std::string		content;

		if (!fileStream.is_open())
			throw std::exception();

		content.assign((std::istreambuf_iterator<char>(fileStream)),
			std::istreambuf_iterator<char>());
		fileStream.close();

		return (content);
	}
	catch(const std::exception& e)
	{
		throw webserv::FileException("readFile(): '" + filePath + "': "
			+ std::strerror(errno), errno);
	}
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