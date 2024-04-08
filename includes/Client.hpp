/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 14:42:17 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/08 19:15:41 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <exception>
# include <iostream>
# include <cerrno>
# include <sstream>

# include <string.h>
# include <sys/socket.h>

# include "net.hpp"
# include "exceptions.hpp"
# include "Harl.hpp"
# include "Socket.hpp"
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

namespace	webserv
{

class	Client
{
	public:
		enum	e_addr_choice
		{
			LOCAL,
			PEER
		};

		/* Constructors */
		explicit Client(const Socket &socket);
		Client(const Client &original);

		/* Destructor */
		~Client(void);

		/* Getters */
		int			getSocketFd(void) const;
		Socket		getSocket(void) const;
		std::string	getAddrStr(e_addr_choice choice) const;

		/* Public methods */
		void	sendResponse(const http::HttpResponse &response);
		void	fetchRequest(void);
		void	parseRequest(const std::string &request);


	private:
		/* Private attributes */
		Socket	_socket;

		/* [delete] Copy assignment operator overload */
		Client	&operator=(const Client &original);
};

} // namespace webserv

#endif // CLIENT_HPP