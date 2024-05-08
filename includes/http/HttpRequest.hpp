/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 22:57:07 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/08 12:05:42 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <string>
# include <stdexcept>
# include <sstream>
# include <algorithm>

# include "exceptions.hpp"
# include "Harl.hpp"
# include "HttpMessage.hpp"
# include "strings.hpp"

namespace	http
{

class	HttpRequest : public HttpMessage
{
	public:
		/* Constructors */
		explicit HttpRequest(void);
		HttpRequest(const HttpRequest &original);

		/* Destructor */
		~HttpRequest(void);

		/* Operator overloads */
		HttpRequest	&operator=(const HttpRequest &original);

		/* Getters */
		std::string	getMethod(void) const;
		std::string	getTarget(void) const;

		/* Public methods */
		void	parseRequestLine(std::string &line);
		void	parseHeaders(std::string &headers);


	private:
		/* Private attributes */
		std::string		_method;
		std::string		_target;


	/* Exceptions */
	public:
		class	RequestException : public webserv::RuntimeError
		{
			public:
				/* Constructors */
				explicit RequestException(void);
				explicit RequestException(const std::string &message,
					int code = 21);
				RequestException(const RequestException &original);

				/* Destructor */
				~RequestException(void) throw();

				/* Operator overloads */
				RequestException	&operator=(const RequestException
					&original);
		}; // class RequestException

		class	BadRequest : public RequestException
		{
			public:
				/* Constructors */
				explicit BadRequest(void);
				explicit BadRequest(const std::string &msg);
				BadRequest(const BadRequest &original);

				/* Destructor */
				~BadRequest(void) throw();

				/* Operator overloads */
				BadRequest	&operator=(const BadRequest &original);
		}; // class IvalidRequest
		
		class	BodyTooLarge : public RequestException
		{
			public:
				/* Constructors */
				explicit BodyTooLarge(void);
				explicit BodyTooLarge(const std::string &msg);
				BodyTooLarge(const BodyTooLarge &original);

				/* Destructor */
				~BodyTooLarge(void) throw();

				/* Operator overloads */
				BodyTooLarge	&operator=(const BodyTooLarge &original);
		}; // class BodyTooLarge
}; // class HttpRequest

} // namespace http

#endif // HTTPREQUEST_HPP