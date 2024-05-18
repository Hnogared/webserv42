/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfiguration.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 14:34:11 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/18 13:34:41 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIGURATION_HPP
# define LOCATIONCONFIGURATION_HPP

# include <string>
# include <set>
# include <map>
# include <iostream>

# include <cerrno>
# include <cstring>

# include "HttpRequest.hpp"
# include "webserv.hpp"
# include "strings.hpp"
# include "exceptions.hpp"

namespace	webserv
{

class	LocationConfiguration
{
	public:
		/* Constructors */
		explicit LocationConfiguration(const std::string &path);
		LocationConfiguration(const LocationConfiguration &original);
		
		/* Destructor */
		~LocationConfiguration(void);

		/* Operator overloads */
		LocationConfiguration	&operator=(const LocationConfiguration
				&original);
		bool				operator<(const LocationConfiguration &other) const;

		/* Getters */
		std::string					getPath(void) const;
		std::string					getRoot(void) const;
		std::string					getIndex(void) const;
		bool						isAutoindex(void) const;
		int							getReturnCode(void) const;
		std::string					getReturnMessage(void) const;
		const std::set<http::HttpRequest::e_method>	&getAllowedMethods(void)
			const;
		std::string									getFCGIServer(void) const;
		const std::map<std::string, std::string>	&getFCGIParams(void) const;

		/* Setters */
		void	setPath(const std::string &path);
		void	setRoot(const std::string &root);
		void	setIndex(const std::string &index);
		void	setAutoindex(bool autoindex);
		void	setReturnCode(int returnCode);
		void	setReturnMessage(const std::string &returnMessage);
		void	setAllowedMethods(const std::set<http::HttpRequest::e_method>
			&allowedMethods);
		void	addAllowedMethod(http::HttpRequest::e_method method);
		void	setFCGIServer(const std::string &fcgiServer);
		void	setFCGIParams(const std::map<std::string, std::string> &params);
		void	addFCGIParam(const std::string &key, const std::string &value);

		/* Public methods */
		bool			methodAllowed(http::HttpRequest::e_method method) const;
		std::ostream	&print(std::ostream &os) const;


	private:
		/* Private attributes */
		std::string								_path;
		std::string								_root;
		std::string								_index;
		bool									_autoindex;
		int										_returnCode;
		std::string								_returnMessage;
		std::set<http::HttpRequest::e_method>	_allowedMethods;
		std::string								_fcgiServer;
		std::map<std::string, std::string>		_fcgiParams;

		/* [delete] */
		LocationConfiguration(void);
}; // class LocationConfiguration

std::ostream	&operator<<(std::ostream &os, const LocationConfiguration
	&location);

} // namespace webserv

#endif // LOCATIONCONFIGURATION_HPP