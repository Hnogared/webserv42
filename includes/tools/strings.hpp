/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strings.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 12:31:31 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/01 15:16:33 by hnogared         ###   ########.fr       */
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
unsigned long int	bytestoul(const std::string &str);

template <typename T>
std::string	toStr(T elem);

template <typename Container>
std::string		join(const Container &strings, const std::string &sep);

} // namespace strings

} // namespace tool

#include "strings.tpp"

#endif // STRINGS_HPP