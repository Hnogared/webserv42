/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 18:20:17 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/12 22:32:22 by hnogared         ###   ########.fr       */
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
		/* Constructors */
		explicit HttpMessage(void);
		explicit HttpMessage(const std::string &statusLine);
		HttpMessage(const HttpMessage &original);

		/* Destructor */
		virtual ~HttpMessage(void);

		/* Operator overloads */
		HttpMessage	&operator=(const HttpMessage &original);


		/* Getters */
		const Protocol	&getProtocol(void) const;
		std::string		getStatusLine(void) const;
		std::string		getHeader(const std::string &key) const;
		const std::map<std::string, std::string>	&getHeaders(void) const;
		std::string		getBody(void) const;

		/* Setters */
		void	setProtocol(const std::string &protocol);
		void	setStatusLine(const std::string &statusLine);
		void	addHeader(const std::string &key, const std::string &val);
		void	setHeader(const std::string &key, const std::string &val);

		/* Virtual setters */
		virtual void	setBody(const std::string &body);

		/* Public methods */
		virtual void	clear(void);


	private:
		/* Private attributes */
		Protocol							_protocol;
		std::string							_statusLine;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
};

std::ostream	&operator<<(std::ostream &out, const HttpMessage &http_msg);

} // namespace http

#endif // HTTPMESSAGE_HPP