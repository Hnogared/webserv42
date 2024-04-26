/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strings.tpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 13:02:29 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/26 13:18:31 by hnogared         ###   ########.fr       */
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

} // namespace strings

} // namespace tool

#endif // STRINGS_TPP