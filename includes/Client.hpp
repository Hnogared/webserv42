/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 14:42:17 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 19:32:20 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <exception>
# include <iostream>
# include <cerrno>

# include <string.h>
# include <sys/socket.h>

# include "Socket.hpp"

namespace	webserv
{

class	Client
{
	public:
		/* Constructors */
		explicit Client(int sock_fd);
		Client(const Client &original);

		/* Destructor */
		~Client(void);

		/* Getters */
		int				getSocketFd(void) const;
		webserv::Socket	getSocket(void) const;

		/* Public methods */
		void	receiveData(void);


	private:
		/* Private static atrtibutes */
		static const std::string	CLASS_NAME;

		/* Private attributes */
		webserv::Socket	_socket;

		/* [delete] Copy assignment operator overload */
		Client	&operator=(const Client &original);
};

} // namespace webserv

#endif // CLIENT_HPP