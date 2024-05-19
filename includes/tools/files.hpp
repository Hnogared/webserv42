/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 23:04:43 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/19 22:26:47 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILES_HPP
#define FILES_HPP

#include <dirent.h>
#include <sys/stat.h>

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "exceptions.hpp"

namespace tool
{

namespace files
{

bool fileExists(const std::string &filePath);
std::string readFile(const std::string &filePath);
std::string joinPaths(const std::string &path1, const std::string &path2);
std::vector<dirent> readDirectory(const std::string &path);

}  // namespace files

}  // namespace tool

#endif  // FILES_HPP