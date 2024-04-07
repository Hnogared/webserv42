/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 14:43:33 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 22:02:09 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

namespace	webserv
{

/* Static atrtibutes initialization */
const std::string	Client::CLASS_NAME = "Client";

/* ************************************************************************** */

/* Socket fd constructor */
Client::Client(int sock_fd) : _socket(sock_fd) {}

/* Copy constructor */
Client::Client(const Client &original)
{
	if (this != &original)
		this->_socket = original.getSocket();
}


/* Destructor */
Client::~Client(void) {}


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

/* ************************************************************************** */
/* Public methods */

void	Client::receiveData(void)
{
	char	buffer[1024];
	ssize_t	bytes_read;

	std::cout << "Receiving data from client..." << std::endl;
	bytes_read = recv(this->_socket.getFd(), buffer, sizeof(buffer), 0);
	if (bytes_read < 0)
	{
		throw std::runtime_error(CLASS_NAME + ": Failed to receive data: "
			+ std::string(strerror(errno)));
	}
	
	if (bytes_read == 0)
		throw std::runtime_error(CLASS_NAME + ": Connection closed by client");

	buffer[bytes_read] = '\0';
	std::cout << buffer << std::endl;
}

} // namespace webserv