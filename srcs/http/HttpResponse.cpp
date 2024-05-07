/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 23:28:04 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/07 14:58:42 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

namespace	http
{

/* ************************************************************************** */
/* Static attributes initialization */

const std::map<int, std::string>	HttpResponse::_statusLines
	= HttpResponse::_initStatusLines();

/* ************************************************************************** */

/* Status code + status line constructor */
HttpResponse::HttpResponse(int status_code, const std::string &status_line)
	: HttpMessage(status_line), _statusCode(status_code)
{
	if (status_code < 100 || status_code > 599)
		throw std::invalid_argument("Invalid status code");
	
	this->_buildHeadersAndBody();
}

/* Copy constructor */
HttpResponse::HttpResponse(const HttpResponse &original) : HttpMessage()
{
	*this = original;
}


/* Destructor */
HttpResponse::~HttpResponse(void) {}


/* ************************************************************************** */
/* Operator overloads */

/* Copy assignment operator */
HttpResponse	&HttpResponse::operator=(const HttpResponse &original)
{
	if (this == &original)
		return (*this);
	this->HttpMessage::operator=(original);
	this->_statusCode = original.getStatusCode();
	return (*this);
}


/* ************************************************************************** */
/* Getters */

int	HttpResponse::getStatusCode(void) const
{
	return (this->_statusCode);
}


/* ************************************************************************** */
/* Public methods */

std::string	HttpResponse::toString(void) const
{
	std::ostringstream							result;
	const std::map<std::string, std::string>	&headers = this->getHeaders();
	std::map<std::string, std::string>::const_iterator	it;

	result << this->getVersion() << " "
		<< this->_statusCode << " "
		<< this->getStatusLine() << "\r\n";

	for (it = headers.begin(); it != headers.end(); it++)
		result << it->first << ": " << it->second << "\r\n";
	
	result << "\r\n" << this->getBody();
	return (result.str());
}


/* ************************************************************************** */
/* Private static methods */

std::map<int, std::string>	HttpResponse::_initStatusLines(void)
{
	std::map<int, std::string>	statusLines;

	statusLines[100] = "Continue";
	statusLines[101] = "Switching Protocols";
	statusLines[200] = "OK";
	statusLines[201] = "Created";
	statusLines[202] = "Accepted";
	statusLines[203] = "Non-Authoritative Information";
	statusLines[204] = "No Content";
	statusLines[205] = "Reset Content";
	statusLines[206] = "Partial Content";
	statusLines[300] = "Multiple Choices";
	statusLines[301] = "Moved Permanently";
	statusLines[302] = "Found";
	statusLines[303] = "See Other";
	statusLines[304] = "Not Modified";
	statusLines[305] = "Use Proxy";
	statusLines[307] = "Temporary Redirect";
	statusLines[400] = "Bad Request";
	statusLines[401] = "Unauthorized";
	statusLines[402] = "Payment Required";
	statusLines[403] = "Forbidden";
	statusLines[404] = "Not Found";
	statusLines[405] = "Method Not Allowed";
	statusLines[406] = "Not Acceptable";
	statusLines[407] = "Proxy Authentication Required";
	statusLines[408] = "Request Timeout";
	statusLines[409] = "Conflict";
	statusLines[410] = "Gone";
	statusLines[411] = "Length Required";
	statusLines[412] = "Precondition Failed";
	statusLines[413] = "Payload Too Large";
	statusLines[414] = "URI Too Long";
	statusLines[415] = "Unsupported Media Type";
	statusLines[416] = "Range Not Satisfiable";
	statusLines[417] = "Expectation Failed";
	statusLines[426] = "Upgrade Required";
	statusLines[500] = "Internal Server Error";
	statusLines[501] = "Not Implemented";
	statusLines[502] = "Bad Gateway";
	statusLines[503] = "Service Unavailable";
	statusLines[504] = "Gateway Timeout";
	statusLines[505] = "HTTP Version Not Supported";

	return (statusLines);
}

std::string	HttpResponse::_makeBody(int statusCode)
{
	std::string	body;
	std::string	message;

	message = tool::strings::toStr(statusCode) + " "
		+ HttpResponse::_statusLines.at(statusCode);

	body = "<html>\n"
		"<head><title>" + message + "</title></head>\n"
		"<body>\n"
		"<center><h1>" + message + "</h1></center>\n"
		"<hr><center>" WS_SERVER_NAME "/" WS_SERVER_VERSION "</center>\n"
		"</body>\n"
		"</html>\n";

	return (body);
}


/* ************************************************************************** */
/* Private methods */

void	HttpResponse::_buildHeadersAndBody(void)
{
	int	codeFamily = this->_statusCode / 100;

	this->addHeader("Server", WS_SERVER_NAME "/" WS_SERVER_VERSION);

	switch (codeFamily)
	{
		case 1:
			this->addHeader("Connection", "Upgrade");
			this->addHeader("Content-Type", "text/plain");
			break;

		case 2: case 3:
			this->addHeader("Connection", "Keep-Alive");
			this->addHeader("Content-Type", "text/html");
			break;
		
		case 4: case 5:
			this->addHeader("Connection", "Close");
			this->addHeader("Content-Type", "text/html");
			break;

		default:
			break;
	}

	if (this->_statusCode >= 400 && this->_statusCode <= 599)
		this->setBody(HttpResponse::_makeBody(this->_statusCode));
	else
		this->setBody(this->_statusLines.at(this->_statusCode));

	this->setStatusLine(this->getVersion() + " "
		+ tool::strings::toStr(this->_statusCode) + " "
		+ this->_statusLines.at(this->_statusCode));

	this->addHeader("Content-Length",
		tool::strings::toStr(this->getBody().size()));
}

} // namespace http