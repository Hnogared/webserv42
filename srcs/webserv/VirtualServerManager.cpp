/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServerManager.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 20:50:46 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/10 22:18:55 by hnogared         ###   ########.fr       */
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
	
	for (std::vector<Client*>::iterator it = this->_clients.begin();
			it != this->_clients.end(); ++it)
		delete *it;
}


/* ************************************************************************** */
/* Getters */

Socket	VirtualServerManager::getSocket(void) const
{
	return (this->_socket);
}

const std::vector<const Socket*>	VirtualServerManager::getSockets(void) const
{
	std::vector<const Socket*>					sockets;
	std::vector<VirtualServer*>::const_iterator	serverIt;
	std::vector<Client*>::const_iterator 		clientIt;

	sockets.push_back(&this->_socket);

	for (clientIt = this->_clients.begin(); clientIt != this->_clients.end();
			clientIt++)
		sockets.push_back((*clientIt)->getSocketPtr());

	return (sockets);
}

const std::vector<VirtualServer*>	&VirtualServerManager::getServers(void)
	const
{
	return (this->_servers);
}

const std::vector<Client*>	&VirtualServerManager::getClients(void) const
{
	return (this->_clients);
}

size_t	VirtualServerManager::getSocketsCount(void) const
{
	return (this->_clients.size() + 1);
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

void	VirtualServerManager::addClient(Client *client)
{
	if (!client)
		return ;

	this->_clients.push_back(client);
}

// if (this->_socket.getFd() < 0 || setsockopt(this->_socket.getFd(),
// 	SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
// {
// 	throw SocketCreationError("Failed to create socket: "
// 		+ std::string(strerror(errno)));
// }

/* ************************************************************************** */
/* Public methods */

bool	VirtualServerManager::handlesFd(int fd) const
{
	std::vector<const Socket*>::const_iterator	it;
	const std::vector<const Socket*>			&sockets = this->getSockets();

	for (it = sockets.begin(); it != sockets.end(); it++)
	{
		if (fd == (*it)->getFd())
			return (true);
	}

	return (false);
}

void	VirtualServerManager::serveFd(int fd)
{
	std::vector<Client*>::iterator	clientIt;

	if (fd == this->_socket.getFd())
	{
		this->_acceptConnection();
		return ;
	}

	for (clientIt = this->_clients.begin(); clientIt != this->_clients.end();
		clientIt++)
	{
		if (fd != (*clientIt)->getSocket().getFd())
			continue ;

		this->_serveClient(*clientIt);
	}
}


/* ************************************************************************** */
/* Private methods */

void	VirtualServerManager::_acceptConnection(void)
{
	int					client_fd;
	struct sockaddr_in	client_address;
	socklen_t			client_address_len = sizeof(client_address);

	client_fd = accept(this->_socket.getFd(),
			(struct sockaddr *)&client_address, &client_address_len);

	if (client_fd == -1)
	{
		this->_log(Harl::ERROR, NULL, "Failed client connection: "
			+ std::string(strerror(errno)));
		return ;
	}

	try
	{
		Socket	socket(client_fd, client_address);

		this->_clients.push_back(new Client(socket));
	}
	catch (const std::exception &e)
	{
		close(client_fd);
		this->_log(Harl::ERROR, NULL, "Failed client connection: "
			+ std::string(e.what()));
		return ;
	}

	this->_log(Harl::INFO, this->_clients.back(), "CONN ACCEPTED");
}

void	VirtualServerManager::_serveClient(const Client *client)
{
	(void)client;
	// std::vector<VirtualServer*>::const_iterator	serverIt;
	// const std::vector<VirtualServer*>			&servers = this->getServers();

	// for (serverIt = servers.begin(); serverIt != servers.end(); serverIt++)
	// {
	// 	if ((*serverIt)->handlesClient(client))
	// 	{
	// 		(*serverIt)->serveClient(client);
	// 		return ;
	// 	}
	// }

	// this->_log(Harl::ERROR, client, "No server to handle client");
}

void	VirtualServerManager::_log(Harl::e_level level, const Client *client,
	const std::string &message)
{
	std::string	dispMessage;

	if (client)
		dispMessage = client->getAddrStr(Client::PEER) + "  " + message;
	else
		dispMessage = message;

	Harl::complain(level, dispMessage);
}

} // namespace webserv