/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 18:25:00 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/19 17:25:34 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpMessage.hpp"

namespace	http
{

/* ************************************************************************** */
/* Constructors */

/* Default constructor */
HttpMessage::HttpMessage(void) {}

/* Version + message constructor */
HttpMessage::HttpMessage(const std::string &status_line,
		const std::string &version)
	: _version(version), _status_line(status_line) {}

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
	this->_status_line = original.getStatusLine();
	this->_headers = original.getHeaders();
	this->_body = original.getBody();
	this->_is_valid = original.isValid();
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
	return (this->_status_line);
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

bool	HttpMessage::isValid(void) const
{
	return (this->_is_valid);
}


/* ************************************************************************** */
/* Setters */

void	HttpMessage::setVersion(const std::string &version)
{
	this->_version = version;
}

void	HttpMessage::setStatusLine(const std::string &status_line)
{
	this->_status_line = status_line;
}

void	HttpMessage::addHeader(const std::string &key, const std::string &val)
{
	if (this->_headers.find(key) != this->_headers.end())
		this->_headers[key] += "," + val;
	else
		this->_headers[key] = val;
}

void	HttpMessage::setBody(const std::string &body)
{
	this->_body = body;
}

void	HttpMessage::setValidity(bool is_valid)
{
	this->_is_valid = is_valid;
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