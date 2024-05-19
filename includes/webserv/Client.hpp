/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 14:42:17 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/19 22:28:22 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string.h>
#include <sys/socket.h>

#include <cerrno>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

#include "Harl.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Socket.hpp"
#include "exceptions.hpp"
#include "net.hpp"
#include "webserv.hpp"

namespace webserv
{

class Client
{
public:
    enum e_addr_choice
    {
        LOCAL,
        PEER
    };

    /* Constructors */
    explicit Client(const Socket &socket);
    Client(const Client &original);

    /* Destructor */
    ~Client(void);

    /* Operator overloads */
    Client &operator=(const Client &original);
    bool operator==(const Client &other) const;

    /* Getters */
    int getSocketFd(void) const;
    Socket getSocket(void) const;
    const Socket *getSocketPtr(void) const;
    std::string getAddrStr(e_addr_choice choice) const;
    bool isRequestPending(void) const;
    const http::HttpRequest &getRequest(void) const;
    const std::string &getBuffer(void) const;

    /* Public methods */
    void sendResponse(const http::HttpResponse &response);
    void fetchRequestLineAndHeaders(const http::Protocol &protocol);
    void fetchRequestBody(size_t maxBodyLen);

private:
    /* Private attributes */
    Socket _socket;
    bool _requestPending;
    http::HttpRequest _request;
    std::string _buffer;

    /* Private methods */
    std::string _readRequestBlock(size_t maxBuffSize = 0) const;

    /* Exceptions */
public:
    class ClientException : public RuntimeError
    {
    public:
        /* Constructors */
        explicit ClientException(void);
        explicit ClientException(const std::string &message, int code = 31);
        ClientException(const ClientException &original);

        /* Destructor */
        ~ClientException(void) throw();

        /* Operator overloads */
        ClientException &operator=(const ClientException &original);
    };  // class ClientException

    class ClientReadException : public ClientException
    {
    public:
        /* Constructors */
        explicit ClientReadException(void);
        explicit ClientReadException(const std::string &message);
        ClientReadException(const ClientReadException &original);

        /* Destructor */
        ~ClientReadException(void) throw();

        /* Operator overloads */
        ClientReadException &operator=(const ClientReadException &original);
    };  // class ClientReadException

    class ClientWriteException : public ClientException
    {
    public:
        /* Constructors */
        explicit ClientWriteException(void);
        explicit ClientWriteException(const std::string &message);
        ClientWriteException(const ClientWriteException &original);

        /* Destructor */
        ~ClientWriteException(void) throw();

        /* Operator overloads */
        ClientWriteException &operator=(const ClientWriteException &original);
    };  // class ClientWriteException
};      // class Client

}  // namespace webserv

#endif  // CLIENT_HPP