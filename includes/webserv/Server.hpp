/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 12:40:13 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/03 12:24:28 by hnogared         ###   ########.fr       */
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
		explicit Server(const std::string &config_path = WS_DFL_CONFIG_PATH);

		/* Destructor */
		~Server(void);

		/* Public methods */
		void	run(void);


	private:
		/* Private static attributes */
		static bool	_initialized;
		static bool	_running;

		/* Private attributes */
		std::vector<VirtualServer*>	_virtualServers;

		/* Private methods */
		void		_init(const std::string &config_path);
		static void	_sigHandler(int signal);

		/* [delete] */
		Server(const Server&);
		Server	&operator=(const Server&);
}; // class Server

} // namespace webserv

#endif // SERVER_HPP