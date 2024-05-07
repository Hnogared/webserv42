/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 12:40:13 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/06 16:32:22 by hnogared         ###   ########.fr       */
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
		explicit Server(const std::string &configPath = WS_DFL_CONFIG_PATH);

		/* Destructor */
		~Server(void);

		/* Public methods */
		void	run(void);


	private:
		/* Private attributes */
		static bool					_initialized;
		static bool					_running;
		std::vector<VirtualServer*>	_virtualServers;

		/* Private methods */
		void	_init(const std::string &configPath);
		void	_updateClientsCounts(std::vector<size_t> &clientsCounts) const;
		void	_updateVServersFds(std::vector<pollfd> &fds,
			const std::vector<size_t> &clientsCounts) const;

		static std::vector<Configuration>	*_makeConfigs(const std::string
			&configPath);
		static void	_sigHandler(int signal);

		/* [delete] */
		Server(const Server&);
		Server	&operator=(const Server&);
}; // class Server

} // namespace webserv

#endif // SERVER_HPP
