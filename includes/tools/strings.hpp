/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strings.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 12:31:31 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/26 14:37:10 by hnogared         ###   ########.fr       */
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

int			stoi(const std::string &str);
std::string	join(const std::vector<std::string> &strings,
	const std::string &sep);

template <typename T>
std::string	toStr(T elem);

} // namespace strings

} // namespace tool

#include "strings.tpp"

#endif // STRINGS_HPP