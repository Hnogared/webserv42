/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 23:04:43 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/14 19:33:14 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILES_HPP
# define FILES_HPP

# include <string>
# include <stdexcept>
# include <fstream>
# include <map>
# include <vector>
# include <algorithm>

# include <cerrno>
# include <cstring>
# include <dirent.h>

# include "exceptions.hpp"

namespace	tool
{

namespace	files
{

std::string	readFile(const std::string &filePath);
std::string	joinPaths(const std::string &path1, const std::string &path2);
std::vector<dirent>	readDirectory(const std::string &path);

} // namespace files

} // namespace webserv

#endif // FILES_HPP