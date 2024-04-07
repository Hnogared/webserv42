/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 12:36:55 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 13:23:11 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* ************************************************************************** */
/* Constructors */

/* Default constructor */
Server::Server(int port, int backlog) : _port(port), _backlog(backlog)
{
	int	optval = 1;

	_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sock_fd < 0 || setsockopt(
		_sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
	{
		throw SocketCreationError(
			"Failed to create socket: " + std::string(strerror(errno)));
	}

	_server_address.sin_family = AF_INET;
	_server_address.sin_addr.s_addr = htonl(INADDR_ANY); // All interfaces
	_server_address.sin_port = htons(port);
	if (bind(_sock_fd, (struct sockaddr *)&_server_address,
		sizeof(_server_address)) == -1)
	{
		throw SocketBindError(
			"Failed to bind socket: " + std::string(strerror(errno)));
	}

	if (listen(_sock_fd, backlog) == -1)
	{
		throw SocketListenError(
			"Failed to listen on socket: " + std::string(strerror(errno)));
	}
}

/* Copy constructor */
Server::Server(const Server &original)
{
	*this = original;
}


/* Destructor */
Server::~Server(void)
{
	close(_sock_fd);
}


/* ************************************************************************** */
/* Operator overloads */

/* Copy assignment operator */
Server	&Server::operator=(const Server &original)
{
	if (this == &original)
		return (*this);
	this->_sock_fd = original.getSockFd();
	this->_port = original.getPort();
	this->_backlog = original.getBacklog();
	this->_server_address = original.getServerAddress();
	return (*this);
}


/* ************************************************************************** */
/* Getters */

int	Server::getSockFd(void) const
{
	return (this->_sock_fd);
}

int	Server::getPort(void) const
{
	return (this->_port);
}

int	Server::getBacklog(void) const
{
	return (this->_backlog);
}

struct sockaddr_in	Server::getServerAddress(void) const
{
	return (this->_server_address);
}


/* ************************************************************************** */
/* Public methods */

int	Server::acceptConnection(void)
{
	int					client_fd;
	struct sockaddr_in	client_address;
	socklen_t			client_address_len = sizeof(client_address);

	client_fd = accept(_sock_fd, (struct sockaddr *)&client_address,
			&client_address_len);
	if (client_fd == -1)
	{
		throw SocketError(
			"Failed to accept connection: " + std::string(strerror(errno)));
	}
	return (client_fd);
}


/* ************************************************************************** */
/* Exceptions */

/* ***************************************** */
/* Socket error                              */
/* ***************************************** */

/* Default constructor */
Server::SocketError::SocketError(void) : _msg("Socket error") {}

/* Message constructor */
Server::SocketError::SocketError(const std::string &message) : _msg(message) {}

/* Copy constructor */
Server::SocketError::SocketError(const SocketError &original) : std::exception()
{
	*this = original;
}


/* Destructor */
Server::SocketError::~SocketError(void) throw() {}


/* ***************************************** */
/* Operator overloads */

/* Copy assignment operator */
Server::SocketError	&Server::SocketError::operator=(const SocketError &original)
{
	if (this == &original)
		return (*this);
	this->_msg = std::string(original.what());
	return (*this);
}

/* ***************************************** */
/* std::exception method overrides */

/* Method to return the exception message */
const char	*Server::SocketError::what(void) const throw()
{
	return (_msg.c_str());
}


/* ***************************************** */
/* Socket creation error                     */
/* ***************************************** */

/* Default constructor */
Server::SocketCreationError::SocketCreationError(void)
	: SocketError("Error creating socket") {}

/* Message constructor */
Server::SocketCreationError::SocketCreationError(const std::string &message)
	: SocketError(message) {}

/* Copy constructor */
Server::SocketCreationError::SocketCreationError(const SocketCreationError
		&original) : SocketError(original)
{
	*this = original;
}


/* Destructor */
Server::SocketCreationError::~SocketCreationError(void) throw() {}


/* ***************************************** */
/* Operator overloads */

/* Copy assignment operator */
Server::SocketCreationError	&Server::SocketCreationError::operator=(const
		SocketCreationError &original)
{
	if (this == &original)
		return (*this);
	SocketError::operator=(original);
	return (*this);
}


/* ***************************************** */
/* Socket bind error                         */
/* ***************************************** */

/* Default constructor */
Server::SocketBindError::SocketBindError(void)
	: SocketError("Error binding socket") {}

/* Message constructor */
Server::SocketBindError::SocketBindError(const std::string &message)
	: SocketError(message) {}

/* Copy constructor */
Server::SocketBindError::SocketBindError(const SocketBindError
		&original) : SocketError(original)
{
	*this = original;
}


/* Destructor */
Server::SocketBindError::~SocketBindError(void) throw() {}


/* ***************************************** */
/* Operator overloads */

/* Copy assignment operator */
Server::SocketBindError	&Server::SocketBindError::operator=(const
		SocketBindError &original)
{
	if (this == &original)
		return (*this);
	SocketError::operator=(original);
	return (*this);
}


/* ***************************************** */
/* Socket listen error                       */
/* ***************************************** */

/* Default constructor */
Server::SocketListenError::SocketListenError(void)
	: SocketError("Error listening on socket") {}

/* Message constructor */
Server::SocketListenError::SocketListenError(const std::string &message)
	: SocketError(message) {}

/* Copy constructor */
Server::SocketListenError::SocketListenError(const SocketListenError
		&original) : SocketError(original)
{
	*this = original;
}


/* Destructor */
Server::SocketListenError::~SocketListenError(void) throw() {}

/* ***************************************** */
/* Operator overloads */

/* Copy assignment operator */
Server::SocketListenError	&Server::SocketListenError::operator=(const
		SocketListenError &original)
{
	if (this == &original)
		return (*this);
	SocketError::operator=(original);
	return (*this);
}