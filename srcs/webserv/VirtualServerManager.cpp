/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServerManager.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 20:50:46 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/08 23:02:06 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VirtualServerManager.hpp"

namespace	webserv
{

/* ************************************************************************** */

/* Socket constructor */
VirtualServerManager::VirtualServerManager(void)
	: _socket(), _defaultServer(NULL) {}


/* Destructor */
VirtualServerManager::~VirtualServerManager(void)
{
	for (std::vector<VirtualServer*>::iterator it = this->_servers.begin();
			it != this->_servers.end(); ++it)
		delete *it;
	
	for (std::vector<Client*>::iterator it = this->_incomingClients.begin();
			it != this->_incomingClients.end(); ++it)
		delete *it;
}


/* ************************************************************************** */
/* Getters */

Socket	VirtualServerManager::getSocket(void) const
{
	return (this->_socket);
}

const std::vector<VirtualServer*>	&VirtualServerManager::getServers(void)
	const
{
	return (this->_servers);
}

const std::vector<Client*>	&VirtualServerManager::getIncomingClients(void)
	const
{
	return (this->_incomingClients);
}


/* ************************************************************************** */
/* Setters */

void	VirtualServerManager::addServer(VirtualServer *server)
{
	if (!server)
		return ;

	this->_servers.push_back(server);

	if (this->_defaultServer)
		return ;

	const Configuration	&config = server->getConfiguration();

	this->_socket = Socket(socket(AF_INET, SOCK_STREAM, 0));
	if (this->_socket.getFd() < 0)
	{
		throw SocketCreationError("Failed to create socket: "
			+ std::string(strerror(errno)));
	}

	if (bind(this->_socket.getFd(),
		(struct sockaddr *) &(config.getConstAddress()),
		sizeof(config.getConstAddress())) == -1)
	{
		throw SocketError("Failed to bind socket: "
			+ std::string(strerror(errno)));
	}

	if (listen(this->_socket.getFd(), config.getBacklog()) == -1)
	{
		throw SocketError("Failed to listen on socket: "
			+ std::string(strerror(errno)));
	}

	this->_defaultServer = server;
}

void	VirtualServerManager::addIncomingClient(Client *client)
{
	if (!client)
		return ;

	this->_incomingClients.push_back(client);
}

// if (this->_socket.getFd() < 0 || setsockopt(this->_socket.getFd(),
// 	SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
// {
// 	throw SocketCreationError("Failed to create socket: "
// 		+ std::string(strerror(errno)));
// }

} // namespace webserv