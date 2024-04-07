/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 14:43:33 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 23:56:31 by hnogared         ###   ########.fr       */
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

void	Client::sendResponse(const HttpResponse &response)
{
	ssize_t		bytes_sent;
	std::string	response_str(response.toString());

	bytes_sent = send(this->_socket.getFd(), response_str.c_str(),
			response_str.size(), 0);
	if (bytes_sent < 0)
	{
		throw std::runtime_error(CLASS_NAME + ": Failed to send data: "
			+ std::string(strerror(errno)));
	}
}

void	Client::getRequest(void)
{
	char	buffer[1024];
	ssize_t	bytes_read;

	bytes_read = recv(this->_socket.getFd(), buffer, sizeof(buffer), 0);
	if (bytes_read < 0)
	{
		throw std::runtime_error(CLASS_NAME + ": Failed to receive data: "
			+ std::string(strerror(errno)));
	}

	if (bytes_read == 0)
		throw std::runtime_error(CLASS_NAME + ": Connection closed by client");

	buffer[bytes_read] = '\0';
	this->parseRequest(std::string(buffer));
}

void	Client::parseRequest(const std::string &request)
{
	Harl::complain(Harl::INFO, request);

	try
	{
		HttpRequest	req(request);
	}
	catch (const HttpRequest::BadRequest &e)
	{
		HttpResponse	response("HTTP/1.1", 400, "Bad Request");

		response.setBody(e.what());
		this->sendResponse(response);
		return ;
	}
	catch (const std::exception &e)
	{
		Harl::complain(Harl::ERROR, e.what());
		this->sendResponse(HttpResponse("HTTP/1.1", 505, "Server Error"));
		return ;
	}

	this->sendResponse(HttpResponse("HTTP/1.1", 200, "OK"));
}

} // namespace webserv