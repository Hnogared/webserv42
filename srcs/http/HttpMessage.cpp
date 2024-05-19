/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 18:25:00 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/19 17:25:45 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpMessage.hpp"

namespace	http
{

/* ************************************************************************** */
/* Static attributes initialization */

const std::map<std::string, HttpMessage::e_mimeType>
	HttpMessage::_extToMimeMap = HttpMessage::_initExtToMimeMap();

const std::map<HttpMessage::e_mimeType, std::string>
	HttpMessage::_mimeToTypeMap = HttpMessage::_initMimeToTypeMap();

/* ************************************************************************** */

/* Default constructor */
HttpMessage::HttpMessage(void) : _protocol(WS_HTTP_VERSION) {}

/* Version + message constructor */
HttpMessage::HttpMessage(const std::string &statusLine)
	: _statusLine(statusLine)
{
	size_t	pos = statusLine.find_last_of("HTTP/");

	if (pos != std::string::npos)
		this->_protocol = Protocol(statusLine.substr(pos));
	else
		this->_protocol = Protocol(WS_HTTP_VERSION);
}

/* Copy constructor */
HttpMessage::HttpMessage(const HttpMessage &original)
{
	*this = original;
}


/* Destructor */
HttpMessage::~HttpMessage(void) {}


/* ************************************************************************** */
/* Operator overloads */

/* Copy assignment operator */
HttpMessage	&HttpMessage::operator=(const HttpMessage &original)
{
	if (this == &original)
		return (*this);

	this->_protocol = original.getProtocol();
	this->_statusLine = original.getStatusLine();
	this->_headers = original.getHeaders();
	this->_body = original.getBody();
	return (*this);
}


/* ************************************************************************** */
/* Getters */

const Protocol	&HttpMessage::getProtocol(void) const
{
	return (this->_protocol);
}

const std::string	&HttpMessage::getStatusLine(void) const
{
	return (this->_statusLine);
}

std::string	HttpMessage::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator	it;

	it = this->_headers.find(key);
	if (it == this->_headers.end())
		return ("");
	return (it->second);
}

const std::map<std::string, std::string>	&HttpMessage::getHeaders(void) const
{
	return (this->_headers);
}

const std::string	&HttpMessage::getBody(void) const
{
	return (this->_body);
}


/* ************************************************************************** */
/* Setters */

void	HttpMessage::setProtocol(const std::string &protocol)
{
	this->_protocol = Protocol(protocol);
}

void	HttpMessage::setStatusLine(const std::string &statusLine)
{
	this->_statusLine = statusLine;
}

void	HttpMessage::addHeader(const std::string &key, const std::string &val)
{
	if (this->_headers.find(key) != this->_headers.end())
		this->_headers[key] += "," + val;
	else
		this->_headers[key] = val;
}

void	HttpMessage::setHeader(const std::string &key, const std::string &val)
{
	this->_headers[key] = val;
}

void	HttpMessage::setBody(const std::string &body)
{
	this->_body = body;
}

void	HttpMessage::setContentType(e_mimeType mime)
{
	this->setHeader("Content-Type", HttpMessage::_mimeToTypeMap.at(mime));
}


/* ************************************************************************** */
/* Public methods */

void	HttpMessage::clear(void)
{
	this->_statusLine.clear();
	this->_headers.clear();
	this->_body.clear();
}


/* ************************************************************************** */
/* Static public methods */

HttpMessage::e_mimeType	HttpMessage::getMimeType(const std::string &filePath)
{
	std::map<std::string, e_mimeType>::const_iterator	it;
	size_t	pos = filePath.find_last_of('.');

	if (pos == std::string::npos)
		return APPLICATION_OCTET_STREAM;

	it = HttpMessage::_extToMimeMap.find(filePath.substr(pos));
	if (it != HttpMessage::_extToMimeMap.end())
		return it->second;

	return APPLICATION_OCTET_STREAM;
}


/* ************************************************************************** */
/* Static private methods */

const std::map<std::string, HttpMessage::e_mimeType>
	HttpMessage::_initExtToMimeMap(void)
{
	std::map<std::string, HttpMessage::e_mimeType>	mimeTypes;

	mimeTypes[".txt"] = HttpMessage::TEXT_PLAIN;
	mimeTypes[".html"] = HttpMessage::TEXT_HTML;
	mimeTypes[".css"] = HttpMessage::TEXT_CSS;
	mimeTypes[".png"] = HttpMessage::IMAGE_PNG;
	mimeTypes[".jpg"] = HttpMessage::IMAGE_JPEG;
	mimeTypes[".jpeg"] = HttpMessage::IMAGE_JPEG;
	mimeTypes[".gif"] = HttpMessage::IMAGE_GIF;
	mimeTypes[".mp3"] = HttpMessage::AUDIO_MPEG;
	mimeTypes[".mpg"] = HttpMessage::VIDEO_MPEG;
	mimeTypes[".mpeg"] = HttpMessage::VIDEO_MPEG;
	mimeTypes[".mp4"] = HttpMessage::VIDEO_MP4;
	mimeTypes[".js"] = HttpMessage::APPLICATION_JAVASCRIPT;
	mimeTypes[".pdf"] = HttpMessage::APPLICATION_PDF;

	return (mimeTypes);
}

const std::map<HttpMessage::e_mimeType, std::string>
	HttpMessage::_initMimeToTypeMap(void)
{
	std::map<HttpMessage::e_mimeType, std::string>	mimeTypes;

	mimeTypes[HttpMessage::TEXT_PLAIN] = "text/plain";
	mimeTypes[HttpMessage::TEXT_HTML] = "text/html";
	mimeTypes[HttpMessage::TEXT_CSS] = "text/css";
	mimeTypes[HttpMessage::IMAGE_PNG] = "image/png";
	mimeTypes[HttpMessage::IMAGE_JPEG] = "image/jpeg";
	mimeTypes[HttpMessage::IMAGE_GIF] = "image/gif";
	mimeTypes[HttpMessage::AUDIO_MPEG] = "audio/mpeg";
	mimeTypes[HttpMessage::AUDIO_MP3] = "audio/mp3";
	mimeTypes[HttpMessage::VIDEO_MPEG] = "video/mpeg";
	mimeTypes[HttpMessage::VIDEO_MP4] = "video/mp4";
	mimeTypes[HttpMessage::APPLICATION_JAVASCRIPT] = "application/javascript";
	mimeTypes[HttpMessage::APPLICATION_PDF] = "application/pdf";
	mimeTypes[HttpMessage::APPLICATION_OCTET_STREAM]
		= "application/octet-stream";

	return (mimeTypes);
}


/* ************************************************************************** */
/* External operator overloads                                                */
/* ************************************************************************** */

std::ostream	&operator<<(std::ostream &os, const HttpMessage &http_msg)
{
	std::map<std::string, std::string>	headers = http_msg.getHeaders();
	std::map<std::string, std::string>::const_iterator	it;

	os << http_msg.getStatusLine() << std::endl;
	for (it = headers.begin(); it != headers.end(); ++it)
		os << it->first << ": " << it->second << std::endl;
	os << std::endl;
	os << http_msg.getBody();
	return (os);
}

} // namespace http