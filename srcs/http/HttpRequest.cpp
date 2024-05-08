/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 23:01:55 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/08 05:15:28 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

namespace	http
{

/* ************************************************************************** */
/* Constructors */

/* Default constructor */
HttpRequest::HttpRequest(void)
	: HttpMessage(), _method(), _target()
{
	this->setValidity(true);
}

/* Request constructor */
// HttpRequest::HttpRequest(const std::string &request)
// 	: HttpMessage(), _method(), _target()
// {
// 	std::istringstream	iss_request(request);

// 	this->setValidity(true);
// 	this->_parseRequestLine(iss_request);
// 	this->_parseHeaders(iss_request);
// 	this->setBody(iss_request.str());
// }

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
	this->_target = original.getTarget();
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


/* ************************************************************************** */
/* Private methods */

/* Method to parse amd store the request line */
void	HttpRequest::parseRequestLine(std::string &line)
{
	if (std::count(line.begin(), line.end(), ' ') != 2)
		throw InvalidRequest();

	this->setStatusLine(line);

	std::istringstream	lineStream(line);
	std::string			version;

	lineStream >> this->_method >> this->_target >> version;
	if (this->_method.empty() || this->_target.empty() || version.empty())
		throw InvalidRequest();

	this->setVersion(version);
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
				throw InvalidRequest();

			header_name = line.substr(0, colon_pos);
			header_value = tool::strings::trim(line.substr(colon_pos + 1),
				" \t\n\r");
			this->addHeader(header_name, header_value);
		}
	}

	if ((this->_method == "POST" && this->getHeader("Content-Length").empty())
			|| this->getHeader("Host").empty())
		throw InvalidRequest();
}


/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */

/* ***************************** */
/* InvalidRequest                */
/* ***************************** */

/* Default constructor */
HttpRequest::InvalidRequest::InvalidRequest(void)
	: RuntimeError("Invalid request", 22) {}

/* Copy constructor */
HttpRequest::InvalidRequest::InvalidRequest(const InvalidRequest &original)
	: RuntimeError(original) {}


/* Destructor */
HttpRequest::InvalidRequest::~InvalidRequest(void) throw() {}


/* ***************************** */
/* Operator overloads */

HttpRequest::InvalidRequest	&HttpRequest::InvalidRequest::operator=(
	const InvalidRequest &original)
{
	if (this == &original)
		return (*this);
	RuntimeError::operator=(original);
	return (*this);
}

} // namespace http