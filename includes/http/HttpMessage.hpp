/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 18:20:17 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/07 15:17:49 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPMESSAGE_HPP
# define HTTPMESSAGE_HPP

# include <iostream>
# include <string>
# include <map>

# include "webserv.hpp"

namespace	http
{

class	HttpMessage
{
	public:
		/* Constructors */
		explicit HttpMessage(void);
		explicit HttpMessage(const std::string &statusLine,
			const std::string &version = WS_HTTP_VERSION);
		HttpMessage(const HttpMessage &original);

		/* Destructor */
		virtual ~HttpMessage(void);

		/* Operator overloads */
		HttpMessage	&operator=(const HttpMessage &original);


		/* Getters */
		std::string	getVersion(void) const;
		std::string	getStatusLine(void) const;
		std::string	getHeader(const std::string &key) const;
		std::map<std::string, std::string>	getHeaders(void) const;
		std::string	getBody(void) const;
		bool		isValid(void) const;

		/* Setters */
		void	setVersion(const std::string &version);
		void	setStatusLine(const std::string &statusLine);
		void	addHeader(const std::string &key, const std::string &val);
		void	setBody(const std::string &body);
		void	setValidity(bool isValid);


	private:
		/* Private attributes */
		std::string							_version;
		std::string							_statusLine;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		bool								_isValid;
};

std::ostream	&operator<<(std::ostream &out, const HttpMessage &http_msg);

} // namespace http

#endif // HTTPMESSAGE_HPP