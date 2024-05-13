/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 14:42:17 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/13 11:51:30 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <exception>
# include <iostream>
# include <cerrno>
# include <sstream>
# include <string>

# include <string.h>
# include <sys/socket.h>

# include "webserv.hpp"
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

		/* Operator overloads */
		Client	&operator=(const Client &original);
		bool	operator==(const Client &other) const;

		/* Getters */
		int						getSocketFd(void) const;
		Socket					getSocket(void) const;
		const Socket			*getSocketPtr(void) const;
		std::string				getAddrStr(e_addr_choice choice) const;
		bool					isRequestPending(void) const;
		const http::HttpRequest	&getRequest(void) const;
		const std::string		&getBuffer(void) const;

		/* Public methods */
		void	sendResponse(const http::HttpResponse &response);
		void	fetchRequestLineAndHeaders(const http::Protocol &protocol);
		void	fetchRequestBody(size_t maxBodyLen);


	private:
		/* Private attributes */
		Socket				_socket;
		bool				_requestPending;
		http::HttpRequest	_request;
		std::string			_buffer;

		/* Private methods */
		std::string	_readRequestBlock(size_t maxBuffSize = 0) const;
}; // class Client

} // namespace webserv

#endif // CLIENT_HPP