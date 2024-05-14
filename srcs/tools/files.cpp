/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 23:03:52 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/14 19:36:13 by hnogared         ###   ########.fr       */
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

static bool	compareEntries(const dirent &entry1, const dirent &entry2)
{
	if (entry1.d_type != entry2.d_type)
		return (entry1.d_type < entry2.d_type);
	return (std::string(entry1.d_name) < std::string(entry2.d_name));
}

std::vector<dirent>	readDirectory(const std::string &path)
{
	DIR					*dir;
	struct dirent		*entry;
	std::vector<dirent>	entries;

	dir = opendir(path.c_str());

	if (!dir)
	{
		throw webserv::RuntimeError("opendir(): '" + path + "': "
			+ std::string(strerror(errno)));
	}

	while ((entry = readdir(dir)))
		entries.push_back(*entry);

	closedir(dir);

	std::sort(entries.begin(), entries.end(), compareEntries);
	return (entries);
}

} // namespace files

} // namespace webserv