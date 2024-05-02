/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfiguration.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 14:34:11 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/02 13:12:27 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIGURATION_HPP
# define LOCATIONCONFIGURATION_HPP

# include <string>
# include <set>
# include <iostream>

# include <cerrno>
# include <cstring>

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
		const std::set<std::string>	&getAllowedMethods(void) const;
		bool						isAutoindex(void) const;
		int							getReturnCode(void) const;
		std::string					getReturnMessage(void) const;

		/* Setters */
		void	setPath(const std::string &path);
		void	setRoot(const std::string &root);
		void	setIndex(const std::string &index);
		void	setAllowedMethods(const std::set<std::string> &allowedMethods);
		void	addAllowedMethod(const std::string &method);
		void	setAutoindex(bool autoindex);
		void	setReturnCode(int returnCode);
		void	setReturnMessage(const std::string &returnMessage);

		/* Public methods */
		bool			isMethodAllowed(const std::string &method) const;
		std::ostream	&print(std::ostream &os) const;


	private:
		/* Private attributes */
		std::string				_path;
		std::string				_root;
		std::string				_index;
		std::set<std::string>	_allowedMethods;
		bool					_autoindex;
		int						_returnCode;
		std::string				_returnMessage;

		/* [delete] */
		LocationConfiguration(void);
}; // class LocationConfiguration

std::ostream	&operator<<(std::ostream &os, const LocationConfiguration
	&location);

} // namespace webserv

#endif // LOCATIONCONFIGURATION_HPP