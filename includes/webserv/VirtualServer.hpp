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

# include <sys/socket.h>
# include <arpa/inet.h>
# include <string.h>
# include <exception>
# include <unistd.h>
# include <poll.h>
# include <signal.h>

# include "exceptions.hpp"
# include "Harl.hpp"
# include "Socket.hpp"
# include "Client.hpp"
# include "HttpRequest.hpp"

# define LOCK_FILE	"/tmp/webserv42.lock"

namespace	webserv
{

class	VirtualServer
{
	public:
		/* Constructors */
		explicit VirtualServer(int port, int backlog = 10);

		/* Destructor */
		~VirtualServer(void);

		/* Getters */
		bool				isRunning(void) const;
		Socket				getSocket(void) const;
		int					getPort(void) const;
		int					getBacklog(void) const;
		struct sockaddr_in	getVirtualServerAddress(void) const;

		/* Public methods */
		void	run(void);
		void	acceptConnection(void);
		void	handleRequest(const Client &client);


	private:
		/* Private static attributes */
		static bool					running;

		/* Private attributes */
		Socket				_socket;
		int					_port;
		int					_backlog;
		struct sockaddr_in	_server_address;
		std::vector<Client>	_clients;

		/* Private static methods */
		static void	sigHandler(int signal);

		/* [delete] Copy constructor */
		VirtualServer(const VirtualServer &original);

		/* [delete] Copy assignment operator */
		VirtualServer	&operator=(const VirtualServer &original);
};

} // namespace webserv

#endif // VIRTUALSERVER_HPP