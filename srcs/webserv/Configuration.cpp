/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   Configuration.cpp								  :+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: hnogared <hnogared@student.42.fr>		  +#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2024/04/24 09:35:37 by hnogared		  #+#	#+#			 */
/*   Updated: 2024/05/11 18:40:49 by hnogared		 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#include "Configuration.hpp"
#include "ConfigurationParser.hpp"

namespace	webserv
{

/* ************************************************************************** */

/* Default constructor */
Configuration::Configuration(void)
	: _protocol(WS_HTTP_VERSION), _backlog(WS_DFL_BACKLOG), _root(""),
	_index(WS_DFL_INDEX), _clientMaxBodySize(WS_DFL_C_MAX_BODY)
{
	this->_address.sin_family = AF_INET;
	this->_address.sin_port = htons(WS_DFL_PORT);
	this->_address.sin_addr.s_addr = htonl(INADDR_ANY);
}

/* Copy constructor */
Configuration::Configuration(const Configuration &original)
	: _protocol(original.getProtocol())
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
	
	if (this->_protocol != original.getProtocol())
		this->_protocol = original.getProtocol();
	this->_address = original.getConstAddress();
	this->_backlog = original.getBacklog();
	this->_serverNames = original.getServerNames();
	this->_root = original.getRoot();
	this->_index = original.getIndex();
	this->_errorRedirects = original.getErrorRedirects();
	this->_clientMaxBodySize = original.getClientMaxBodySize();
	this->_locations = original.getLocations();
	return (*this);
}


/* ************************************************************************** */
/* Getters */

const http::Protocol	&Configuration::getProtocol(void) const
{
	return (this->_protocol);
}

sockaddr_in	&Configuration::getAddress(void)
{
	return (this->_address);
}

const sockaddr_in	&Configuration::getConstAddress(void) const
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

unsigned int	Configuration::getBacklog(void) const
{
	return (this->_backlog);
}

const std::set<std::string>	&Configuration::getServerNames(void) const
{
	return (this->_serverNames);
}

std::string	Configuration::getRoot(void) const
{
	return (this->_root);
}

std::string	Configuration::getIndex(void) const
{
	return (this->_index);
}

const std::map<int,std::string>	&Configuration::getErrorRedirects(void) const
{
	return (this->_errorRedirects);
}

unsigned long int	Configuration::getClientMaxBodySize(void) const
{
	return (this->_clientMaxBodySize);
}

const std::set<LocationConfiguration>	&Configuration::getLocations(void) const
{
	return (this->_locations);
}


/* ************************************************************************** */
/* Setters */

void	Configuration::setProtocol(const http::Protocol &protocol)
{
	this->_protocol = protocol;
}

void	Configuration::setAddress(const std::string &address)
{
	if (!tool::net::inet_aton(address, this->_address.sin_addr))
		throw std::invalid_argument("Invalid adress `" + address + "`");
}

void	Configuration::setAddress(const sockaddr_in &address)
{
	this->_address = address;
}

void	Configuration::setPort(int port)
{
	this->_address.sin_port = htons(port);
}

void	Configuration::setBacklog(unsigned int backlog)
{
	this->_backlog = backlog;
}

void	Configuration::setServerNames(const std::set<std::string> &serverNames)
{
	this->_serverNames = serverNames;
}

void	Configuration::addServerName(const std::string &serverName)
{
	this->_serverNames.insert(serverName);
}

void	Configuration::setRoot(const std::string &root)
{
	if (!tool::strings::isValidPath(root))
		throw InvalidPath("`" + root + "`: " + strerror(errno));
	this->_root = root;
}

void	Configuration::setIndex(const std::string &index)
{
	this->_index = index;
}

void	Configuration::addErrorRedirect(int error, const std::string &redirect)
{
	if (error < 300 || error > 599)
	{
		throw std::invalid_argument("Invalid error code `"
			+ tool::strings::toStr(error) + "`");
	}
	this->_errorRedirects[error] = redirect;
}

void	Configuration::setClientMaxBodySize(unsigned long int size)
{
	this->_clientMaxBodySize = size;
}

void	Configuration::addLocation(const LocationConfiguration &location)
{
	this->_locations.insert(location);
}


/* ************************************************************************** */
/* Public methods */

const LocationConfiguration* Configuration::findBestLocation(
	const std::string &uri) const
{
	const LocationConfiguration*					bestLocation = NULL;
	std::set<LocationConfiguration>::const_iterator it;

	for (it = this->_locations.begin(); it != this->_locations.end(); it++)
	{
		std::string path = it->getPath();

		if (uri.substr(0, path.size()) != path)
			continue ;

		if (!bestLocation || path.size() > bestLocation->getPath().size())
			bestLocation = &(*it);
	}

	return (bestLocation);
}

std::ostream	&Configuration::print(std::ostream &os) const
{
	os << "=================================\n"
		<< "\nProtocol : " << this->_protocol
		<< "\nAddr     : " << this->getAddressString()
		<< "\nPort     : " << this->getPort()
		<< "\nBacklog  : " << this->getBacklog();
	
	if (!this->_serverNames.empty())
		os << "\nNames    : " << tool::strings::join(this->_serverNames, ", ");

	if (!this->_root.empty())
		os << "\nRoot     : '" << this->_root << "'";
	
	os << "\nIndex    : '" << this->_index << "'"
		<< "\nClient max body size : " << this->_clientMaxBodySize;

	if (!this->_errorRedirects.empty())
	{
		std::map<int, std::string>::const_iterator	it;

		os << "\n\nError redirects";
		for (it = this->_errorRedirects.begin();
				it != this->_errorRedirects.end(); it++)
			os << "\n └ " << it->first << " -> " << it->second;
	}


	if (!this->_locations.empty())
	{
		std::set<LocationConfiguration>::const_iterator	it;

		os << "\n  -----------------------------\nLocations:";
		for (it = this->_locations.begin(); it != this->_locations.end(); it++)
			os << "\n\n" << *it;
	}

	os << "\n=================================";
	return (os);
}


/* ************************************************************************** */
/* External operator overloads												*/
/* ************************************************************************** */

std::ostream	&operator<<(std::ostream &os, const Configuration &config)
{
	return (config.print(os));
}

} // namespace webserv