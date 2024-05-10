/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServerManager.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 20:51:17 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/09 15:48:16 by hnogared         ###   ########.fr       */
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
		explicit VirtualServerManager(void);

		/* Destructor */
		~VirtualServerManager(void);

		/* Getters */
		Socket								getSocket(void) const;
		const std::vector<const Socket*>	getSockets(void) const;
		const std::vector<VirtualServer*>	&getServers(void) const;
		const std::vector<Client*>			&getIncomingClients(void) const;
		size_t								getSocketsCount(void) const;

		/* Setters */
		void	addServer(VirtualServer *server);
		void	addIncomingClient(Client *client);

		/* Public methods */
		bool	handlesFd(int fd) const;
		void	serveFd(int fd);


	private:
		/* Private attributes */
		Socket						_socket;
		VirtualServer				*_defaultServer;
		std::vector<VirtualServer*>	_servers;
		std::vector<Client*>		_incomingClients;

		/* Private methods */
		static void	_log(Harl::e_level level, const Client *client,
			const std::string &message);
		void	_acceptConnection(void);

		/* [delete] */
		VirtualServerManager(const VirtualServerManager&);
		VirtualServerManager	&operator=(const VirtualServerManager&);
}; // class VirtualServerManager

} // namespace webserv

#endif // VIRTUALSERVERMANAGER_HPP