/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 23:01:55 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/08 19:56:12 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

namespace	http
{

/* ************************************************************************** */
/* Constructors */

/* Default constructor */
HttpRequest::HttpRequest(void)
	: HttpMessage(), _is_valid(false), _method(), _target() {}

/* Request constructor */
HttpRequest::HttpRequest(const std::string &request)
	: HttpMessage(), _is_valid(true), _method(), _target()
{
	this->_parseRequestLine(request);
}

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
	this->_is_valid = original.isValid();
	this->_method = original.getMethod();
	this->_target = original.getTarget();
	return (*this);
}

/* ************************************************************************** */
/* Getters */

bool	HttpRequest::isValid(void) const
{
	return (this->_is_valid);
}

std::string	HttpRequest::getMethod(void) const
{
	return (this->_method);
}

std::string	HttpRequest::getTarget(void) const
{
	return (this->_target);
}


/* ************************************************************************** */
/* Private methods */

/* Method to parse amd store the request line */
void	HttpRequest::_parseRequestLine(const std::string &request)
{
	std::istringstream	iss_request(request);
	std::string			line;

	std::getline(iss_request, line);

	if (line.size() < 3 || std::count(line.begin(), line.end(), ' ') != 2
			|| line[line.size() - 1] != '\r')
		this->_is_valid = false;

	if (line[line.size() - 1] == '\r')
		line = line.substr(0, line.size() - 1);

	this->setStatusLine(line);

	std::istringstream	line_stream(line);
	std::string			version;

	line_stream >> this->_method >> this->_target >> version;
	this->setVersion(version);

	if (this->_method.empty() || this->_target.empty())
		this->_is_valid = false;
}

} // namespace http