/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 18:20:17 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/08 18:35:48 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPMESSAGE_HPP
# define HTTPMESSAGE_HPP

# include <string>
# include <map>

namespace	http
{

class	HttpMessage
{
	public:
		/* Constructors */
		explicit HttpMessage(void);
		explicit HttpMessage(const std::string &status_line,
			const std::string &version);
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

		/* Setters */
		void	setVersion(const std::string &version);
		void	setStatusLine(const std::string &status_line);
		void	addHeader(const std::string &key, const std::string &val);
		void	setBody(const std::string &body);


	private:
		/* Private attributes */
		std::string							_version;
		std::string							_status_line;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
};

} // namespace http

#endif // HTTPMESSAGE_HPP