/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strings.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 12:31:31 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/03 12:43:38 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRINGS_HPP
# define STRINGS_HPP

# include <string>
# include <sstream>
# include <vector>

# include <unistd.h>

namespace	tool
{

namespace	strings
{

int					stoi(const std::string &str);
unsigned int		stoui(const std::string &str);
unsigned long int	bytestoul(const std::string &str);
std::string			trim(const std::string &str, const std::string &set);
bool				isValidPath(const std::string &path);

template <typename T>
std::string	toStr(T elem);

template <typename Container>
std::string		join(const Container &strings, const std::string &sep);

} // namespace strings

} // namespace tool

#include "strings.tpp"

#endif // STRINGS_HPP