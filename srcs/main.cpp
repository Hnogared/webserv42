/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 21:49:03 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/06 11:25:19 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <memory>

#include "Server.hpp"

int	main(int argc, char **argv)
{
	webserv::Server	*server_p = NULL;

	if (argc > 2)
	{
		Harl::complain(Harl::ERROR,
			"Usage: ./webserv [configuration_file.conf - OPTIONAL]");
		return (1);
	}

	try
	{
		if (argc == 2)
			server_p = new webserv::Server(argv[1]);
		else
			server_p = new webserv::Server();

		if (server_p)
		{
			server_p->run();
			delete server_p;
		}
	}
	catch(const webserv::RuntimeError &e)
	{
		delete server_p;
		return (e.code());
	}
	catch(const std::exception &e)
	{
		delete server_p;
		return (2);
	}

	return (0);
}