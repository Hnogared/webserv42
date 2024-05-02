/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfiguration.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 14:41:57 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/02 13:15:01 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfiguration.hpp"

namespace webserv
{

/* ************************************************************************** */

/* Path constructor */
LocationConfiguration::LocationConfiguration(const std::string &path)
	: _path(path),
	_root(""),
	_index(WS_DFL_INDEX),
	_allowedMethods(),
	_autoindex(false),
	_returnCode(0),
	_returnMessage("")
{}

/* Copy constructor */
LocationConfiguration::LocationConfiguration(const LocationConfiguration
	&original)
{
	*this = original;
}


/* Destructor */
LocationConfiguration::~LocationConfiguration(void) {}


/* ************************************************************************** */
/* Operator overloads */

LocationConfiguration	&LocationConfiguration::operator=(
	const LocationConfiguration &original)
{
	if (this == &original)
		return (*this);
	this->_path = original.getPath();
	this->_root = original.getRoot();
	this->_index = original.getIndex();
	this->_allowedMethods = original.getAllowedMethods();
	this->_autoindex = original.isAutoindex();
	this->_returnCode = original.getReturnCode();
	this->_returnMessage = original.getReturnMessage();
	return (*this);
}

bool	LocationConfiguration::operator<(const LocationConfiguration &other)
	const
{
	if (this->_path != other.getPath())
		return (this->_path < other.getPath());
	if (this->_root != other.getRoot())
		return (this->_root < other.getRoot());
	if (this->_index != other.getIndex())
		return (this->_index < other.getIndex());
	if (this->_allowedMethods != other.getAllowedMethods())
		return (this->_allowedMethods < other.getAllowedMethods());
	if (this->_autoindex != other.isAutoindex())
		return (this->_autoindex < other.isAutoindex());
	if (this->_returnCode != other.getReturnCode())
		return (this->_returnCode < other.getReturnCode());
	return (this->_returnMessage < other.getReturnMessage());
}


/* ************************************************************************** */
/* Getters */

std::string	LocationConfiguration::getPath(void) const
{
	return (this->_path);
}

std::string	LocationConfiguration::getRoot(void) const
{
	return (this->_root);
}

std::string	LocationConfiguration::getIndex(void) const
{
	return (this->_index);
}

const std::set<std::string>	&LocationConfiguration::getAllowedMethods(void)
	const
{
	return (this->_allowedMethods);
}

bool	LocationConfiguration::isAutoindex(void) const
{
	return (this->_autoindex);
}

int	LocationConfiguration::getReturnCode(void) const
{
	return (this->_returnCode);
}

std::string	LocationConfiguration::getReturnMessage(void) const
{
	return (this->_returnMessage);
}


/* ************************************************************************** */
/* Setters */

void	LocationConfiguration::setPath(const std::string &path)
{
	this->_path = path;
}

void	LocationConfiguration::setRoot(const std::string &root)
{
	if (!tool::strings::isValidPath(root))
		throw InvalidPath("`" + root + "`: " + strerror(errno));
	this->_root = root;
}

void	LocationConfiguration::setIndex(const std::string &index)
{
	this->_index = index;
}

void	LocationConfiguration::setAllowedMethods(
	const std::set<std::string> &allowedMethods)
{
	this->_allowedMethods = allowedMethods;
}

void	LocationConfiguration::addAllowedMethod(const std::string &method)
{
	this->_allowedMethods.insert(method);
}

void	LocationConfiguration::setAutoindex(bool autoindexState)
{
	this->_autoindex = autoindexState;
}

void	LocationConfiguration::setReturnCode(int returnCode)
{
	this->_returnCode = returnCode;
}

void	LocationConfiguration::setReturnMessage(const std::string
	&returnMessage)
{
	this->_returnMessage = returnMessage;
}


/* ************************************************************************** */
/* Public methods */

bool	LocationConfiguration::isMethodAllowed(const std::string &method) const
{
	return (this->_allowedMethods.empty()
		|| this->_allowedMethods.find(method) != this->_allowedMethods.end());
}

std::ostream	&LocationConfiguration::print(std::ostream &os) const
{
	os << "'" << this->_path << "'";

	if (!this->_root.empty())
		os << "\nRoot  : '" << this->_root << "'";
	
	os << "\nIndex : '" << this->_index << "'"
		<< "\nAllowed methods : ";

	if (this->_allowedMethods.empty())
		os << "all";
	else
		os << tool::strings::join(this->_allowedMethods, ", ");

	os << "\nAutoindex : " << (this->isAutoindex() ? "on" : "off");

	if (this->_returnCode != 0)
	{
		os << "\nReturn : [" << this->getReturnCode()
			<< "] '" << this->getReturnMessage() << "'";
	}
	return (os);
}


/* ************************************************************************** */
/* External operator overloads                                                */
/* ************************************************************************** */

std::ostream	&operator<<(std::ostream &os, const LocationConfiguration
	&location)
{
	return (location.print(os));
}

} // namespace webserv

