/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 21:49:03 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/12 15:41:09 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <memory>

#include "Server.hpp"

int	main(int argc, char **argv)
{
	if (argc > 2)
	{
		Harl::complain(Harl::ERROR,
			"Usage: ./webserv [configuration_file.conf - OPTIONAL]");
		return (1);
	}

	try
	{
		if (argc == 2)
			webserv::Server::getInstance(argv[1]);
		webserv::Server::getInstance().start();
	}
	catch(const webserv::RuntimeError &e)
	{
		Harl::complain(Harl::ERROR, "Fatal error: " + std::string(e.what())
			+ ".\n For detailed logs: " WS_LOG_FILE);
		return (e.code());
	}
	catch(const std::exception &e)
	{
		Harl::complain(Harl::ERROR, "Fatal error: " + std::string(e.what())
			+ ".\n For detailed logs: " WS_LOG_FILE);
		return (2);
	}

	return (0);
}
