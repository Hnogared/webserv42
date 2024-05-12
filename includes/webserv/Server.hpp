/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 12:40:13 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/12 04:04:38 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <map>

# include "ConfigurationParser.hpp"
# include "VirtualServerManager.hpp"

namespace	webserv
{

class	Server
{
	public:
		/* Constructors */
		explicit Server(const std::string &configPath = WS_DFL_CONFIG_PATH);

		/* Destructor */
		~Server(void);

		/* Public methods */
		void	run(void);


	private:
		/* Private attributes */
		Harl								_logger;
		static bool							_initialized;
		static bool							_running;
		std::map<std::pair<std::string, int>, VirtualServerManager*> _managers;

		/* Private methods */
		void	_init(const std::string &configPath);
		void	_initVirtualServerManagers(const std::vector<Configuration>
			*configs);
		void	_initVirtualServer(const Configuration &config);
		std::vector<Configuration>	*_makeConfigs(const std::string
			&configPath);
		void	_updateFds(std::vector<pollfd> &fds);
		void	_cleanup(void);

		static void	_sigHandler(int signal);

		/* [delete] */
		Server(const Server&);
		Server	&operator=(const Server&);
}; // class Server

} // namespace webserv

#endif // SERVER_HPP
