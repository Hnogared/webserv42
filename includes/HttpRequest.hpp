/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 22:57:07 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 23:15:23 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <string>
# include <stdexcept>
# include <sstream>

namespace	webserv
{

class	HttpRequest
{
	public:
		/* Constructors */
		HttpRequest(const std::string &request);
		HttpRequest(const HttpRequest &original);

		/* Destructor */
		~HttpRequest(void);

		/* Operator overloads */
		HttpRequest	&operator=(const HttpRequest &original);

		/* Getters */
		std::string	getMethod(void) const;
		std::string	getTarget(void) const;
		std::string	getVersion(void) const;


	private:
		/* Private attributes */
		std::string	_method;
		std::string	_target;
		std::string	_version;


	/* Exceptions */
	public:
		class	BadRequest : public std::runtime_error
		{
			public:
				/* Constructors */
				explicit BadRequest(const std::string &msg);
				BadRequest(const BadRequest &original);

				/* Destructor */
				virtual ~BadRequest(void) throw();

				/* Operator overloads */
				BadRequest	&operator=(const BadRequest &original);
		};
};

} // namespace webserv

#endif // HTTPREQUEST_HPP