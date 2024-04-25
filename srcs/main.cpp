/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 21:49:03 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/24 14:18:02 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <memory>

#include "Harl.hpp"
#include "VirtualServer.hpp"
#include "ConfigurationParser.hpp"

int	main(int argc, char **argv)
{
	webserv::ConfigurationParser::parse("my_config.config");
	return (0);
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