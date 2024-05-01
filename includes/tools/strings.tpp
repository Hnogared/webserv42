/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strings.tpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 13:02:29 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/01 15:16:18 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRINGS_TPP
# define STRINGS_TPP

namespace	tool
{

namespace	strings
{

template <typename T>
std::string	toStr(T elem)
{
	std::ostringstream oss;

	oss << elem;
	return (oss.str());
}

template <typename Container>
std::string		join(const Container &strings, const std::string &sep)
{
	std::string							joined;
	typename Container::const_iterator	it = strings.begin();

	if (it != strings.end())
		joined += *it++;
	while (it != strings.end())
		joined += sep + *it++;
	return (joined);
}

} // namespace strings

} // namespace tool

#endif // STRINGS_TPP