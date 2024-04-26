/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 09:35:37 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/26 12:42:59 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#include "ConfigurationParser.hpp"

namespace	webserv
{

/* ************************************************************************** */

/* Default constructor */
Configuration::Configuration(void)
{
	this->_address.sin_family = AF_INET;
	this->_address.sin_port = htons(8080);
	this->_address.sin_addr.s_addr = htonl(INADDR_ANY);
}

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

sockaddr_in	Configuration::getAddress(void) const
{
	return (this->_address);
}

std::string	Configuration::getAddressString(void) const
{
	return (tool::net::inet_ntoa(this->_address.sin_addr));
}

int	Configuration::getPort(void) const
{
	return (ntohs(this->_address.sin_port));
}

std::vector<std::string>	Configuration::getServerNames(void) const
{
	return (this->_serverNames);
}


/* ************************************************************************** */
/* Setters */

int	Configuration::setAddress(const std::string &address)
{
	return (tool::net::inet_aton(address, this->_address.sin_addr));
}

void	Configuration::setAddress(const sockaddr_in &address)
{
	this->_address = address;
}

void	Configuration::setPort(int port)
{
	this->_address.sin_port = htons(port);
}

void	Configuration::addServerName(const std::string &serverName)
{
	this->_serverNames.push_back(serverName);
}


/* ************************************************************************** */
/* External operator overloads                                                */
/* ************************************************************************** */

std::ostream	&operator<<(std::ostream &os, const Configuration &config)
{
	os << "Addr: " << config.getAddressString()
		<< "\nPort: " << config.getPort() << std::endl;
	return (os);
}

} // namespace webserv