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
# include <sys/stat.h>
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
		explicit VirtualServer(const Configuration &config, Harl *loggr = NULL);

		/* Destructor */
		~VirtualServer(void);

		/* Getters */
		const Configuration	&getConfiguration(void) const;

		/* Public methods */
		bool	tryHandleClientRequest(Client &client);


	private:
		/* Private attributes */
		Configuration	_config;
		Harl			*_logger;

		/* Private methods */
		void	_log(Harl::e_level level, const Client *client,
			const std::string &message) const;
		bool	_tryFileResponse(Client &client,
			const LocationConfiguration &location);
		bool	_tryDirectoryResponse(Client &client,
			const LocationConfiguration &location);
		bool	_tryDirectoryListing(Client &client, const std::string &uri,
			const std::string &path);

		static const std::vector<const dirent*>	_readDirectory(
			const std::string &path);

		/* [delete] */
		VirtualServer(void);
		VirtualServer(const VirtualServer&);
		VirtualServer	&operator=(const VirtualServer&);
}; // class VirtualServer

} // namespace webserv

#endif // VIRTUALSERVER_HPP