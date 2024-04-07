/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 21:49:03 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 17:41:41 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <memory>

#include "Harl.hpp"
#include "Server.hpp"

int	main(int argc, char **argv)
{
	if (argc > 2)
	{
		Harl::complain(Harl::ERROR,
			"Usage: ./webserv [OPT<configuration_file>]");
		return (1);
	}

	try
	{
		std::auto_ptr<webserv::Server> server(new webserv::Server(8080));

		std::cout << "Listening on port " << server->getPort() << "..."
			<< std::endl;

		try
		{
			server->run();
		}
		catch(const std::exception &e)
		{
			Harl::complain(Harl::ERROR, e.what());
			return (3);
		}
	}
	catch(const std::exception &e)
	{
		Harl::complain(Harl::ERROR, e.what());
		return (2);
	}

	(void)argv;
	return (0);
}