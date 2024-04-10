/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 12:36:55 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/09 19:59:29 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

namespace	webserv
{

/* Static class attributes initialization */
bool	Server::running = false;

/* ************************************************************************** */
/* Constructors */

/* Default constructor */
Server::Server(int port, int backlog) : _port(port), _backlog(backlog)
{
	signal(SIGINT, sigHandler);

	{
		std::ifstream	lock_file(LOCK_FILE);

		if (lock_file.is_open())
		{
			lock_file.close();
			throw std::runtime_error("Another instance is already running");
		}
	}

	{
		std::ofstream	lock_file_out(LOCK_FILE);

		if (!lock_file_out.is_open())
			throw std::runtime_error("Failed to create lock file");
		lock_file_out.close();
	}

	int	optval = 1;

	this->_socket = Socket(socket(AF_INET, SOCK_STREAM, 0));
	if (this->_socket.getFd() < 0 || setsockopt(this->_socket.getFd(),
		SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
	{
		throw SocketCreationError("Failed to create socket: "
			+ std::string(strerror(errno)));
	}

	this->_server_address.sin_family = AF_INET;
	this->_server_address.sin_addr.s_addr = htonl(INADDR_ANY); // All interfaces
	this->_server_address.sin_port = htons(port);

	if (bind(this->_socket.getFd(), (struct sockaddr *)&_server_address,
		sizeof(this->_server_address)) == -1)
	{
		throw SocketError("Failed to bind socket: "
			+ std::string(strerror(errno)));
	}

	if (listen(this->_socket.getFd(), backlog) == -1)
	{
		throw SocketError("Failed to listen on socket: "
			+ std::string(strerror(errno)));
	}
}


/* Destructor */
Server::~Server(void)
{
	remove(LOCK_FILE);
}


/* ************************************************************************** */
/* Getters */

bool	Server::isRunning(void) const
{
	return (Server::running);
}

webserv::Socket	Server::getSocket(void) const
{
	return (this->_socket);
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

void	Server::run(void)
{
	Server::running = true;

	while (Server::running)
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

		res = poll(poll_fds, clients_count + 1, -1); // Wait infitely
		
		if (!Server::running)
			break;
		
		if (res == -1)
			throw SocketError("Failed poll: " + std::string(strerror(errno)));

		if (res)
		{
			if (poll_fds[0].revents & POLLIN)
				this->acceptConnection();
			for (size_t i = 1; i < clients_count + 1; i++)
			{
				if (poll_fds[i].revents & POLLIN)
					this->handleRequest(this->_clients[i - 1]);
			}
		}
	}
}


void	Server::acceptConnection(void)
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


void	Server::handleRequest(const Client &client)
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


/* ************************************************************************** */
/* Private static methods */

 /* Method to execute when receiving signals */
void	Server::sigHandler(int signal)
{
	if (signal == SIGINT)
	{
		remove(LOCK_FILE);
		Server::running = false;

		Harl::complain(Harl::INFO, "Received SIGINT, stopping...");
	}
}

} // namespace webserv