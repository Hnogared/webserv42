/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 23:04:43 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/13 17:44:46 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILES_HPP
# define FILES_HPP

# include <string>
# include <stdexcept>
# include <fstream>

# include <cerrno>
# include <cstring>

# include "exceptions.hpp"

namespace	tool
{

namespace	files
{

std::string	readFile(const std::string &filePath);
std::string	joinPaths(const std::string &path1, const std::string &path2);

} // namespace files

} // namespace webserv

#endif // FILES_HPP