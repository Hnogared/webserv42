/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServerManager.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 12:06:54 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/14 20:26:44 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VirtualServerManager.hpp"

namespace	webserv
{

/* ************************************************************************** */

/* Socket constructor */
VirtualServerManager::VirtualServerManager(const http::Protocol &protocol,
		Harl *logger)
	: _protocol(protocol), _logger(logger), _socket(), _defaultServer(NULL) {}


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
	int optval = 1;

	if (!server)
		return ;

	this->_servers.push_back(server);

	if (this->_defaultServer)
		return ;

	const Configuration	&config = server->getConfiguration();

	this->_socket = Socket(socket(AF_INET, SOCK_STREAM, 0));
	if (this->_socket.getFd() < 0 || setsockopt(this->_socket.getFd(),
	 	SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
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

		if (this->_serveClient(*clientIt))
		{
			this->_log(Harl::INFO, *clientIt, "CONN CLOSED - Local host");
			delete *clientIt;
			this->_clients.erase(clientIt);
		}

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

bool	VirtualServerManager::_serveClient(Client *client)
{
	try
	{
		std::vector<VirtualServer*>::iterator	serverIt;

		client->fetchRequestLineAndHeaders(this->_protocol);

		for (serverIt = this->_servers.begin();
			serverIt != this->_servers.end(); serverIt++)
		{
			if ((*serverIt)->tryHandleClientRequest(*client))
				return (false);
		}

		if (this->_defaultServer->tryHandleClientRequest(*client))
			return (false);

		throw http::HttpRequest::RequestException(
			client->getRequest().getStatusLine(), 404);
	}
	catch (const http::HttpRequest::RequestException &e)
	{
		this->_log(Harl::INFO, client, tool::strings::toStr(e.code()));
		client->sendResponse(http::HttpResponse(e.code(),client->getRequest()));
		return (true);
	}
	catch (const SocketConnectionClosed &e)
	{
		this->_log(Harl::INFO, client, "CONN CLOSED - Remote host");
		return (true);
	}
	catch(const std::exception &e)
	{
		this->_log(Harl::ERROR, client, "500 - " + std::string(e.what()));
		client->sendResponse(http::HttpResponse(500, client->getRequest()));
		return (true);
	}
}

void	VirtualServerManager::_log(Harl::e_level level, const Client *client,
	const std::string &message)
{
	std::string	statusLine;
	std::string	logMessage;

	if (!client)
	{
		if (this->_logger)
			this->_logger->log(level, message);
		else
			Harl::complain(level, message);
		return ;
	}

	logMessage = client->getAddrStr(Client::PEER);
	statusLine = client->getRequest().getStatusLine();

	if (!statusLine.empty())
		logMessage += " REQ '" + client->getRequest().getStatusLine() + "'";
		
	logMessage += " " + message;

	if (this->_logger)
		this->_logger->log(level, logMessage);
	else
		Harl::complain(level, logMessage);
}

} // namespace webserv