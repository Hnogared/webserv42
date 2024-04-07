/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 14:43:33 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 14:55:14 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

namespace	webserv
{

/* ************************************************************************** */

/* Socket fd constructor */
Client::Client(int sock_fd) : _sock_fd(sock_fd) {}

/* Copy constructor */
Client::Client(const Client &original)
{
	*this = original;
}

/* Destructor */
Client::~Client(void)
{
	close(this->_sock_fd);
}


/* ************************************************************************** */
/* Operator overloads */

/* Copy assignment operator */
Client	&Client::operator=(const Client &original)
{
	if (this != &original)
	{
		this->_sock_fd = original.getSockFd();
	}
	return (*this);
}


/* ************************************************************************** */
/* Getters */

int	Client::getSockFd(void) const
{
	return (this->_sock_fd);
}

} // namespace webserv