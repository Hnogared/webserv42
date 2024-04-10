/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   net.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 12:47:27 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/08 12:58:44 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net.hpp"

namespace	net
{

std::string	my_inet_ntoa(struct in_addr addr)
{
	std::ostringstream	oss_ip;
	uint32_t			s_addr = ntohl(addr.s_addr);

	oss_ip << ((s_addr >> 24) & 0xff) << "."
		<< ((s_addr >> 16) & 0xff) << "."
		<< ((s_addr >> 8) & 0xff) << "."
		<< (s_addr & 0xff);

	return (oss_ip.str());
}

} // namespace net