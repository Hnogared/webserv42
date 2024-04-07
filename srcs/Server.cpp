/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 12:36:55 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 15:19:00 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

namespace	webserv
{

/* Private class constants initialization */
const std::string	Server::CLASS_NAME = "Server";

/* ************************************************************************** */
/* Constructors */

/* Default constructor */
Server::Server(int port, int backlog) : _port(port), _backlog(backlog)
{
	{
		std::ifstream	lock_file(LOCK_FILE);

		if (lock_file.is_open())
		{
			lock_file.close();
			throw std::runtime_error(CLASS_NAME
				+ ": Another instance is already running");
		}
	}

	{
		std::ofstream	lock_file_out(LOCK_FILE);
		
		if (!lock_file_out.is_open())
		{
			throw std::runtime_error(CLASS_NAME
				+ ": Failed to create lock file");
		}
		lock_file_out.close();
	}

	int	optval = 1;

	this->_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_sock_fd < 0 || setsockopt(
		this->_sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
	{
		throw SocketCreationError(CLASS_NAME + ": Failed to create socket: "
			+ std::string(strerror(errno)));
	}

	this->_server_address.sin_family = AF_INET;
	this->_server_address.sin_addr.s_addr = htonl(INADDR_ANY); // All interfaces
	this->_server_address.sin_port = htons(port);

	if (bind(this->_sock_fd, (struct sockaddr *)&_server_address,
		sizeof(this->_server_address)) == -1)
	{
		throw SocketError(CLASS_NAME + ": Failed to bind socket: "
			+ std::string(strerror(errno)));
	}

	if (listen(this->_sock_fd, backlog) == -1)
	{
		throw SocketError(CLASS_NAME + ": Failed to listen on socket: "
			+ std::string(strerror(errno)));
	}
}

/* Copy constructor */
Server::Server(const Server &original)
{
	*this = original;
	remove(LOCK_FILE);
}


/* Destructor */
Server::~Server(void)
{
	close(this->_sock_fd);
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

void	Server::acceptConnection(void)
{
	int					client_fd;
	struct sockaddr_in	client_address;
	socklen_t			client_address_len = sizeof(client_address);

	client_fd = accept(this->_sock_fd, (struct sockaddr *)&client_address,
			&client_address_len);
	if (client_fd == -1)
	{
		throw SocketConnectionError(CLASS_NAME
			+ ": Failed to accept connection: " + std::string(strerror(errno)));
	}
	
	_clients.push_back(Client(client_fd));
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
/* Socket connection error                   */
/* ***************************************** */

/* Default constructor */
Server::SocketConnectionError::SocketConnectionError(void)
	: SocketError("Error binding socket") {}

/* Message constructor */
Server::SocketConnectionError::SocketConnectionError(const std::string &message)
	: SocketError(message) {}

/* Copy constructor */
Server::SocketConnectionError::SocketConnectionError(const SocketConnectionError
		&original) : SocketError(original)
{
	*this = original;
}


/* Destructor */
Server::SocketConnectionError::~SocketConnectionError(void) throw() {}


/* ***************************************** */
/* Operator overloads */

/* Copy assignment operator */
Server::SocketConnectionError	&Server::SocketConnectionError::operator=(
		const SocketConnectionError &original)
{
	if (this == &original)
		return (*this);
	SocketError::operator=(original);
	return (*this);
}

} // namespace webserv