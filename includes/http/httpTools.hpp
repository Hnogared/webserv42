/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpTools.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 16:42:22 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/18 16:45:47 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPTOOLS_HPP
# define HTTPTOOLS_HPP

# include <string>

namespace	http
{

std::string	trimLWS(const std::string &str, const std::string &set = " \t\n\r");

} // mamespace http

#endif // HTTPTOOLS_HPP