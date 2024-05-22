/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 23:20:31 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/22 17:08:41 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <sstream>
#include <string>

#include "HttpRequest.hpp"
#include "strings.hpp"

namespace http
{

class HttpResponse : public HttpMessage
{
public:
    /* Constructors */
    explicit HttpResponse(int statusCode, const std::string &statusLine = "");
    HttpResponse(int statusCode, const HttpRequest &request,
                 const std::string &statusLine = "");
    HttpResponse(const HttpResponse &original);

    /* Destructor */
    ~HttpResponse(void);

    /* Operator overloads */
    HttpResponse &operator=(const HttpResponse &original);

    /* Getters */
    int getStatusCode(void) const;

    /* Setters */
    void setBody(const std::string &body);
    void setBody(const std::string &body, HttpMessage::e_mimeType mime);

    /* Public methods */
    std::string toString(bool withBody = true) const;
    void parseCGIResponse(const std::string &cgiResponse);

private:
    /* Static rivate attributes */
    static const std::map<int, std::string> _statusLines;

    /* Private attributes */
    int _statusCode;

    /* Private static methods */
    static std::map<int, std::string> _initStatusLines(void);
    static std::string _makeBody(int statusCode, const std::string &statusLine);

    /* Private methods */
    void _buildHeadersAndBody(void);
    void _parseCGIHeaders(const std::string &headers);
};  // class HttpResponse

}  // namespace http

#endif  // HTTPRESPONSE_HPP