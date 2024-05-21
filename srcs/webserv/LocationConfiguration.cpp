/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfiguration.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 14:41:57 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/21 21:13:18 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfiguration.hpp"

namespace webserv
{

/* ************************************************************************** */

/* Path constructor */
LocationConfiguration::LocationConfiguration(const std::string &path)
    : _matchType(LocationConfiguration::MATCH_PREFIX),
      _path(path),
      _root(""),
      _index(""),
      _autoindex(false),
      _clientMaxBodySize(-1),
      _returnCode(0),
      _returnMessage(""),
      _allowedMethods(),
      _fcgiServer(""),
      _fcgiParams()
{
    this->_allowedMethods.insert(http::HttpRequest::GET);
    this->_allowedMethods.insert(http::HttpRequest::HEAD);
}

/* Copy constructor */
LocationConfiguration::LocationConfiguration(
    const LocationConfiguration &original)
{
    *this = original;
}

/* Destructor */
LocationConfiguration::~LocationConfiguration(void) {}

/* ************************************************************************** */
/* Operator overloads */

LocationConfiguration &LocationConfiguration::operator=(
    const LocationConfiguration &original)
{
    if (this == &original) return (*this);
    this->_matchType = original.getMatchType();
    this->_path = original.getPath();
    this->_root = original.getRoot();
    this->_index = original.getIndex();
    this->_autoindex = original.isAutoindex();
    this->_clientMaxBodySize = original.getClientMaxBodySize();
    this->_returnCode = original.getReturnCode();
    this->_returnMessage = original.getReturnMessage();
    this->_allowedMethods = original.getAllowedMethods();
    this->_fcgiServer = original.getFCGIServer();
    this->_fcgiParams = original.getFCGIParams();
    return (*this);
}

bool LocationConfiguration::operator<(const LocationConfiguration &other) const
{
    if (this->_matchType != other.getMatchType())
        return (this->_matchType < other.getMatchType());
    if (this->_path != other.getPath()) return (this->_path < other.getPath());
    if (this->_root != other.getRoot()) return (this->_root < other.getRoot());
    if (this->_index != other.getIndex())
        return (this->_index < other.getIndex());
    if (this->_autoindex != other.isAutoindex())
        return (this->_autoindex < other.isAutoindex());
    if (this->_clientMaxBodySize != other.getClientMaxBodySize())
        return (this->_clientMaxBodySize < other.getClientMaxBodySize());
    if (this->_returnCode != other.getReturnCode())
        return (this->_returnCode < other.getReturnCode());
    if (this->_returnMessage != other.getReturnMessage())
        return (this->_returnMessage < other.getReturnMessage());
    if (this->_allowedMethods != other.getAllowedMethods())
        return (this->_allowedMethods < other.getAllowedMethods());
    if (this->_fcgiServer != other.getFCGIServer())
        return (this->_fcgiServer < other.getFCGIServer());
    return (this->_fcgiParams < other.getFCGIParams());
}

/* ************************************************************************** */
/* Getters */

LocationConfiguration::e_matchType LocationConfiguration::getMatchType(
    void) const
{
    return (this->_matchType);
}

const std::string &LocationConfiguration::getPath(void) const
{
    return (this->_path);
}

const std::string &LocationConfiguration::getRoot(void) const
{
    return (this->_root);
}

const std::string &LocationConfiguration::getIndex(void) const
{
    return (this->_index);
}

bool LocationConfiguration::isAutoindex(void) const
{
    return (this->_autoindex);
}

long int LocationConfiguration::getClientMaxBodySize(void) const
{
    return (this->_clientMaxBodySize);
}

int LocationConfiguration::getReturnCode(void) const
{
    return (this->_returnCode);
}

const std::string &LocationConfiguration::getReturnMessage(void) const
{
    return (this->_returnMessage);
}

const std::set<http::HttpRequest::e_method>
    &LocationConfiguration::getAllowedMethods(void) const
{
    return (this->_allowedMethods);
}

const std::string &LocationConfiguration::getFCGIServer(void) const
{
    return (this->_fcgiServer);
}

const std::map<std::string, std::string> &LocationConfiguration::getFCGIParams(
    void) const
{
    return (this->_fcgiParams);
}

/* ************************************************************************** */
/* Setters */

void LocationConfiguration::setMatchType(const std::string &matchType)
{
    if (matchType.empty()) throw std::invalid_argument("Empty match type");

    if (matchType.size() == 1)
    {
        switch (matchType[0])
        {
            case '=':
                this->_matchType = LocationConfiguration::MATCH_EXACT;
                return;
            case '>':
                this->_matchType = LocationConfiguration::MATCH_PREFIX;
                return;
            case '<':
                this->_matchType = LocationConfiguration::MATCH_SUFFIX;
                return;
        }
    }

    throw std::invalid_argument("Invalid match type `" + matchType + "`");
}

void LocationConfiguration::setMatchType(e_matchType matchType)
{
    this->_matchType = matchType;
}

void LocationConfiguration::setPath(const std::string &path)
{
    this->_path = path;
}

void LocationConfiguration::setRoot(const std::string &root)
{
    if (!tool::strings::isValidPath(root))
        throw InvalidPath("`" + root + "`: " + strerror(errno));
    this->_root = root;
}

void LocationConfiguration::setIndex(const std::string &index)
{
    this->_index = index;
}

void LocationConfiguration::setAutoindex(bool autoindexState)
{
    this->_autoindex = autoindexState;
}

void LocationConfiguration::setClientMaxBodySize(long int size)
{
    this->_clientMaxBodySize = size;
}

void LocationConfiguration::setReturnCode(int returnCode)
{
    if (returnCode < 100 || returnCode > 599)
    {
        throw std::invalid_argument("Invalid return code `" +
                                    tool::strings::toStr(returnCode) + "`");
    }
    this->_returnCode = returnCode;
}

void LocationConfiguration::setReturnMessage(const std::string &returnMessage)
{
    this->_returnMessage = returnMessage;
}

void LocationConfiguration::setAllowedMethods(
    const std::set<http::HttpRequest::e_method> &allowedMethods)
{
    this->_allowedMethods = allowedMethods;
}

void LocationConfiguration::addAllowedMethod(http::HttpRequest::e_method method)
{
    this->_allowedMethods.insert(method);
}

void LocationConfiguration::setFCGIServer(const std::string &fcgiServer)
{
    this->_fcgiServer = fcgiServer;
}

void LocationConfiguration::setFCGIParams(
    const std::map<std::string, std::string> &params)
{
    this->_fcgiParams = params;
}

void LocationConfiguration::addFCGIParam(const std::string &key,
                                         const std::string &value)
{
    this->_fcgiParams[key] = value;
}

/* ************************************************************************** */
/* Public methods */

bool LocationConfiguration::ismethodAllowed(
    http::HttpRequest::e_method method) const
{
    return (this->_allowedMethods.find(method) != this->_allowedMethods.end());
}

std::ostream &LocationConfiguration::print(std::ostream &os) const
{
    os << "@ " << this->_path << "\nMatch type      : " << this->_matchType;

    if (!this->_root.empty()) os << "\nRoot            : " << this->_root;

    os << "\nAutoindex       : " << (this->isAutoindex() ? "on" : "off");
    os << "\nIndex           : " << this->_index << "\nAllowed methods : ";

    if (this->_allowedMethods.empty())
        os << "all";
    else
        os << tool::strings::join(this->_allowedMethods, ", ");

    if (this->_clientMaxBodySize != -1)
        os << "\nC max body size : " << this->_clientMaxBodySize;

    if (this->_returnCode != 0)
    {
        os << "\nReturn          : [" << this->getReturnCode() << "] "
           << this->getReturnMessage();
    }

    if (!this->_fcgiServer.empty())
        os << "\n\nFastCGI server  : " << this->_fcgiServer;

    if (!this->_fcgiParams.empty())
    {
        std::map<std::string, std::string>::const_iterator it;

        os << "\nFastCGI params";
        for (it = this->_fcgiParams.begin(); it != this->_fcgiParams.end();
             it++)
            os << "\n └ " << it->first << " = " << it->second;
    }
    return (os);
}

/* ************************************************************************** */
/* External operator overloads                                                */
/* ************************************************************************** */

std::ostream &operator<<(std::ostream &os,
                         const LocationConfiguration &location)
{
    return (location.print(os));
}

}  // namespace webserv
