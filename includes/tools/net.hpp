/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   net.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 12:44:46 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/19 22:26:51 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NET_HPP
#define NET_HPP

#include <netinet/in.h>

#include <sstream>

#include "strings.hpp"

namespace tool
{

namespace net
{

std::string inet_ntoa(struct in_addr addr);
int inet_aton(const std::string &address, struct in_addr &addr);

}  // namespace net

}  // namespace tool

#endif  // NET_HPP