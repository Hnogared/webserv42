/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 23:28:04 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 23:50:57 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

namespace	webserv
{

/* Constructors */

/* Version + status code + reason constructor */
HttpResponse::HttpResponse(const std::string &version, int status_code,
		const std::string &reason)
	: _version(version), _status_code(status_code), _reason(reason) {}

/* Copy constructor */
HttpResponse::HttpResponse(const HttpResponse &original)
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
	this->_version = original.getVersion();
	this->_status_code = original.getStatusCode();
	this->_reason = original.getReason();
	this->_headers = original.getHeaders();
	this->_body = original.getBody();
	return (*this);
}


/* ************************************************************************** */
/* Getters */

std::string	HttpResponse::getVersion(void) const
{
	return (this->_version);
}

int			HttpResponse::getStatusCode(void) const
{
	return (this->_status_code);
}

std::string	HttpResponse::getReason(void) const
{
	return (this->_reason);
}

std::string	HttpResponse::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator	it;

	it = this->_headers.find(key);
	if (it == this->_headers.end())
		return ("");
	return (it->second);
}

std::map<std::string, std::string>	HttpResponse::getHeaders(void) const
{
	return (this->_headers);
}

std::string	HttpResponse::getBody(void) const
{
	return (this->_body);
}


/* ************************************************************************** */
/* Public methods */

void	HttpResponse::addHeader(const std::string &key, const std::string &val)
{
	this->_headers[key] = val;
}

void	HttpResponse::setBody(const std::string &body)
{
	this->_body = body;
}

std::string	HttpResponse::toString(void) const
{
	std::ostringstream									result;
	std::map<std::string, std::string>::const_iterator	it;

	result << this->_version << " "
		<< this->_status_code << " "
		<< this->_reason << "\r\n";

	for (it = this->_headers.begin(); it != this->_headers.end(); it++)
		result << it->first << ": " << it->second << "\r\n";
	
	result << "\r\n"
		<< this->_body;
	return (result.str());
}

} // namespace webserv