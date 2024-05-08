/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 14:43:33 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/08 05:55:47 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

namespace	webserv
{

/* ************************************************************************** */

/* Socket fd constructor */
Client::Client(const Socket &socket) : _socket(socket) {}

/* Copy constructor */
Client::Client(const Client &original)
{
	if (this != &original)
		this->_socket = original.getSocket();
}


/* Destructor */
Client::~Client(void) {}


/* ************************************************************************** */
/* Operator overloads */

/* Copy assignment operator */
Client	&Client::operator=(const Client &original)
{
	if (this == &original)
		return (*this);
	this->_socket = original.getSocket();
	return (*this);
}

/* Equality comparison operator */
bool	Client::operator==(const Client &other) const
{
	return (this->_socket == other.getSocket());
}


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

std::string	Client::getAddrStr(e_addr_choice choice) const
{
	std::ostringstream	oss;
	struct sockaddr_in	addr;

	if (choice == LOCAL)
		addr = this->_socket.getLocalAddr();
	else if (this->_socket.isPeerAddrSet())
		addr = this->_socket.getPeerAddr();
	else
		return ("Unknown");

	oss << tool::net::inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port);
	return (oss.str());
}


/* ************************************************************************** */
/* Public methods */

void	Client::sendResponse(const http::HttpResponse &response) const
{
	ssize_t		bytes_sent;
	std::string	response_str(response.toString());

	bytes_sent = send(this->_socket.getFd(), response_str.c_str(),
			response_str.size(), 0);
	if (bytes_sent < 0)
	{
		throw std::runtime_error("Failed to send data to client: "
			+ std::string(strerror(errno)));
	}
}

http::HttpRequest	Client::fetchRequest(size_t maxBodySize) const
{
	size_t				bodySize;
	size_t				pos;
	std::string			content;
	std::string			temp;
	http::HttpRequest	request;

	content = this->_readRequestBlock();

	if (content.empty())
		throw SocketConnectionClosed();

	pos = content.find("\r\n");
	temp = content.substr(0, pos);
	request.parseRequestLine(temp);

	content = content.substr(pos + 2);

	while (true)
	{
		pos = content.find("\r\n");
		if (pos == 0)
			break;

		pos = content.find("\r\n\r\n");
		if (pos != std::string::npos)
			break;

		temp = this->_readRequestBlock();
		if (temp.empty())
			break;

		content += temp;
	}

	temp = content.substr(0, pos);
	request.parseHeaders(temp);
	content = content.substr(pos + 4 - 2 * (pos == 0));

	if (request.getMethod() != "POST")
		return (request);

	bodySize = tool::strings::stoi(request.getHeader("Content-Length"));
	if (bodySize > maxBodySize)
		throw RequestBodyTooLarge();

	while (content.size() < bodySize)
	{
		temp = this->_readRequestBlock(bodySize - content.size());
		if (temp.empty())
			break;

		content += temp;
	}

	request.setBody(content);
	return (request);
}


/* ************************************************************************** */
/* Private methods */

std::string	Client::_readRequestBlock(size_t maxBuffSize) const
{
	int		bytesRead;
	char	buffer[WS_CLIENT_BUFF_SIZE];

	if (maxBuffSize == 0 || WS_CLIENT_BUFF_SIZE - 1 < maxBuffSize)
		maxBuffSize = WS_CLIENT_BUFF_SIZE - 1;

	bytesRead = recv(this->_socket.getFd(), buffer, maxBuffSize, 0);

	if (bytesRead < 0)
	{
		throw RuntimeError("Failed to receive data from client: "
			+ std::string(strerror(errno)), errno);
	}
	
	buffer[bytesRead] = '\0';

	return (std::string(buffer));
}


/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */

/* ***************************** */
/* RequestBodyTooLarge           */
/* ***************************** */

/* Default constructor */
Client::RequestBodyTooLarge::RequestBodyTooLarge(void)
	: RuntimeError("Request body is too large", 22) {}

/* Message constructor */
Client::RequestBodyTooLarge::RequestBodyTooLarge(const std::string &msg)
	: RuntimeError(msg, 22) {}

/* Copy constructor */
Client::RequestBodyTooLarge::RequestBodyTooLarge(
		const RequestBodyTooLarge &original)
	: RuntimeError(original) {}


/* Destructor */
Client::RequestBodyTooLarge::~RequestBodyTooLarge(void) throw() {}


/* ***************************** */
/* Operator overloads */

Client::RequestBodyTooLarge	&Client::RequestBodyTooLarge::operator=(
		const RequestBodyTooLarge &original)
{
	if (this == &original)
		return (*this);
	RuntimeError::operator=(original);
	return (*this);
}

} // namespace webserv