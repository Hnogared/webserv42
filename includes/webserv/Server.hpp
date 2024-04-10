/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 12:33:54 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/08 19:44:44 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

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

class	Server
{
	public:
		/* Constructors */
		explicit Server(int port, int backlog = 10);

		/* Destructor */
		~Server(void);

		/* Getters */
		bool				isRunning(void) const;
		Socket				getSocket(void) const;
		int					getPort(void) const;
		int					getBacklog(void) const;
		struct sockaddr_in	getServerAddress(void) const;

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
		Server(const Server &original);

		/* [delete] Copy assignment operator */
		Server	&operator=(const Server &original);
};

} // namespace webserv

#endif // SERVER_HPP