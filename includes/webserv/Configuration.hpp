/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 09:34:06 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/11 23:13:00 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include <algorithm>
# include <string>
# include <vector>
# include <set>
# include <map>
# include <iostream>
# include <netinet/in.h>

# include "webserv.hpp"
# include "net.hpp"
# include "LocationConfiguration.hpp"

namespace	webserv
{

class	Configuration
{
	public:
		/* Constructors */
		explicit Configuration(void);
		Configuration(const Configuration &original);

		/* Destructor */
		~Configuration(void);

		/* Operator overloads */
		Configuration &operator=(const Configuration &original);

		/* Getters */
		sockaddr_in							&getAddress(void);
		const sockaddr_in					&getConstAddress(void) const;
		std::string							getAddressString(void) const;
		int									getPort(void) const;
		unsigned int						getBacklog(void) const;
		const std::set<std::string>			&getServerNames(void) const;
		std::string							getRoot(void) const;
		std::string							getIndex(void) const;
		const std::map<int, std::string>	&getErrorRedirects(void) const;
		unsigned long int					getClientMaxBodySize(void) const;
		const std::set<LocationConfiguration>	&getLocations(void) const;

		/* Setters */
		void	setAddress(const std::string &address);
		void	setAddress(const sockaddr_in &address);
		void	setBacklog(unsigned int backlog);
		void	setPort(int port);
		void	addServerName(const std::string &serverName);
		void	setRoot(const std::string &root);
		void	setIndex(const std::string &index);
		void	addErrorRedirect(int error, const std::string &redirect);
		void	setClientMaxBodySize(unsigned long int size);
		void	addLocation(const LocationConfiguration &location);

		/* Public methods */
		const LocationConfiguration*	findBestLocation(const std::string &uri)
			const;
		std::ostream	&print(std::ostream &os = std::cout) const;


	private:
		/* Private attributes */
		sockaddr_in						_address;
		unsigned int					_backlog;
		std::set<std::string>			_serverNames;
		std::string						_root;
		std::string						_index;
		std::map<int, std::string>		_errorRedirects;
		unsigned long int				_clientMaxBodySize;
		std::set<LocationConfiguration>	_locations;
}; // class Configuration

std::ostream	&operator<<(std::ostream &os, const Configuration &config);

} // namespace webserv

#endif // CONFIGURATION_HPP