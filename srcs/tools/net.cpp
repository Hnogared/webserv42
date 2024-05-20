/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   net.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 12:47:27 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/20 18:23:40 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net.hpp"

namespace tool
{

namespace net
{

std::string inet_ntoa(struct in_addr addr)
{
    std::ostringstream oss_ip;
    uint32_t s_addr = ntohl(addr.s_addr);

    oss_ip << ((s_addr >> 24) & 0xff) << "." << ((s_addr >> 16) & 0xff) << "."
           << ((s_addr >> 8) & 0xff) << "." << (s_addr & 0xff);

    return (oss_ip.str());
}

int inet_aton(const std::string &address, struct in_addr &addr)
{
    int i = 0;
    uint32_t s_addr = 0;
    std::string segment;
    std::istringstream iss(address);

    while (std::getline(iss, segment, '.'))
    {
        if (i > 3) return (0);
        s_addr = (s_addr << 8) | tool::strings::stoib(segment);
        i++;
    }
    addr.s_addr = htonl(s_addr);
    return (1);
}

}  // namespace net

}  // namespace tool