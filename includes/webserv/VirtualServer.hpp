/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualVirtualServer.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 12:33:54 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/24 10:47:09 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VIRTUALSERVER_HPP
# define VIRTUALSERVER_HPP

# include <string>
# include <vector>
# include <fstream>
# include <utility>

# include <cerrno>
# include <cstring>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <exception>
# include <unistd.h>
# include <poll.h>
# include <signal.h>

# include "exceptions.hpp"
# include "Harl.hpp"
# include "Socket.hpp"
# include "Client.hpp"
# include "HttpRequest.hpp"
# include "Configuration.hpp"

namespace	webserv
{

class	VirtualServer
{
	public:
		/* Constructors */
		explicit VirtualServer(const Configuration &config, int backlog = 10);

		/* Destructor */
		~VirtualServer(void);

		/* Getters */
		Socket				getSocket(void) const;
		int					getPort(void) const;
		int					getBacklog(void) const;
		struct sockaddr_in	getVirtualServerAddress(void) const;
		const Configuration	&getConfiguration(void) const;

		/* Public methods */
		void	acceptConnection(void);
		void	handleRequest(const Client &client);


	private:
		/* Private attributes */
		Configuration		_config;
		Socket				_socket;
		int					_backlog;
		std::vector<Client>	_clients;

		/* [delete] */
		VirtualServer(const VirtualServer&);
		VirtualServer	&operator=(const VirtualServer&);
};

} // namespace webserv

#endif // VIRTUALSERVER_HPP