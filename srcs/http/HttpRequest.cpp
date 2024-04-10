/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 23:01:55 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/09 19:35:05 by hnogared         ###   ########.fr       */
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
	std::istringstream	iss_request(request);

	this->_parseRequestLine(iss_request);
	this->_parseHeaders(iss_request);
	this->setBody(iss_request.str());
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
void	HttpRequest::_parseRequestLine(std::istringstream &iss_request)
{
	std::string	line;

	std::getline(iss_request, line);

	if (line.size() < 3 || line[line.size() - 1] != '\r'
			|| std::count(line.begin(), line.end(), ' ') != 2)
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

/* Method to parse and store the headers */
void	HttpRequest::_parseHeaders(std::istringstream &iss_request)
{
	std::string	line;

	while (std::getline(iss_request, line))
	{
		if (line.size() == 1 && line[0] == '\r')
			break;

		if (line.size() < 2 || line[line.size() - 1] != '\r')
			this->_is_valid = false;

		if (line[line.size() - 1] == '\r')
			line = line.substr(0, line.size() - 1);

		{
			std::string::size_type	colon_pos = line.find(':');
			std::string				header_name;
			std::string				header_value;

			if (colon_pos == std::string::npos)
			{
				this->_is_valid = false;
				header_name = line;
				header_value = "";
			}
			else
			{
				header_name = line.substr(0, colon_pos);
				header_value = utils::trim(line.substr(colon_pos + 1));
			}
			this->addHeader(header_name, header_value);
		}
	}
}

} // namespace http