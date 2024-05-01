/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strings.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 12:31:31 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/01 13:09:43 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRINGS_HPP
# define STRINGS_HPP

# include <string>
# include <sstream>
# include <vector>

namespace	tool
{

namespace	strings
{

int					stoi(const std::string &str);
std::string			join(const std::vector<std::string> &strings,
	const std::string &sep);
unsigned long int	bytestoul(const std::string &str);

template <typename T>
std::string	toStr(T elem);

} // namespace strings

} // namespace tool

#include "strings.tpp"

#endif // STRINGS_HPP