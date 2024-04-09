/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 22:57:07 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/09 19:22:14 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <string>
# include <stdexcept>
# include <sstream>
# include <algorithm>

# include "HttpMessage.hpp"
# include "utils.hpp"

namespace	http
{

class	HttpRequest : public HttpMessage
{
	public:
		/* Constructors */
		explicit HttpRequest(void);
		explicit HttpRequest(const std::string &request);
		HttpRequest(const HttpRequest &original);

		/* Destructor */
		~HttpRequest(void);

		/* Operator overloads */
		HttpRequest	&operator=(const HttpRequest &original);

		/* Getters */
		bool		isValid(void) const;
		std::string	getMethod(void) const;
		std::string	getTarget(void) const;


	private:
		/* Private attributes */
		bool			_is_valid;
		std::string		_method;
		std::string		_target;

		/* Private methods */
		void	_parseRequestLine(std::istringstream &iss_request);
		void	_parseHeaders(std::istringstream &iss_request);
};

} // namespace http

#endif // HTTPREQUEST_HPP