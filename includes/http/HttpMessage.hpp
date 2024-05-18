/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 18:20:17 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/18 13:03:44 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPMESSAGE_HPP
# define HTTPMESSAGE_HPP

# include <iostream>
# include <string>
# include <map>

# include "Protocol.hpp"
# include "webserv.hpp"
# include "exceptions.hpp"

namespace	http
{

class	HttpMessage
{
	public:
		enum	e_mimeType
		{
			TEXT_PLAIN,
			TEXT_HTML,
			TEXT_CSS,
			IMAGE_PNG,
			IMAGE_JPEG,
			IMAGE_GIF,
			AUDIO_MPEG,
			AUDIO_MP3,
			VIDEO_MPEG,
			VIDEO_MP4,
			APPLICATION_JAVASCRIPT,
			APPLICATION_PDF,
			APPLICATION_OCTET_STREAM,
		};

		/* Constructors */
		explicit HttpMessage(void);
		explicit HttpMessage(const std::string &statusLine);
		HttpMessage(const HttpMessage &original);

		/* Destructor */
		virtual ~HttpMessage(void);

		/* Operator overloads */
		HttpMessage	&operator=(const HttpMessage &original);


		/* Getters */
		const Protocol		&getProtocol(void) const;
		const std::string	&getStatusLine(void) const;
		std::string			getHeader(const std::string &key) const;
		const std::map<std::string, std::string>	&getHeaders(void) const;
		const std::string	&getBody(void) const;

		/* Setters */
		void	setProtocol(const std::string &protocol);
		void	setStatusLine(const std::string &statusLine);
		void	addHeader(const std::string &key, const std::string &val);
		void	setHeader(const std::string &key, const std::string &val);
		void	setBody(const std::string &body);
		void	setContentType(e_mimeType mime);

		/* Public methods */
		virtual void	clear(void);

		/* Static public methods */
		static e_mimeType	getMimeType(const std::string &path);


	private:
		/* Private attributes */
		Protocol							_protocol;
		std::string							_statusLine;
		std::map<std::string, std::string>	_headers;
		std::string							_body;

		/* Static private attributes */
		static const std::map<std::string, e_mimeType>	_extToMimeMap;
		static const std::map<e_mimeType, std::string>	_mimeToTypeMap;

		/* Static private methods */
		static const std::map<std::string, HttpMessage::e_mimeType>
			_initExtToMimeMap(void);
		static const std::map<HttpMessage::e_mimeType, std::string>
			_initMimeToTypeMap(void);
};

std::ostream	&operator<<(std::ostream &out, const HttpMessage &http_msg);

} // namespace http

#endif // HTTPMESSAGE_HPP