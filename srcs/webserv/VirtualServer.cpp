/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 12:36:55 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/19 17:32:57 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VirtualServer.hpp"

namespace	webserv
{

/* ************************************************************************** */

/* Default constructor */
VirtualServer::VirtualServer(const Configuration &config) : _config(config)
{
	int	optval = 1;

	this->_socket = Socket(socket(AF_INET, SOCK_STREAM, 0));
	if (this->_socket.getFd() < 0 || setsockopt(this->_socket.getFd(),
		SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
	{
		throw SocketCreationError("Failed to create socket: "
			+ std::string(strerror(errno)));
	}

	if (bind(this->_socket.getFd(),
		(struct sockaddr *)&(this->_config.getConstAddress()),
		sizeof(this->_config.getAddress())) == -1)
	{
		throw SocketError("Failed to bind socket: "
			+ std::string(strerror(errno)));
	}

	if (listen(this->_socket.getFd(), this->_config.getBacklog()) == -1)
	{
		throw SocketError("Failed to listen on socket: "
			+ std::string(strerror(errno)));
	}
}


/* Destructor */
VirtualServer::~VirtualServer(void) {}


/* ************************************************************************** */
/* Getters */

webserv::Socket	VirtualServer::getSocket(void) const
{
	return (this->_socket);
}

const Configuration	&VirtualServer::getConfiguration(void) const
{
	return (this->_config);
}


/* ************************************************************************** */
/* Public methods */

void	VirtualServer::update(void)
{
	int				res;
	size_t			clients_count = this->_clients.size();
	struct pollfd	poll_fds[clients_count + 1];

	poll_fds[0].fd = this->_socket.getFd();
	poll_fds[0].events = POLLIN;

	for (size_t i = 1; i < clients_count + 1; i++)
	{
		poll_fds[i].fd = this->_clients[i - 1].getSocketFd();
		poll_fds[i].events = POLLIN;
	}

	res = poll(poll_fds, clients_count + 1, 100);

	if (res == -1)
		throw SocketError("Failed poll: " + std::string(strerror(errno)));

	if (res)
	{
		if (poll_fds[0].revents & POLLIN)
			this->_acceptConnection();
		for (size_t i = 1; i < clients_count + 1; i++)
		{
			if (poll_fds[i].revents & POLLIN)
				this->_handleRequest(this->_clients[i - 1]);
		}
	}
}


/* ************************************************************************** */
/* Private methods */

void	VirtualServer::_acceptConnection(void)
{
	int					client_fd;
	struct sockaddr_in	client_address;
	socklen_t			client_address_len = sizeof(client_address);

	client_fd = accept(this->_socket.getFd(),
			(struct sockaddr *)&client_address, &client_address_len);
	if (client_fd == -1)
	{
		Harl::complain(Harl::ERROR, "Failed client connection: "
			+ std::string(strerror(errno)));
		return ;
	}

	try
	{
		this->_clients.push_back(Client(Socket(client_fd, client_address)));
	}
	catch (const std::exception &e)
	{
		std::stringstream error_message;

		close(client_fd);
		error_message << "Failed client connection: " << e.what();
		Harl::complain(Harl::ERROR, error_message.str());
		return ;
	}

	Harl::complain(Harl::INFO, this->_clients.back().getAddrStr(Client::PEER)
		+ " CONN ACCEPTED");
}

void	VirtualServer::_handleRequest(const Client &client)
{
	http::HttpRequest request;

	try
	{
		request = client.fetchRequest();
	}
	catch (const SocketConnectionClosed &e)
	{
		Harl::complain(Harl::INFO, client.getAddrStr(Client::PEER)
			+ " CONN CLOSED");
		this->_clients.erase(std::remove(this->_clients.begin(),
			this->_clients.end(), client), this->_clients.end());
		return ;
	}
	catch(const std::exception& e)
	{
		Harl::complain(Harl::ERROR, client.getAddrStr(Client::PEER)
			+ " Failed to fetch request: " + e.what());
		return ;
	}

	Harl::complain(Harl::INFO, client.getAddrStr(Client::PEER) + " REQ '"
		+ request.getStatusLine() + "'");

	if (!request.isValid())
	{
		client.sendResponse(http::HttpResponse(400, "Bad Request", "HTTP/1.1"));
		return ;
	}

	client.sendResponse(http::HttpResponse(200, "OK", "HTTP/1.1"));
}

} // namespace webserv