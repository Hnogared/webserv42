/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServerManager.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 20:51:17 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/13 12:06:01 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VIRTUALSERVERMANAGER_HPP
# define VIRTUALSERVERMANAGER_HPP

# include "VirtualServer.hpp"

namespace	webserv
{

class VirtualServerManager
{
	public:
		/* Constructors */
		explicit VirtualServerManager(const http::Protocol &protocol,
			Harl *logger = NULL);

		/* Destructor */
		~VirtualServerManager(void);

		/* Getters */
		const http::Protocol				&getProtocol(void) const;
		const Harl							*getLogger(void) const;
		Socket								getSocket(void) const;
		const std::vector<const Socket*>	getSockets(void) const;
		const std::vector<VirtualServer*>	&getServers(void) const;
		const std::vector<Client*>			&getClients(void) const;
		size_t								getSocketsCount(void) const;

		/* Setters */
		void	addServer(VirtualServer *server);
		void	addClient(Client *client);

		/* Public methods */
		bool	tryServeFd(int fd);
		void	stop(void);


	private:
		/* Private attributes */
		http::Protocol				_protocol;
		Harl						*_logger;
		Socket						_socket;
		VirtualServer				*_defaultServer;
		std::vector<VirtualServer*>	_servers;
		std::vector<Client*>		_clients;


		/* Private methods */
		void	_log(Harl::e_level level, const Client *client,
			const std::string &message);
		void	_acceptConnection(void);
		bool	_serveClient(Client *client);


		/* [delete] */
		VirtualServerManager(const VirtualServerManager&);
		VirtualServerManager	&operator=(const VirtualServerManager&);
}; // class VirtualServerManager

} // namespace webserv

#endif // VIRTUALSERVERMANAGER_HPP