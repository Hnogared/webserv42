/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 12:33:54 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 19:18:48 by hnogared         ###   ########.fr       */
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

# include "Harl.hpp"
# include "Socket.hpp"
# include "Client.hpp"

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
		webserv::Socket		getSocket(void) const;
		int					getPort(void) const;
		int					getBacklog(void) const;
		struct sockaddr_in	getServerAddress(void) const;

		/* Public methods */
		void	run(void);
		void	acceptConnection(void);


	private:
		/* Private static attributes */
		static const std::string	CLASS_NAME;
		static bool					running;

		/* Private attributes */
		webserv::Socket		_socket;
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


	/* Exceptions */
	public:
		class	SocketError : public std::exception
		{
			public:
				/* Constructors */
				explicit SocketError(void);
				explicit SocketError(const std::string &message);
				SocketError(const SocketError &original);

				/* Destructor */
				~SocketError(void) throw();

				/* Operator overloads */
				SocketError	&operator=(const SocketError &original);

				/* std::exception method overrides */
				virtual const char *what(void) const throw();


				private:
					std::string	_msg;
		};

		class	SocketCreationError : public SocketError
		{
			public:
				/* Constructors */
				explicit SocketCreationError(void);
				explicit SocketCreationError(const std::string &message);
				SocketCreationError(const SocketCreationError &original);

				/* Destructor */
				~SocketCreationError(void) throw();

				/* Operator overloads */
				SocketCreationError	&operator=(const SocketCreationError
						&original);
		};

		class	SocketConnectionError : public SocketError
		{
			public:
				/* Constructors */
				explicit SocketConnectionError(void);
				explicit SocketConnectionError(const std::string &message);
				SocketConnectionError(const SocketConnectionError &original);

				/* Destructor */
				~SocketConnectionError(void) throw();

				/* Operator overloads */
				SocketConnectionError	&operator=(const SocketConnectionError
						&original);
		};
};

} // namespace webserv

#endif // SERVER_HPP