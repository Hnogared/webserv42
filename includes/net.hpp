/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   net.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 12:44:46 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/08 12:57:55 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NET_HPP
# define NET_HPP

# include <sstream>
# include <netinet/in.h>

namespace	net
{

std::string	my_inet_ntoa(struct in_addr addr);

} // namespace net

#endif // NET_HPP