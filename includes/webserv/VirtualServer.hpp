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
# include <dirent.h>

# include "exceptions.hpp"
# include "files.hpp"
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
		explicit VirtualServer(const Configuration &config);

		/* Destructor */
		~VirtualServer(void);

		/* Getters */
		const Configuration			&getConfiguration(void) const;
		const std::vector<Client*>	&getClients(void) const;

		/* Setters */
		void	addClient(Client *client);

		/* Public methods */
		void	update(void);
		void	acceptConnection(void);
		void	handleRequest(const Client &client);


	private:
		/* Private attributes */
		Configuration			_config;
		std::vector<Client*>	_clients;

		/* Private methods */
		void	_log(Harl::e_level level, const Client *client,
			const std::string &message);
		void	_sendResponse(const Client &client,
			const http::HttpRequest &request);
		void	_sendDirectoryResponse(const Client &client,
			const http::HttpRequest &request);
		void	_sendDirectoryListing(const Client &client,
			const std::string &uri, const std::string &path);

		/* [delete] */
		VirtualServer(void);
		VirtualServer(const VirtualServer&);
		VirtualServer	&operator=(const VirtualServer&);
}; // class VirtualServer

} // namespace webserv

#endif // VIRTUALSERVER_HPP