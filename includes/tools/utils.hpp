/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 19:18:32 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/09 19:33:38 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>

namespace	utils
{

std::string	trim(const std::string &str, const std::string &set = " \t\n\r");

}

#endif // UTILS_HPP