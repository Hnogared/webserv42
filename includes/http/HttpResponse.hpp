/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 23:20:31 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/07 19:05:58 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <string>
# include <sstream>

# include "HttpMessage.hpp"
# include "strings.hpp"

namespace	http
{

class	HttpResponse : public HttpMessage
{
	public:
		/* Constructors */
		explicit HttpResponse(int statusCode,
			const std::string &statusLine = "");
		HttpResponse(const HttpResponse &original);

		/* Destructor */
		~HttpResponse(void);

		/* Operator overloads */
		HttpResponse	&operator=(const HttpResponse &original);

		/* Getters */
		int	getStatusCode(void) const;

		/* Public methods */
		std::string	toString(void) const;


	private:
		/* Private attributes */
		static const std::map<int, std::string>	_statusLines;
		int										_statusCode;

		/* Private static methods */
		static std::map<int, std::string>	_initStatusLines(void);
		static std::string					_makeBody(int statusCode,
			const std::string &statusLine);

		/* Private methods */
		void	_buildHeadersAndBody(void);
};

} // namespace http

#endif // HTTPRESPONSE_HPP