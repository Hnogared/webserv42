/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 23:20:31 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 23:48:38 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <string>
# include <sstream>
# include <map>

namespace	webserv
{

class	HttpResponse
{
	public:
		/* Constructors */
		HttpResponse(const std::string &version, int status_code,
			const std::string &reason);
		HttpResponse(const HttpResponse &original);

		/* Destructor */
		~HttpResponse(void);

		/* Operator overloads */
		HttpResponse	&operator=(const HttpResponse &original);

		/* Getters */
		std::string	getVersion(void) const;
		int			getStatusCode(void) const;
		std::string	getReason(void) const;
		std::string	getHeader(const std::string &key) const;
		std::map<std::string, std::string>	getHeaders(void) const;
		std::string	getBody(void) const;

		/* Public methods */
		void		addHeader(const std::string &key, const std::string &val);
		void		setBody(const std::string &body);
		std::string	toString(void) const;


	private:
		/* Private attributes */
		std::string							_version;
		int									_status_code;
		std::string							_reason;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
};

} // namespace webserv

#endif // HTTPRESPONSE_HPP