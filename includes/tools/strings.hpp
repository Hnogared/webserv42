/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strings.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 12:31:31 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/26 13:04:11 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRINGS_HPP
# define STRINGS_HPP

# include <string>
# include <sstream>

namespace	tool
{

namespace	strings
{

int			stoi(const std::string &str);

template <typename T>
std::string	toStr(T elem);

} // namespace strings

} // namespace tool

#include "strings.tpp"

#endif // STRINGS_HPP