/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 09:34:06 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/25 14:28:43 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include <string>

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
		int			getPort(void) const;
		std::string	getServerName(void) const;

		/* Setters */
		void		setPort(int port);
		void		setServerName(const std::string &serverName);


	private:
		/* Private attributes */
		int			_port;
		std::string	_serverName;
		
};

} // namespace webserv

#endif // CONFIGURATION_HPP