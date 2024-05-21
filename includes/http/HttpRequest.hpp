/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 22:57:07 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/21 23:16:01 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>

#include "Harl.hpp"
#include "HttpMessage.hpp"
#include "exceptions.hpp"
#include "http_common.hpp"
#include "strings.hpp"

namespace http
{

class HttpRequest : public HttpMessage
{
public:
    enum e_method
    {
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        UNKNOWN,
    };

    /* Constructors */
    explicit HttpRequest(void);
    HttpRequest(const HttpRequest &original);

    /* Destructor */
    ~HttpRequest(void);

    /* Operator overloads */
    HttpRequest &operator=(const HttpRequest &original);

    /* Getters */
    HttpRequest::e_method getMethod(void) const;
    const std::string &getMethodStr(void) const;
    const std::string &getUri(void) const;
    const std::string &getQueryString(void) const;

    /* Public methods */
    void parseRequestLine(std::string &line);
    void parseHeaders(std::string &headers);

    /* HttpMessage virtual methods overloads */
    void clear(void);

    /* Static public methods */
    static e_method strToMethod(const std::string &str);
    static const std::string &methodToStr(e_method method);

private:
    /* Private attributes */
    e_method _method;
    std::string _uri;
    std::string _queryString;

    /* Static private attributes */
    static const std::map<std::string, e_method> _strToMethodMap;

    /* Static private methods */
    static const std::map<std::string, e_method> _initStrToMethodMap();

    /* Exceptions */
public:
    class RequestException : public webserv::RuntimeError
    {
    public:
        /* Constructors */
        explicit RequestException(void);
        explicit RequestException(const std::string &message, int code = 21);
        RequestException(const RequestException &original);

        /* Destructor */
        ~RequestException(void) throw();

        /* Operator overloads */
        RequestException &operator=(const RequestException &original);
    };  // class RequestException

    class BadRequest : public RequestException
    {
    public:
        /* Constructors */
        explicit BadRequest(void);
        explicit BadRequest(const std::string &msg);
        BadRequest(const BadRequest &original);

        /* Destructor */
        ~BadRequest(void) throw();

        /* Operator overloads */
        BadRequest &operator=(const BadRequest &original);
    };  // class IvalidRequest

    class BodyTooLarge : public RequestException
    {
    public:
        /* Constructors */
        explicit BodyTooLarge(void);
        explicit BodyTooLarge(const std::string &msg);
        BodyTooLarge(const BodyTooLarge &original);

        /* Destructor */
        ~BodyTooLarge(void) throw();

        /* Operator overloads */
        BodyTooLarge &operator=(const BodyTooLarge &original);
    };  // class BodyTooLarge
};      // class HttpRequest

}  // namespace http

#endif  // HTTPREQUEST_HPP