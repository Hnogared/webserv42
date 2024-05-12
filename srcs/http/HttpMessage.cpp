/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 18:25:00 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/11 17:40:02 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpMessage.hpp"

namespace	http
{

/* ************************************************************************** */

/* Default constructor */
HttpMessage::HttpMessage(void) {}

/* Version + message constructor */
HttpMessage::HttpMessage(const std::string &statusLine,
		const std::string &version)
	: _version(version), _statusLine(statusLine) {}

/* Copy constructor */
HttpMessage::HttpMessage(const HttpMessage &original)
{
	*this = original;
}


/* Destructor */
HttpMessage::~HttpMessage(void) {}


/* ************************************************************************** */
/* Operator overloads */

/* Copy assignment operator */
HttpMessage	&HttpMessage::operator=(const HttpMessage &original)
{
	if (this == &original)
		return (*this);

	this->_version = original.getVersion();
	this->_statusLine = original.getStatusLine();
	this->_headers = original.getHeaders();
	this->_body = original.getBody();
	return (*this);
}


/* ************************************************************************** */
/* Getters */

std::string	HttpMessage::getVersion(void) const
{
	return (this->_version);
}

std::string	HttpMessage::getStatusLine(void) const
{
	return (this->_statusLine);
}

std::string	HttpMessage::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator	it;

	it = this->_headers.find(key);
	if (it == this->_headers.end())
		return ("");
	return (it->second);
}

std::map<std::string, std::string>	HttpMessage::getHeaders(void) const
{
	return (this->_headers);
}

std::string	HttpMessage::getBody(void) const
{
	return (this->_body);
}


/* ************************************************************************** */
/* Setters */

void	HttpMessage::setVersion(const std::string &version)
{
	this->_version = version;
}

void	HttpMessage::setStatusLine(const std::string &statusLine)
{
	this->_statusLine = statusLine;
}

void	HttpMessage::addHeader(const std::string &key, const std::string &val)
{
	if (this->_headers.find(key) != this->_headers.end())
		this->_headers[key] += "," + val;
	else
		this->_headers[key] = val;
}

void	HttpMessage::setHeader(const std::string &key, const std::string &val)
{
	this->_headers[key] = val;
}


/* Virtual setters */

void	HttpMessage::setBody(const std::string &body)
{
	this->_body = body;
}


/* ************************************************************************** */
/* Public methods */

void	HttpMessage::clear(void)
{
	this->_version.clear();
	this->_statusLine.clear();
	this->_headers.clear();
	this->_body.clear();
}


/* ************************************************************************** */
/* External operator overloads                                                */
/* ************************************************************************** */

std::ostream	&operator<<(std::ostream &os, const HttpMessage &http_msg)
{
	std::map<std::string, std::string>	headers = http_msg.getHeaders();
	std::map<std::string, std::string>::const_iterator	it;

	os << http_msg.getStatusLine() << std::endl;
	for (it = headers.begin(); it != headers.end(); ++it)
		os << it->first << ": " << it->second << std::endl;
	os << std::endl;
	os << http_msg.getBody();
	return (os);
}

} // namespace http