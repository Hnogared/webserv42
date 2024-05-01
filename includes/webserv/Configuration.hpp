/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 09:34:06 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/01 13:37:41 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include <string>
# include <vector>
# include <map>
# include <iostream>
# include <netinet/in.h>

# include "net.hpp"

namespace	webserv
{

class	Configuration
{
	public:
		/* Constructors */
		explicit Configuration(void);
		explicit Configuration(const std::string &path);
		Configuration(const Configuration &original);

		/* Destructor */
		~Configuration(void);

		/* Operator overloads */
		Configuration &operator=(const Configuration &original);

		/* Getters */
		sockaddr_in							getAddress(void) const;
		std::string							getAddressString(void) const;
		int									getPort(void) const;
		const std::vector<std::string>		&getServerNames(void) const;
		const std::map<int, std::string>	&getErrorRedirects(void) const;
		unsigned long int					getClientMaxBodySize(void) const;

		/* Setters */
		int		setAddress(const std::string &address);
		void	setAddress(const sockaddr_in &address);
		void	setPort(int port);
		void	addServerName(const std::string &serverName);
		void	addErrorRedirect(int error, const std::string &redirect);
		void	setClientMaxBodySize(unsigned long int size);

		/* Public methods */
		std::ostream	&print(std::ostream &os = std::cout) const;


	private:
		/* Private attributes */
		sockaddr_in					_address;
		std::vector<std::string>	_serverNames;
		std::map<int, std::string>	_errorRedirects;
		unsigned long int			_clientMaxBodySize;
}; // class Configuration

std::ostream	&operator<<(std::ostream &os, const Configuration &config);

} // namespace webserv

#endif // CONFIGURATION_HPP