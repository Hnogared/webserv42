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

const Configuration	&VirtualServer::getConfiguration(void) const
{
	return (this->_config);
}

webserv::Socket	VirtualServer::getSocket(void) const
{
	return (this->_socket);
}

const std::vector<Client>	&VirtualServer::getClients(void) const
{
	return (this->_clients);
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
			this->acceptConnection();
		for (size_t i = 1; i < clients_count + 1; i++)
		{
			if (poll_fds[i].revents & POLLIN)
				this->handleRequest(this->_clients[i - 1]);
		}
	}
}


/* ************************************************************************** */
/* Private methods */

void	VirtualServer::acceptConnection(void)
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
		this->_clients.push_back(Client(Socket(client_fd, client_address)));
	}
	catch (const std::exception &e)
	{
		close(client_fd);
		this->_log(Harl::ERROR, NULL, "Failed client connection: "
			+ std::string(e.what()));
		return ;
	}

	this->_log(Harl::INFO, &(this->_clients.back()), "CONN ACCEPTED");
}

void	VirtualServer::handleRequest(const Client &client)
{
	http::HttpRequest	request;

	try
	{
		request = client.fetchRequest(this->_config.getClientMaxBodySize());
	}
	catch (const http::HttpRequest::RequestException &e)
	{
		this->_log(Harl::INFO, &client, "REQ '" + std::string(e.what()) + "' "
			+ tool::strings::toStr(e.code()));

		client.sendResponse(http::HttpResponse(e.code()));
		this->_clients.erase(std::remove(this->_clients.begin(),
			this->_clients.end(), client), this->_clients.end());
		return ;
	}
	catch (const SocketConnectionClosed &e)
	{
		this->_log(Harl::INFO, &client, "CONN CLOSED");

		this->_clients.erase(std::remove(this->_clients.begin(),
			this->_clients.end(), client), this->_clients.end());
		return ;
	}
	catch(const std::exception& e)
	{
		this->_log(Harl::ERROR, &client, "500 - Failed to fetch request: "
			+ std::string(e.what()));

		client.sendResponse(http::HttpResponse(500));
		this->_clients.erase(std::remove(this->_clients.begin(),
			this->_clients.end(), client), this->_clients.end());
		return ;
	}

	this->_sendResponse(client, request);
}


/* ************************************************************************** */
/* Private methods */

void	VirtualServer::_log(Harl::e_level level, const Client *client,
	const std::string &message)
{
	std::string	dispMessage;

	if (client)
		dispMessage = client->getAddrStr(Client::PEER) + "  " + message;
	else
		dispMessage = message;

	Harl::complain(level, dispMessage);
}

void	VirtualServer::_sendResponse(const Client &client,
	const http::HttpRequest &request)
{
	std::string	message;
	std::string	uri = request.getTarget();

	message = client.getAddrStr(Client::PEER) + " REQ '"
		+ request.getStatusLine() + "'";

	if (!request.isValid())
	{
		client.sendResponse(http::HttpResponse(400));
		return ;
	}

	if (request.getVersion() != WS_HTTP_VERSION)
	{
		client.sendResponse(http::HttpResponse(505));
		return ;
	}

	if (*(uri.end() - 1) == '/')
	{
		this->_sendDirectoryResponse(client, request);
		return ;
	}

	http::HttpResponse	response(200);

	client.sendResponse(response);
}

void	VirtualServer::_sendDirectoryResponse(const Client &client,
	const http::HttpRequest &request)
{
	std::string	uri = request.getTarget();
	std::string	path;
	std::set<LocationConfiguration>::const_iterator	location;

	for (location = this->_config.getLocations().begin();
		location != this->_config.getLocations().end(); location++)
	{
		if (location->getPath() != uri)
			continue ;

		if (!location->getIndex().empty())
		{
			path = tool::files::joinPaths(location->getRoot(), uri);
			path = tool::files::joinPaths(path, location->getIndex());

			try
			{
				http::HttpResponse	response(200);

				response.setBody(tool::files::readFile(path));
				client.sendResponse(response);
				return ;
			}
			catch(const std::exception& /* e */) {}
		}

		if (location->isAutoindex())
		{
			path = tool::files::joinPaths(location->getRoot(), uri);
			this->_sendDirectoryListing(client, uri, path);
			return ;
		}
	}

	client.sendResponse(http::HttpResponse(404));
}

void	VirtualServer::_sendDirectoryListing(const Client &client,
	const std::string &uri, const std::string &path)
{
	std::string		icon;
	std::string		body;
	DIR				*dir;
	struct dirent	*entry;

	dir = opendir(path.c_str());
	if (!dir)
	{
		client.sendResponse(http::HttpResponse(404));
		return ;
	}

	body = "<html>\n"
		"<head><title>Index of " + uri + "</title></head>\n"
		"<body>\n"
		"  <h1>Index of " + uri + "</h1>\n"
		"  <hr><pre>\n";

	while ((entry = readdir(dir)))
	{
		switch (entry->d_type)
		{
			case DT_REG:
				icon = "&#128196;";
				break;
			default:
				icon = "&#128193;";
				break;
		}
		body += "  <span>" + icon + " " + "</span>"
			+ "<a href=\"" + uri + entry->d_name + "\">"
			+ entry->d_name + "</a><br>\n";
	}

	body += "  </pre><hr>\n"
		"</body>\n"
		"</html>\n";

	closedir(dir);

	http::HttpResponse	response(200);

	response.setBody(body);
	client.sendResponse(response);
}

} // namespace webserv