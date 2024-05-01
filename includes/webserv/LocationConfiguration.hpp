/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfiguration.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 14:34:11 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/01 15:58:02 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIGURATION_HPP
# define LOCATIONCONFIGURATION_HPP

# include <string>
# include <set>
# include <iostream>

# include "strings.hpp"

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
		const std::set<std::string>	&getAllowedMethods(void) const;
		bool						isAutoindex(void) const;

		/* Setters */
		void	setPath(const std::string &path);
		void	setAllowedMethods(const std::set<std::string> &allowedMethods);
		void	addAllowedMethod(const std::string &method);
		void	setAutoindex(bool autoindex);

		/* Public methods */
		bool			isMethodAllowed(const std::string &method) const;
		std::ostream	&print(std::ostream &os) const;


	private:
		/* Private attributes */
		std::string				_path;
		std::set<std::string>	_allowedMethods;
		bool					_autoindex;
};

std::ostream	&operator<<(std::ostream &os, const LocationConfiguration
	&location);

} // namespace webserv

#endif // LOCATIONCONFIGURATION_HPP