/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfiguration.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 14:34:11 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/02 12:00:41 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIGURATION_HPP
# define LOCATIONCONFIGURATION_HPP

# include <string>
# include <set>
# include <iostream>

# include <cerrno>
# include <cstring>

# include "strings.hpp"
# include "exceptions.hpp"

namespace	webserv
{

class	LocationConfiguration
{
	public:
		/* Constructors */
		explicit LocationConfiguration(void);
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
		const std::set<std::string>	&getAllowedMethods(void) const;
		bool						isAutoindex(void) const;
		int							getReturnCode(void) const;
		std::string					getReturnMessage(void) const;

		/* Setters */
		void	setPath(const std::string &path);
		void	setRoot(const std::string &root);
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
		std::set<std::string>	_allowedMethods;
		bool					_autoindex;
		int						_returnCode;
		std::string				_returnMessage;


	/* Exceptions */
	public:
		class	InvalidPath : public RuntimeError
		{
			public:
				/* Constructors */
				explicit InvalidPath(void);
				explicit InvalidPath(const std::string &message);
				InvalidPath(const InvalidPath &original);

				/* Destructor */
				~InvalidPath(void) throw();

				/* Operator overloads */
				InvalidPath	&operator=(const InvalidPath &original);
		}; // class InvalidPath
}; // class LocationConfiguration

std::ostream	&operator<<(std::ostream &os, const LocationConfiguration
	&location);

} // namespace webserv

#endif // LOCATIONCONFIGURATION_HPP