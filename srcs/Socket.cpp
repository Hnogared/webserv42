/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 19:07:28 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 19:26:10 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

namespace webserv
{

/* ************************************************************************** */
/* Constructors */

/* FD constructor */
Socket::Socket(int fd) : _fd(fd), _ref_count(new int(1)) {}

/* Copy constructor */
Socket::Socket(const Socket &original)
	: _fd(original.getFd()), _ref_count(original.getRefCountPtr())
{
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
	this->_ref_count = original.getRefCountPtr();
	(*(this->_ref_count))++;
	return (*this);
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