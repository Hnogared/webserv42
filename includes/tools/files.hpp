/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 23:04:43 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/07 23:17:09 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILES_HPP
# define FILES_HPP

# include <string>
# include <stdexcept>
# include <fstream>

# include <cerrno>
# include <cstring>

namespace	tool
{

namespace	files
{

std::string	readFile(const std::string &filePath);

} // namespace files

} // namespace webserv

#endif // FILES_HPP