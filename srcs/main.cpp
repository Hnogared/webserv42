/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 21:49:03 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 12:01:37 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Harl.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

int	main(int argc, char **argv)
{
	if (argc > 2)
	{
		Harl::complain(Harl::ERROR, "Usage: ./webserv [configuration_file]");
		return (1);
	}

	{
		int					port = 8080;
		int					backlog = 10;
		int					sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		int					client_fd;
		struct sockaddr_in	server_address;
		struct sockaddr_in	client_address;

		if (sock_fd < 0)
		{
			Harl::complain(Harl::ERROR,
				"Failed to create socket: " + std::string(strerror(errno)));
			return (2);
		}

		server_address.sin_family = AF_INET;
		server_address.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on all interfaces
		server_address.sin_port = htons(port);
		if (bind(sock_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
		{
			Harl::complain(Harl::ERROR,
				"Failed to bind socket: " + std::string(strerror(errno)));
			return (3);
		}

		std::clog << "Listening on port " << port << "..." << std::endl;

		if (listen(sock_fd, backlog) == -1)
		{
			Harl::complain(Harl::ERROR,
				"Failed to listen on socket: " + std::string(strerror(errno)));
			return (4);
		}

		socklen_t client_address_len = sizeof(client_address);
		client_fd = accept(sock_fd, (struct sockaddr *)&client_address, &client_address_len);
		if (client_fd == -1)
		{
			Harl::complain(Harl::ERROR,
				"Failed to accept connection" + std::string(strerror(errno)));
			return (5);
		}

		std::clog << "Accepted connection from " << inet_ntoa(client_address.sin_addr) << std::endl;
		(void)client_fd;
	}


	(void)argv;
	return (0);
}
