/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 12:40:13 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/12 14:43:47 by hnogared         ###   ########.fr       */
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
		/* Singleton getter */
		static Server	&getInstance(const std::string
			&configPath = WS_DFL_CONFIG_PATH);

		/* Getters */
		Harl	&getLogger(void);

		/* Public methods */
		void	run(void);
		void	stop(void);
		void	reload(void);


	private:
		/* Private attributes */
		bool		_running;
		Harl		_logger;
		std::string	_configPath;
		std::map<std::pair<std::string, int>, VirtualServerManager*> _managers;


		/* Private handlers */
		explicit Server(const std::string&);
		~Server(void);


		/* Private methods */
		void	_init(const std::string &configPath);
		void	_buildFromConfigFile(const std::string &configPath);
		void	_initVirtualServerManagers(const std::vector<Configuration>
			*configs);
		void	_initVirtualServer(const Configuration &config);
		void	_updateFds(std::vector<pollfd> &fds);
		void	_cleanup(bool removeLockFile = true);

		static void	_sigHandler(int signal);


		/* [delete] */
		Server(const Server&);
		Server	&operator=(const Server&);
}; // class Server

} // namespace webserv

#endif // SERVER_HPP
