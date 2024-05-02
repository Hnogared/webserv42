/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 21:49:03 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/02 16:54:32 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <memory>

#include "Harl.hpp"
#include "VirtualServer.hpp"
#include "ConfigurationParser.hpp"

int	main()
{
	std::vector<webserv::Configuration>	*configs;

	configs = webserv::ConfigurationParser::parse("small_config.conf");
	for (size_t i = 0; i < configs->size(); i++)
		std::cout << (*configs)[i] << "\n" << std::endl;
	
	delete configs;
	return (0);
}

/*
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
		std::auto_ptr<webserv::VirtualServer> server(new webserv::VirtualServer(8080));

		std::cout << "Listening on port " << server->getPort() << "..."
			<< std::endl;

		try
		{
			server->run();
		}
		catch(const webserv::RuntimeError &e)
		{
			Harl::complain(Harl::ERROR, e.what());
			return (e.code());
		}
		catch(const std::exception &e)
		{
			Harl::complain(Harl::ERROR, e.what());
			return (2);
		}
	}
	catch(const webserv::RuntimeError &e)
	{
		Harl::complain(Harl::ERROR, e.what());
		return (e.code());
	}
	catch(const std::exception &e)
	{
		Harl::complain(Harl::ERROR, e.what());
		return (3);
	}

	(void)argv;
	return (0);
}
*/