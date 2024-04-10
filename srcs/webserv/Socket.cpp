/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 19:07:28 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/09 19:56:14 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

namespace webserv
{

/* ************************************************************************** */
/* Constructors */

/* FD constructor */
Socket::Socket(int fd) : _fd(fd), _ref_count(new int(1)), _peer_addr_set(false)
{
	socklen_t			addr_len = sizeof(struct sockaddr_in);

	if (fd >= 0 && getsockname(this->_fd, (struct sockaddr *)&this->_local_addr,
		&addr_len) == -1)
	{
		throw SocketError("Error getting the client socket name: " +
			std::string(strerror(errno)), errno);
	}
}

/* FD + peer address constructor */
Socket::Socket(int fd, const struct sockaddr_in &peer_addr)
	: _fd(fd),
	_ref_count(new int(1)),
	_peer_addr(peer_addr),
	_peer_addr_set(true)
{
	socklen_t			addr_len = sizeof(struct sockaddr_in);

	if (fd >= 0 && getsockname(this->_fd, (struct sockaddr *)&this->_local_addr,
		&addr_len) == -1)
	{
		throw SocketError("Error getting the client socket name: " +
			std::string(strerror(errno)), errno);
	}
}

/* Copy constructor */
Socket::Socket(const Socket &original)
	: _fd(original.getFd()),
	_ref_count(original.getRefCountPtr()),
	_local_addr(original.getLocalAddr()),
	_peer_addr_set(false)
{
	if (original.isPeerAddrSet())
	{
		this->_peer_addr = original.getPeerAddr();
		this->_peer_addr_set = true;
	}
	(*(this->_ref_count))++;
}


/* Destructor */
Socket::~Socket(void)
{
	if (--(*(this->_ref_count)) == 0)
	{
		delete this->_ref_count;
		if (this->_fd != -1)
			::close(this->_fd);
	}
}

/* ************************************************************************** */
/* Operator overloads */

/* Copy assignment operator */
Socket	&Socket::operator=(const Socket &original)
{
	if (this == &original)
		return (*this);

	if (--(*(this->_ref_count)) == 0)
	{
		delete this->_ref_count;
		if (this->_fd != -1)
			::close(this->_fd);
	}

	this->_fd = original.getFd();
	this->_local_addr = original.getLocalAddr();
	this->_peer_addr_set = original.isPeerAddrSet();
	if (this->_peer_addr_set)
		this->_peer_addr = original.getPeerAddr();
	this->_ref_count = original.getRefCountPtr();
	(*(this->_ref_count))++;
	return (*this);
}

/* Equality comparison operator */
bool	Socket::operator==(const Socket &rhs) const
{
	return (this->_fd == rhs.getFd());
}


/* ************************************************************************** */
/* Getters */

int	Socket::getFd(void) const
{
	return (this->_fd);
}

int	*Socket::getRefCountPtr(void) const
{
	return (this->_ref_count);
}

struct sockaddr_in	Socket::getLocalAddr(void) const
{
	return (this->_local_addr);
}

struct sockaddr_in	Socket::getPeerAddr(void) const
{
	return (this->_peer_addr);
}

bool	Socket::isPeerAddrSet(void) const
{
	return (this->_peer_addr_set);
}

std::string	Socket::getInfosStr(void) const
{
	std::ostringstream	oss_msg;

	if (this->_fd == -1)
		return ("Socket closed");

	oss_msg << "Local addr: "
		<< net::my_inet_ntoa(this->_local_addr.sin_addr)
		<< ":" << ntohs(this->_local_addr.sin_port);

	if (this->_peer_addr_set)
	{
		oss_msg << " | Peer addr: "
			<< net::my_inet_ntoa(this->_peer_addr.sin_addr)
			<< ":" << ntohs(this->_peer_addr.sin_port);
	}
	else
		oss_msg << " | Peer addr: not set";

	return (oss_msg.str());
}


/* ************************************************************************** */
/* Setters */

void	Socket::setFd(int fd)
{
	this->_fd = fd;
}

void	Socket::setLocalAddr(struct sockaddr_in local_addr)
{
	this->_local_addr = local_addr;
}

void	Socket::setPeerAddr(struct sockaddr_in peer_addr)
{
	this->_peer_addr = peer_addr;
}


/* ************************************************************************** */
/* Public methods */

/* Method to close a socket */
void	Socket::close(void)
{
	if (this->_fd != -1)
		::close(this->_fd);
	this->_fd = -1;
}

} // namespace webserv