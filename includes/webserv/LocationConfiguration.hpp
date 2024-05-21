/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfiguration.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 14:34:11 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/21 22:09:33 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIGURATION_HPP
#define LOCATIONCONFIGURATION_HPP

#include <cerrno>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <string>

#include "HttpRequest.hpp"
#include "exceptions.hpp"
#include "strings.hpp"
#include "webserv.hpp"

namespace webserv
{

class LocationConfiguration
{
public:
    enum e_locationType
    {
        STATIC = 0,
        DYNAMIC,
        REDIRECT,
    };

    enum e_matchType
    {
        MATCH_PREFIX = 0,
        MATCH_SUFFIX,
        MATCH_EXACT,
    };

    /* Constructors */
    explicit LocationConfiguration(const std::string &path);
    LocationConfiguration(const LocationConfiguration &original);

    /* Destructor */
    ~LocationConfiguration(void);

    /* Operator overloads */
    LocationConfiguration &operator=(const LocationConfiguration &original);
    bool operator<(const LocationConfiguration &other) const;

    /* Getters */
    e_locationType getLocationType(void) const;
    e_matchType getMatchType(void) const;
    const std::string &getPath(void) const;
    const std::string &getRoot(void) const;
    const std::string &getIndex(void) const;
    bool isAutoindex(void) const;
    long int getClientMaxBodySize(void) const;
    int getReturnCode(void) const;
    const std::string &getReturnMessage(void) const;
    const std::set<http::HttpRequest::e_method> &getAllowedMethods(void) const;
    const std::string &getFCGIServer(void) const;
    const std::map<std::string, std::string> &getFCGIParams(void) const;

    /* Setters */
    void setLocationType(const std::string &locationType);
    void setMatchType(const std::string &matchType);
    void setMatchType(e_matchType matchType);
    void setPath(const std::string &path);
    void setRoot(const std::string &root);
    void setIndex(const std::string &index);
    void setAutoindex(bool autoindex);
    void setClientMaxBodySize(long int size);
    void setReturnCode(int returnCode);
    void setReturnMessage(const std::string &returnMessage);
    void setAllowedMethods(
        const std::set<http::HttpRequest::e_method> &allowedMethods);
    void addAllowedMethod(http::HttpRequest::e_method method);
    void setFCGIServer(const std::string &fcgiServer);
    void setFCGIParams(const std::map<std::string, std::string> &params);
    void addFCGIParam(const std::string &key, const std::string &value);

    /* Public methods */
    bool ismethodAllowed(http::HttpRequest::e_method method) const;
    std::ostream &print(std::ostream &os) const;

private:
    /* Private attributes */
    e_locationType _locationType;
    e_matchType _matchType;
    std::string _path;
    std::string _root;
    std::string _index;
    bool _autoindex;
    long int _clientMaxBodySize;
    int _returnCode;
    std::string _returnMessage;
    std::set<http::HttpRequest::e_method> _allowedMethods;
    std::string _fcgiServer;
    std::map<std::string, std::string> _fcgiParams;

    /* [delete] */
    LocationConfiguration(void);
};  // class LocationConfiguration

std::ostream &operator<<(std::ostream &os,
                         const LocationConfiguration &location);

}  // namespace webserv

#endif  // LOCATIONCONFIGURATION_HPP