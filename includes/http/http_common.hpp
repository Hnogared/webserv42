/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_common.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 21:30:42 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/18 12:52:34 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_COMMON_HPP
# define HTTP_COMMON_HPP

# include <string>
# include <stdexcept>

namespace	http
{

std::string	urlEncode(const std::string &str);
std::string	urlDecode(const std::string &str);

} // namespace http

#endif // HTTP_COMMON_HPP