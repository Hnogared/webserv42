/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 23:01:55 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 23:16:42 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

namespace	webserv
{

/* ************************************************************************** */
/* Constructors */

/* Request constructor */
HttpRequest::HttpRequest(const std::string &request)
{
	std::istringstream	iss_request(request);
	std::string			line;

	std::getline(iss_request, line);

	// Check for CRLF at the end of the request line and remove it
	if (line.size() < 2 || line.substr(line.size() - 2) != "\r\n")
		throw BadRequest("Missing CRLF at end of request line");
	line = line.substr(0, line.size() - 2);

	std::istringstream line_stream(line);
	line_stream >> this->_method >> this->_target >> this->_version;

	if (this->_method.empty() || this->_target.empty()
		|| this->_version != "HTTP/1.1")
	{
		throw BadRequest("Invalid request line");
	}
}

/* Copy constructor */
HttpRequest::HttpRequest(const HttpRequest &original)
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

	this->_method = original.getMethod();
	this->_target = original.getTarget();
	this->_version = original.getVersion();
	return (*this);
}

/* ************************************************************************** */
/* Getters */

std::string	HttpRequest::getMethod(void) const
{
	return (this->_method);
}

std::string	HttpRequest::getTarget(void) const
{
	return (this->_target);
}

std::string	HttpRequest::getVersion(void) const
{
	return (this->_version);
}


/* ************************************************************************** */
/* Exceptions */

/* *********************************** */
/* BadRequest                          */
/* *********************************** */

/* Message constructor */
HttpRequest::BadRequest::BadRequest(const std::string &msg)
	: std::runtime_error(msg) {}

/* Copy constructor */
HttpRequest::BadRequest::BadRequest(const BadRequest &original)
	: std::runtime_error("")
{
	*this = original;
}


/* Destructor */
HttpRequest::BadRequest::~BadRequest(void) throw() {}


/* *********************************** */
/* Operator overloads */

/* Copy assignment operator */
HttpRequest::BadRequest	&HttpRequest::BadRequest::operator=(const BadRequest
		&original)
{
	if (this == &original)
		return (*this);

	this->std::runtime_error::operator=(original);
	return (*this);
}


/* ************************************************************************** */

} // namespace webserv