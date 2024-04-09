/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 14:43:33 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/09 19:57:18 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

namespace	webserv
{

/* ************************************************************************** */

/* Socket fd constructor */
Client::Client(const Socket &socket) : _socket(socket) {}

/* Copy constructor */
Client::Client(const Client &original)
{
	if (this != &original)
		this->_socket = original.getSocket();
}


/* Destructor */
Client::~Client(void) {}


/* ************************************************************************** */
/* Operator overloads */

/* Copy assignment operator */
Client	&Client::operator=(const Client &original)
{
	if (this == &original)
		return (*this);
	this->_socket = original.getSocket();
	return (*this);
}

/* Equality comparison operator */
bool	Client::operator==(const Client &other) const
{
	return (this->_socket == other.getSocket());
}


/* ************************************************************************** */
/* Getters */

int	Client::getSocketFd(void) const
{
	return (this->_socket.getFd());
}

webserv::Socket	Client::getSocket(void) const
{
	return (this->_socket);
}

std::string	Client::getAddrStr(e_addr_choice choice) const
{
	std::ostringstream	oss;
	struct sockaddr_in	addr;

	if (choice == LOCAL)
		addr = this->_socket.getLocalAddr();
	else if (this->_socket.isPeerAddrSet())
		addr = this->_socket.getPeerAddr();
	else
		return ("Unknown");

	oss << net::my_inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port);
	return (oss.str());
}


/* ************************************************************************** */
/* Public methods */

void	Client::sendResponse(const http::HttpResponse &response) const
{
	ssize_t		bytes_sent;
	std::string	response_str(response.toString());

	bytes_sent = send(this->_socket.getFd(), response_str.c_str(),
			response_str.size(), 0);
	if (bytes_sent < 0)
	{
		throw std::runtime_error("Failed to send data to client: "
			+ std::string(strerror(errno)));
	}
}

http::HttpRequest	Client::fetchRequest(void) const
{
	char	buffer[1024];
	ssize_t	bytes_read;

	bytes_read = recv(this->_socket.getFd(), buffer, sizeof(buffer), 0);
	if (bytes_read < 0)
	{
		throw SocketError("Failed to receive client data: "
			+ std::string(strerror(errno)));
	}

	if (bytes_read == 0)
		throw SocketConnectionClosed(std::string(strerror(errno)));

	buffer[bytes_read] = '\0';
	return (http::HttpRequest(buffer));
}

} // namespace webserv