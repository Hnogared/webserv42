/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 21:06:56 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/19 22:34:00 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exceptions.hpp"

namespace webserv
{

/* ************************************************************************** */
/* Runtime error                                                              */
/* ************************************************************************** */

/* Default constructor */
RuntimeError::RuntimeError(int code)
    : std::runtime_error("Socket error"), _code(code)
{
}

/* Message constructor */
RuntimeError::RuntimeError(const std::string &message, int code)
    : std::runtime_error(message), _code(code)
{
}

/* Copy constructor */
RuntimeError::RuntimeError(const RuntimeError &original)
    : std::runtime_error("")
{
    *this = original;
}

/* Destructor */
RuntimeError::~RuntimeError(void) throw() {}

/* ************************************************************************** */
/* Operator overloads */

/* Copy assignment operator */
RuntimeError &RuntimeError::operator=(const RuntimeError &original)
{
    if (this == &original) return (*this);
    std::runtime_error::operator=(original);
    this->_code = original.code();
    return (*this);
}

/* ************************************************************************** */
/* Getters */

int RuntimeError::code(void) const { return (this->_code); }

/* ************************************************************************** */
/* Socket error                                                               */
/* ************************************************************************** */

/* Default constructor */
SocketError::SocketError(int code) : RuntimeError("Socket error", code) {}

/* Message constructor */
SocketError::SocketError(const std::string &message, int code)
    : RuntimeError(message, code)
{
}

/* Copy constructor */
SocketError::SocketError(const SocketError &original) : RuntimeError()
{
    *this = original;
}

/* Destructor */
SocketError::~SocketError(void) throw() {}

/* ************************************************************************** */
/* Operator overloads */

/* Copy assignment operator */
SocketError &SocketError::operator=(const SocketError &original)
{
    if (this == &original) return (*this);
    RuntimeError::operator=(original);
    return (*this);
}

/* ************************************************************************** */
/* Socket creation error                                                      */
/* ************************************************************************** */

/* Default constructor */
SocketCreationError::SocketCreationError()
    : SocketError("Error creating socket", 7)
{
}

/* Message constructor */
SocketCreationError::SocketCreationError(const std::string &message)
    : SocketError(message, 7)
{
}

/* Copy constructor */
SocketCreationError::SocketCreationError(const SocketCreationError &original)
    : SocketError(original)
{
    *this = original;
}

/* Destructor */
SocketCreationError::~SocketCreationError(void) throw() {}

/* ************************************************************************** */
/* Operator overloads */

/* Copy assignment operator */
SocketCreationError &SocketCreationError::operator=(
    const SocketCreationError &original)
{
    if (this == &original) return (*this);
    SocketError::operator=(original);
    return (*this);
}

/* ************************************************************************** */
/* Socket connection error                                                    */
/* ************************************************************************** */

/* Default constructor */
SocketConnectionError::SocketConnectionError()
    : SocketError("Error binding socket", 8)
{
}

/* Message constructor */
SocketConnectionError::SocketConnectionError(const std::string &message)
    : SocketError(message, 8)
{
}

/* Copy constructor */
SocketConnectionError::SocketConnectionError(
    const SocketConnectionError &original)
    : SocketError(original)
{
    *this = original;
}

/* Destructor */
SocketConnectionError::~SocketConnectionError(void) throw() {}

/* ************************************************************************** */
/* Operator overloads */

/* Copy assignment operator */
SocketConnectionError &SocketConnectionError::operator=(
    const SocketConnectionError &original)
{
    if (this == &original) return (*this);
    SocketError::operator=(original);
    return (*this);
}

/* ************************************************************************** */
/* Socket connection closed                                                   */
/* ************************************************************************** */

/* Default constructor */
SocketConnectionClosed::SocketConnectionClosed()
    : SocketError("Connection closed", 9)
{
}

/* Message constructor */
SocketConnectionClosed::SocketConnectionClosed(const std::string &message)
    : SocketError(message, 9)
{
}

/* Copy constructor */
SocketConnectionClosed::SocketConnectionClosed(
    const SocketConnectionClosed &original)
    : SocketError(original)
{
    *this = original;
}

/* Destructor */
SocketConnectionClosed::~SocketConnectionClosed(void) throw() {}

/* ************************************************************************** */
/* Operator overloads */

/* Copy assignment operator */
SocketConnectionClosed &SocketConnectionClosed::operator=(
    const SocketConnectionClosed &original)
{
    if (this == &original) return (*this);
    SocketError::operator=(original);
    return (*this);
}

/* ************************************************************************** */
/* Invalid Path                                                               */
/* ************************************************************************** */

/* Default constructor */
InvalidPath::InvalidPath(void) : RuntimeError("Invalid path", 23) {}

/* Message constructor */
InvalidPath::InvalidPath(const std::string &message) : RuntimeError(message, 23)
{
}

/* Copy constructor */
InvalidPath::InvalidPath(const InvalidPath &original) : RuntimeError(original)
{
}

/* Destructor */
InvalidPath::~InvalidPath(void) throw() {}

/* ************************************************************************** */
/* Operator overloads */

/* Copy assignment operator */
InvalidPath &InvalidPath::operator=(const InvalidPath &original)
{
    if (this != &original) RuntimeError::operator=(original);
    return (*this);
}

/* ************************************************************************** */
/* FileException                                                              */
/* ************************************************************************** */

/* Message and code constructor */
FileException::FileException(const std::string &message, int code)
    : RuntimeError(message, code)
{
}

/* Copy constructor */
FileException::FileException(const FileException &original)
    : RuntimeError(original)
{
}

/* Destructor */
FileException::~FileException(void) throw() {}

/* ************************************************************************** */
/* Operator overloads */

FileException &FileException::operator=(const FileException &original)
{
    RuntimeError::operator=(original);
    return (*this);
}

}  // namespace webserv