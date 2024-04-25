/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 09:35:37 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/25 14:29:14 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#include "ConfigurationParser.hpp"

namespace	webserv
{

/* ************************************************************************** */

/* Default constructor */
Configuration::Configuration(void) {}

/* Path constructor */
Configuration::Configuration(const std::string &path)
{
	(void)path;
	//ConfigurationParser::parse(path);
}

/* Copy constructor */
Configuration::Configuration(const Configuration &original)
{
	*this = original;
}


/* Destructor */
Configuration::~Configuration(void) {}


/* ************************************************************************** */
/* Operator overloads */

Configuration	&Configuration::operator=(const Configuration &original)
{
	if (this == &original)
		return (*this);
	return (*this);
}


/* ************************************************************************** */
/* Getters */

int	Configuration::getPort(void) const
{
	return (this->_port);
}

std::string	Configuration::getServerName(void) const
{
	return (this->_serverName);
}


/* ************************************************************************** */
/* Setters */

void	Configuration::setPort(int port)
{
	this->_port = port;
}

void	Configuration::setServerName(const std::string &serverName)
{
	this->_serverName = serverName;
}

} // namespace webserv