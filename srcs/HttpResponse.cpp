/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 23:28:04 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/08 19:15:20 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

namespace	http
{

/* Constructors */

/* Version + status code + reason constructor */
HttpResponse::HttpResponse(int status_code, const std::string &status_line,
		const std::string &version)
	: HttpMessage(status_line, version), _status_code(status_code) {}

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
	this->_status_code = original.getStatusCode();
	return (*this);
}


/* ************************************************************************** */
/* Getters */

int	HttpResponse::getStatusCode(void) const
{
	return (this->_status_code);
}


/* ************************************************************************** */
/* Public methods */

std::string	HttpResponse::toString(void) const
{
	std::ostringstream							result;
	const std::map<std::string, std::string>	&headers = this->getHeaders();
	std::map<std::string, std::string>::const_iterator	it;

	result << this->getVersion() << " "
		<< this->_status_code << " "
		<< this->getStatusLine() << "\r\n";

	for (it = headers.begin(); it != headers.end(); it++)
		result << it->first << ": " << it->second << "\r\n";
	
	result << "\r\n"
		<< this->getBody();
	return (result.str());
}

} // namespace http