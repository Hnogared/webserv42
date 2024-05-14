/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 14:43:33 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/13 19:06:27 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

namespace	webserv
{

/* ************************************************************************** */

/* Socket fd constructor */
Client::Client(const Socket &socket)
	: _socket(socket), _requestPending(), _request(), _buffer("") {}

/* Copy constructor */
Client::Client(const Client &original)
{
	*this = original;
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
	this->_requestPending = original.isRequestPending();
	this->_request = original.getRequest();
	this->_buffer = original.getBuffer();
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

const Socket	*Client::getSocketPtr(void) const
{
	return (&this->_socket);
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

bool	Client::isRequestPending(void) const
{
	return (this->_requestPending);
}

const http::HttpRequest	&Client::getRequest(void) const
{
	return (this->_request);
}

const std::string	&Client::getBuffer(void) const
{
	return (this->_buffer);
}


/* ************************************************************************** */
/* Public methods */

void	Client::sendResponse(const http::HttpResponse &response)
{
	int			code = response.getStatusCode();
	ssize_t		bytes_sent;
	std::string	response_str(response.toString());

	if (code >= 100 && code < 400 && this->_requestPending)
		this->fetchRequestBody(0);
	this->_request.clear();

	bytes_sent = send(this->_socket.getFd(), response_str.c_str(),
			response_str.size(), 0);
	if (bytes_sent < 0)
		throw ClientWriteException(strerror(errno));
}

void	Client::fetchRequestLineAndHeaders(const http::Protocol &protocol)
{
	size_t		pos;
	std::string	content("");
	std::string	temp;

	while (content.find("\r\n") == std::string::npos)
	{
		temp = this->_readRequestBlock();
		content += temp;

		if (!temp.empty())
			continue ;

		if (content.empty())
			throw SocketConnectionClosed();
		
		this->_request.setStatusLine(content);
		throw http::HttpRequest::BadRequest();
	}

	pos = content.find("\r\n");
	temp = content.substr(0, pos);
	this->_request.parseRequestLine(temp);

	if (this->_request.getProtocol() > protocol)
		throw http::HttpRequest::RequestException("Bad protocol", 505);

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
	this->_request.parseHeaders(temp);

	this->_buffer = content.substr(pos + 4 - 2 * (pos == 0));
	this->_requestPending = true;
}

void	Client::fetchRequestBody(size_t maxBodyLen)
{
	size_t		bodySize;
	std::string	content;
	std::string	temp;

	content = this->_buffer;
	this->_buffer.clear();

	if (this->_request.getMethod() != "POST")
		return ;

	bodySize = tool::strings::stoi(this->_request.getHeader("Content-Length"));
	if (maxBodyLen && bodySize > maxBodyLen)
		throw http::HttpRequest::BodyTooLarge(this->_request.getStatusLine());

	while (content.size() < bodySize)
	{
		temp = this->_readRequestBlock(bodySize - content.size());
		if (temp.empty())
			break;

		content += temp;
	}

	this->_request.setBody(content);
	this->_requestPending = false;
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
		throw ClientReadException(strerror(errno));
	
	buffer[bytesRead] = '\0';
	return (std::string(buffer));
}


/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */

/* *********************************** */
/* ClientException                     */
/* *********************************** */

/* Default constructor */
Client::ClientException::ClientException(void)
	: RuntimeError("Client exception", 31) {}

/* String and code constructor */
Client::ClientException::ClientException(const std::string &msg, int code)
	: RuntimeError(msg, code) {}

/* Copy constructor */
Client::ClientException::ClientException(const ClientException &original)
	: RuntimeError(original) {}


/* Destructor */
Client::ClientException::~ClientException(void) throw() {}


/* *********************************** */
/* Operator overloads */

Client::ClientException	&Client::ClientException::operator=(
	const ClientException &original)
{
	RuntimeError::operator=(original);
	return (*this);
}


/* *********************************** */
/* ClientReadException                 */
/* *********************************** */

/* Default constructor */
Client::ClientReadException::ClientReadException(void)
	: ClientException("Failed read from client", 32) {}

/* String and code constructor */
Client::ClientReadException::ClientReadException(const std::string &msg)
	: ClientException("Failed read from client: " + msg, 32) {}

/* Copy constructor */
Client::ClientReadException::ClientReadException(const ClientReadException
	&original) : ClientException(original) {}


/* Destructor */
Client::ClientReadException::~ClientReadException(void) throw() {}


/* *********************************** */
/* Operator overloads */

Client::ClientReadException	&Client::ClientReadException::operator=(
	const ClientReadException &original)
{
	ClientException::operator=(original);
	return (*this);
}


/* *********************************** */
/* ClientWriteException                */
/* *********************************** */

/* Default constructor */
Client::ClientWriteException::ClientWriteException(void)
	: ClientException("Failed write to client", 33) {}

/* String and code constructor */
Client::ClientWriteException::ClientWriteException(const std::string &msg)
	: ClientException("Failed write to client: " + msg, 33) {}

/* Copy constructor */
Client::ClientWriteException::ClientWriteException(const ClientWriteException
	&original) : ClientException(original) {}


/* Destructor */
Client::ClientWriteException::~ClientWriteException(void) throw() {}


/* *********************************** */
/* Operator overloads */

Client::ClientWriteException	&Client::ClientWriteException::operator=(
	const ClientWriteException &original)
{
	ClientException::operator=(original);
	return (*this);
}
} // namespace webserv