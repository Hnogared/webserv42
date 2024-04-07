/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 12:33:54 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 13:15:27 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>

# include <sys/socket.h>
# include <arpa/inet.h>
# include <errno.h>
# include <string.h>
# include <exception>
# include <unistd.h>

class	Server
{
	public:
		/* Constructors */
		explicit Server(int port, int backlog = 10);
		Server(const Server &original);

		/* Destructor */
		~Server();

		/* Operator overloads */
		Server	&operator=(const Server &original);

		/* Getters */
		int					getSockFd(void) const;
		int					getPort(void) const;
		int					getBacklog(void) const;
		struct sockaddr_in	getServerAddress(void) const;

		/* Public methods */
		int acceptConnection();


	private:
		/* Private attributes */
		int					_sock_fd;
		int					_port;
		int					_backlog;
		struct sockaddr_in	_server_address;


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

		class	SocketBindError : public SocketError
		{
			public:
				/* Constructors */
				explicit SocketBindError(void);
				explicit SocketBindError(const std::string &message);
				SocketBindError(const SocketBindError &original);

				/* Destructor */
				~SocketBindError(void) throw();

				/* Operator overloads */
				SocketBindError	&operator=(const SocketBindError &original);
		};

		class	SocketListenError : public SocketError
		{
			public:
				/* Constructors */
				explicit SocketListenError(void);
				explicit SocketListenError(const std::string &message);
				SocketListenError(const SocketListenError &original);

				/* Destructor */
				~SocketListenError(void) throw();

				/* Operator overloads */
				SocketListenError	&operator=(const SocketListenError &original);
		};
};

#endif // SERVER_HPP