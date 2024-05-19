/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServerManager.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 12:06:54 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/19 18:36:06 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VirtualServerManager.hpp"

namespace	webserv
{

/* ************************************************************************** */

/* Socket constructor */
VirtualServerManager::VirtualServerManager(const http::Protocol &protocol,
		Harl *logger)
	: _protocol(protocol),
	_logger(logger),
	_socket(),
	_defaultServer(NULL),
	_catchAllServer(NULL) {}


/* Destructor */
VirtualServerManager::~VirtualServerManager(void)
{
	for (std::vector<VirtualServer*>::iterator it = this->_servers.begin();
			it != this->_servers.end(); ++it)
		delete *it;
	
	if (this->_defaultServer != this->_catchAllServer)
		delete this->_catchAllServer;
	
	delete this->_defaultServer;
	
	for (std::vector<Client*>::iterator it = this->_clients.begin();
			it != this->_clients.end(); ++it)
		delete *it;
}


/* ************************************************************************** */
/* Getters */

const http::Protocol	&VirtualServerManager::getProtocol(void) const
{
	return (this->_protocol);
}

const Harl	*VirtualServerManager::getLogger(void) const
{
	return (this->_logger);
}

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

	if (!this->_catchAllServer
			&& server->getConfiguration().getServerNames().empty())
		this->_catchAllServer = server;

	if (!this->_defaultServer)
	{
		this->_initSocket(server->getConfiguration());
		this->_defaultServer = server;
	}
	else if (server != this->_catchAllServer)
		this->_servers.push_back(server);
}

void	VirtualServerManager::addClient(Client *client)
{
	if (client)
		this->_clients.push_back(client);
}


/* ************************************************************************** */
/* Public methods */

bool	VirtualServerManager::tryServeFd(int fd, short revents)
{
	std::vector<Client*>::iterator	clientIt;

	if (fd == this->_socket.getFd())
	{
		this->_acceptConnection();
		return (true);
	}

	for (clientIt = this->_clients.begin(); clientIt != this->_clients.end();
		clientIt++)
	{
		if (fd != (*clientIt)->getSocket().getFd())
			continue ;

		if (revents & (POLLHUP | POLLERR))
		{
			this->_log(Harl::INFO, *clientIt, "CONN CLOSED - Remote host");
			delete *clientIt;
			this->_clients.erase(clientIt);
			return (true);
		}

		this->_serveClient(clientIt);
		return (true);
	}

	return (false);
}

void	VirtualServerManager::stop(void)
{
	std::vector<Client*>::iterator	clientIt;

	for (clientIt = this->_clients.begin(); clientIt != this->_clients.end();
			clientIt++)
		delete *clientIt;

	this->_clients.clear();
}


/* ************************************************************************** */
/* Private methods */

void	VirtualServerManager::_initSocket(const Configuration &config)
{
	int	optval = 1;

	this->_socket = Socket(socket(AF_INET, SOCK_STREAM, 0));
	if (this->_socket.getFd() < 0 || setsockopt(this->_socket.getFd(),
	 	SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) < 0)
	{
		throw SocketCreationError("Failed to create socket: "
			+ std::string(strerror(errno)));
	}

	if (bind(this->_socket.getFd(),
		(struct sockaddr *) (&(config.getConstAddress())),
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
}

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

void	VirtualServerManager::_serveClient(
		std::vector<webserv::Client*>::iterator clientIt)
{
	Client									*client = *clientIt;
	std::vector<VirtualServer*>::iterator	serverIt = this->_servers.begin();

	try
	{
		client->fetchRequestLineAndHeaders(this->_protocol);

		for (; serverIt != this->_servers.end(); serverIt++)
		{
			if ((*serverIt)->tryHandleClientRequest(*client))
				return ;
		}

		if (this->_defaultServer->tryHandleClientRequest(*client,
				this->_catchAllServer == this->_defaultServer))
			return ;

		if (this->_catchAllServer
				&& this->_catchAllServer != this->_defaultServer)
			this->_catchAllServer->tryHandleClientRequest(*client, true);
	}
	catch (const SocketConnectionClosed& /* e */)
	{
		this->_log(Harl::INFO, client, "CONN CLOSED - Remote host");
		delete client;
		this->_clients.erase(clientIt);
	}
	catch (const std::exception& /* e */)
	{
		this->_log(Harl::INFO, client, "CONN CLOSED - Local host");
		delete client;
		this->_clients.erase(clientIt);
	}
}

void	VirtualServerManager::_log(Harl::e_level level, const Client *client,
	const std::string &message)
{
	if (!client)
	{
		if (this->_logger)
			this->_logger->log(level, message);
		else
			Harl::complain(level, message);
		return ;
	}

	std::string				logMessage = client->getAddrStr(Client::PEER);
	std::string				statusLine = client->getRequest().getStatusLine();
	const http::HttpRequest	&request = client->getRequest();

	if (!statusLine.empty())
		logMessage += " REQ '" + request.getStatusLine() + "'";

	logMessage += " " + message;

	if (!request.getHeader("Host").empty())
		logMessage += " - " + request.getHeader("Host");

	if (!request.getHeader("User-Agent").empty())
		logMessage += " - '" + request.getHeader("User-Agent") + "'";

	if (this->_logger)
		this->_logger->log(level, logMessage);
	else
		Harl::complain(level, logMessage);
}

} // namespace webserv