/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 12:40:13 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/02 17:46:43 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>

# include "ConfigurationParser.hpp"
# include "VirtualServer.hpp"

namespace	webserv
{

class	Server
{
	public:
		/* Constructors */
		explicit Server(const std::string &config_path);

		/* Destructor */
		~Server(void);

	private:
		/* Private static attributes */
		static bool	_running;
		
		/* Private attributes */
		std::vector<VirtualServer*>	_virtualServers;

		/* Private methods */
		static void	sigHandler(int signal);

		/* [delete] */
		Server(void);
		Server(const Server&);
		Server	&operator=(const Server&);
}; // class Server

} // namespace webserv

#endif // SERVER_HPP