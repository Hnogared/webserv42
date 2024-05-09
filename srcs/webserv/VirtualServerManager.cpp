/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServerManager.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 20:50:46 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/09 15:31:19 by hnogared         ###   ########.fr       */
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

const std::vector<const Socket*>	VirtualServerManager::getSockets(void) const
{
	std::vector<const Socket*>					sockets;
	std::vector<VirtualServer*>::const_iterator	serverIt;
	std::vector<Client*>::const_iterator 		clientIt;

	sockets.push_back(&this->_socket);

	for (clientIt = this->_incomingClients.begin();
			clientIt != this->_incomingClients.end(); clientIt++)
		sockets.push_back((*clientIt)->getSocketPtr());

	for (serverIt = this->_servers.begin(); serverIt != this->_servers.end();
		serverIt++)
	{
		const std::vector<Client*>	&clients = (*serverIt)->getClients();

		for (clientIt = clients.begin(); clientIt != clients.end(); clientIt++)
			sockets.push_back((*clientIt)->getSocketPtr());
	}
	return (sockets);
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

size_t	VirtualServerManager::getSocketsCount(void) const
{
	size_t										count = 1;
	std::vector<VirtualServer*>::const_iterator it;

	for (it = this->_servers.begin(); it != this->_servers.end(); ++it)
		count += (*it)->getClients().size();
	return (count + this->_incomingClients.size());
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
	if (fd == this->_socket.getFd())
	{
		this->_acceptConnection();
		return ;
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

		this->_incomingClients.push_back(new Client(socket));
	}
	catch (const std::exception &e)
	{
		close(client_fd);
		this->_log(Harl::ERROR, NULL, "Failed client connection: "
			+ std::string(e.what()));
		return ;
	}

	this->_log(Harl::INFO, this->_incomingClients.back(), "CONN ACCEPTED");
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