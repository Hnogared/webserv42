/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 23:20:31 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/08 18:34:36 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <string>
# include <sstream>
# include <map>

# include "HttpMessage.hpp"

namespace	http
{

class	HttpResponse : public HttpMessage
{
	public:
		/* Constructors */
		HttpResponse(int status_code, const std::string &status_line,
			const std::string &version);
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
		int	_status_code;
};

} // namespace http

#endif // HTTPRESPONSE_HPP