/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 19:02:56 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/19 22:31:12 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include <cerrno>
#include <sstream>
#include <string>

#include "Harl.hpp"
#include "exceptions.hpp"
#include "net.hpp"

namespace webserv
{

class Socket
{
public:
    /* Constructors */
    explicit Socket(int fd = -1);
    Socket(int fd, const struct sockaddr_in &peer_addr);
    Socket(const Socket &original);

    /* Destructor */
    ~Socket(void);

    /* Operator overloads */
    Socket &operator=(const Socket &original);
    bool operator==(const Socket &other) const;

    /* Getters */
    int getFd(void) const;
    int *getRefCountPtr(void) const;
    struct sockaddr_in getLocalAddr(void) const;
    struct sockaddr_in getPeerAddr(void) const;
    bool isPeerAddrSet(void) const;
    std::string getInfosStr(void) const;

    /* Setters */
    void setFd(int fd);
    void setLocalAddr(struct sockaddr_in local_addr);
    void setPeerAddr(struct sockaddr_in peer_addr);

    /* Public methods */
    void close(void);

private:
    /* Private attributes */
    int _fd;
    int *_ref_count;
    struct sockaddr_in _local_addr;
    struct sockaddr_in _peer_addr;
    bool _peer_addr_set;
};

}  // namespace webserv

#endif  // SOCKET_HPP