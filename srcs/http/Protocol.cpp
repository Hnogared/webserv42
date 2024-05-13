/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Protocol.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 18:55:44 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/13 12:22:28 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Protocol.hpp"

namespace	http
{

/* ************************************************************************** */

/* Default constructor */
Protocol::Protocol(void) : _name("HTTP")
{
	this->_version[0] = 1;
	this->_version[1] = 0;
}

/* String constructor */
Protocol::Protocol(const std::string &str)
{
	size_t	pos;
	char	sep;

	pos = str.find('/');
	if (pos == std::string::npos || pos == 0 || pos == str.length() - 1)
		throw std::invalid_argument(str + ": Invalid protocol string");
	
	this->_name = str.substr(0, pos);

	{
		std::istringstream	iss(str.substr(pos + 1));

		iss >> this->_version[0] >> sep >> this->_version[1];
		if (iss.fail() || !iss.eof() || sep != '.')
			throw std::invalid_argument(str + ": Invalid protocol string");
	}
}

/* Copy constructor */
Protocol::Protocol(const Protocol &original)
{
	*this = original;
}


/* Destructor */
Protocol::~Protocol(void) {}


/* ************************************************************************** */
/* Operator overloads */

Protocol	&Protocol::operator=(const Protocol &original)
{
	const int	*version;

	if (this == &original)
		return (*this);

	version = original.getVersion();
	this->_name = original.getName();
	this->_version[0] = version[0];
	this->_version[1] = version[1];
	return (*this);
}

bool	Protocol::operator==(const Protocol &other) const
{
	const int	*otherVersion = other.getVersion();

	return (this->_name == other.getName()
		&& this->_version[0] == otherVersion[0]
		&& this->_version[1] == otherVersion[1]);
}

bool	Protocol::operator!=(const Protocol &other) const
{
	return (!(*this == other));
}

bool	Protocol::operator<(const Protocol &other) const
{
	const int	*otherVersion = other.getVersion();

	if (this->_name != other.getName())
		return (this->_name < other.getName());
	if (this->_version[0] != otherVersion[0])
		return (this->_version[0] < otherVersion[0]);
	return (this->_version[1] < otherVersion[1]);
}

bool	Protocol::operator>(const Protocol &other) const
{
	return (other < *this);
}

bool	Protocol::operator<=(const Protocol &other) const
{
	return (!(*this > other));
}

bool	Protocol::operator>=(const Protocol &other) const
{
	return (!(*this < other));
}


/* ************************************************************************** */
/* Getters */

const std::string	&Protocol::getName(void) const
{
	return (this->_name);
}

const int	*Protocol::getVersion(void) const
{
	return (this->_version);
}


/* ************************************************************************** */
/* External operator overloads                                                */
/* ************************************************************************** */

std::ostream	&operator<<(std::ostream &os, const Protocol &protocol)
{
	os << protocol.getName() << '/' << protocol.getVersion()[0]
		<< '.' << protocol.getVersion()[1];
	return (os);
}

} // namespace http