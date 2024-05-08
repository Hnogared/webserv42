/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 22:57:07 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/08 04:13:10 by hnogared         ###   ########.fr       */
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
		//explicit HttpRequest(const std::string &request);
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
		class	InvalidRequest : public webserv::RuntimeError
		{
			public:
				/* Constructors */
				explicit InvalidRequest(void);
				InvalidRequest(const InvalidRequest &original);

				/* Destructor */
				~InvalidRequest(void) throw();

				/* Operator overloads */
				InvalidRequest	&operator=(const InvalidRequest &original);
		}; // class IvalidRequest
}; // class HttpRequest

} // namespace http

#endif // HTTPREQUEST_HPP