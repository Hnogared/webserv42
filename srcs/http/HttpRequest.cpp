/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 23:01:55 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/14 21:52:36 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

namespace	http
{

/* ************************************************************************** */
/* Constructors */

/* Default constructor */
HttpRequest::HttpRequest(void)
	: HttpMessage(), _method(), _uri() {}

/* Copy constructor */
HttpRequest::HttpRequest(const HttpRequest &original) : HttpMessage()
{
	*this = original;
}


/* Destructor */
HttpRequest::~HttpRequest(void) {}


/* ************************************************************************** */
/* Operator overloads */

/* Copy assignment operator */
HttpRequest	&HttpRequest::operator=(const HttpRequest &original)
{
	if (this == &original)
		return (*this);

	this->HttpMessage::operator=(original);
	this->_method = original.getMethod();
	this->_uri = original.getUri();
	return (*this);
}

/* ************************************************************************** */
/* Getters */

const std::string	&HttpRequest::getMethod(void) const
{
	return (this->_method);
}

const std::string	&HttpRequest::getUri(void) const
{
	return (this->_uri);
}


/* ************************************************************************** */
/* Private methods */

/* Method to parse amd store the request line */
void	HttpRequest::parseRequestLine(std::string &line)
{
	std::string	version;
	
	this->setStatusLine(line);

	if (std::count(line.begin(), line.end(), ' ') != 2)
		throw BadRequest();

	{
		std::istringstream	lineStream(line);

		lineStream >> this->_method >> this->_uri >> version;
	}


	if (this->_method.empty() || this->_uri.empty() || version.empty())
		throw BadRequest();

	try
	{
		this->_uri = urlDecode(this->_uri);
		this->setProtocol(version);
	}
	catch(const std::invalid_argument &e)
	{
		throw BadRequest();
	}
	
}

/* Method to parse and store the headers */
void	HttpRequest::parseHeaders(std::string &headers)
{
	std::string			line;
	std::istringstream	iss(headers);

	while (std::getline(iss, line))
	{
		{
			std::string::size_type	colon_pos = line.find(':');
			std::string				header_name;
			std::string				header_value;

			if (colon_pos == std::string::npos)
				throw BadRequest();

			header_name = line.substr(0, colon_pos);
			header_value = tool::strings::trim(line.substr(colon_pos + 1),
				" \t\n\r");
			this->addHeader(header_name, header_value);
		}
	}

	if ((this->_method == "POST" && this->getHeader("Content-Length").empty())
			|| this->getHeader("Host").empty())
		throw BadRequest();
}


/* ************************************************************************** */
/* HttpMessage virtual methods overrides */

void	HttpRequest::clear(void)
{
	this->HttpMessage::clear();
	this->_method.clear();
	this->_uri.clear();
}


/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */

/* ***************************** */
/* RequestException              */
/* ***************************** */

/* Default constructor */
HttpRequest::RequestException::RequestException(void)
	: RuntimeError("Request error", 21) {}

/* Message and code constructor */
HttpRequest::RequestException::RequestException(const std::string &msg,
		int code)
	: RuntimeError(msg, code) {}

/* Copy constructor */
HttpRequest::RequestException::RequestException(const RequestException
		&original)
	: RuntimeError(original) {}


/* Destructor */
HttpRequest::RequestException::~RequestException(void) throw() {}


/* ***************************** */
/* Operator overloads */

HttpRequest::RequestException	&HttpRequest::RequestException::operator=(
	const RequestException &original)
{
	if (this == &original)
		return (*this);
	RuntimeError::operator=(original);
	return (*this);
}


/* ***************************** */
/* BadRequest                    */
/* ***************************** */

/* Default constructor */
HttpRequest::BadRequest::BadRequest(void)
	: RequestException("Invalid request", 400) {}

/* Message constructor */
HttpRequest::BadRequest::BadRequest(const std::string &msg)
	: RequestException(msg, 400) {}

/* Copy constructor */
HttpRequest::BadRequest::BadRequest(const BadRequest &original)
	: RequestException(original) {}


/* Destructor */
HttpRequest::BadRequest::~BadRequest(void) throw() {}


/* ***************************** */
/* Operator overloads */

HttpRequest::BadRequest	&HttpRequest::BadRequest::operator=(
	const BadRequest &original)
{
	if (this == &original)
		return (*this);
	RequestException::operator=(original);
	return (*this);
}


/* ***************************** */
/* BodyTooLarge                  */
/* ***************************** */

/* Default constructor */
HttpRequest::BodyTooLarge::BodyTooLarge(void)
	: RequestException("Request body is too large", 413) {}

/* Message constructor */
HttpRequest::BodyTooLarge::BodyTooLarge(const std::string &msg)
	: RequestException(msg, 413) {}

/* Copy constructor */
HttpRequest::BodyTooLarge::BodyTooLarge(const BodyTooLarge &original)
	: RequestException(original) {}


/* Destructor */
HttpRequest::BodyTooLarge::~BodyTooLarge(void) throw() {}


/* ***************************** */
/* Operator overloads */

HttpRequest::BodyTooLarge	&HttpRequest::BodyTooLarge::operator=(
	const BodyTooLarge &original)
{
	if (this == &original)
		return (*this);
	RequestException::operator=(original);
	return (*this);
}

} // namespace http