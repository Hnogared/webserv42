/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 17:06:34 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/03 12:25:20 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

namespace	webserv
{
/* ************************************************************************** */
/* Static attributes initialization */

bool	Server::_initialized = false;
bool	Server::_running = false;

/* ************************************************************************** */

/* Config path constructor */
Server::Server(const std::string &config_path)
{
	if (Server::_initialized)
		throw RuntimeError("Another instance is already initialized", 11);

	{
		std::ifstream	lock_file(WS_LOCK_FILE);

		if (lock_file.is_open())
		{
			lock_file.close();
			throw RuntimeError("Another instance is already initialized", 11);
		}

		std::ofstream	lock_file_out(WS_LOCK_FILE);

		if (!lock_file_out.is_open())
			throw RuntimeError("Failed to create the lock file", 10);
		lock_file_out.close();
	}

	this->_init(config_path);
}


/* Destructor */
Server::~Server(void)
{
	std::vector<VirtualServer*>::iterator	it = this->_virtualServers.begin();

	while (it != this->_virtualServers.end())
		delete *(it++);
	remove(WS_LOCK_FILE);
	Server::_initialized = false;
}

/* ************************************************************************** */
/* Public methods */

void	Server::run(void)
{
	Server::_running = true;
	while (Server::_running) ;
}


/* ************************************************************************** */
/* Private methods */

void	Server::_init(const std::string &config_path)
{
	std::vector<Configuration>					*configs;
	std::vector<Configuration>::const_iterator	it;

	signal(SIGINT, Server::_sigHandler);
	signal(SIGQUIT, Server::_sigHandler);
	signal(SIGTERM, Server::_sigHandler);

	try
	{
		configs = ConfigurationParser::parse(config_path);

		for (it = configs->begin(); it != configs->end(); it++)
			this->_virtualServers.push_back(new VirtualServer(*it));

		delete configs;
	}
	catch(const std::exception& e)
	{
		delete configs;
		throw;
	}

	Server::_initialized = true;
}

/* Method to execute when receiving signals */
void	Server::_sigHandler(int signal)
{
	if (signal == SIGINT || signal == SIGQUIT || signal == SIGTERM)
	{
		Server::_running = false;
		Harl::complain(Harl::INFO, "Received SIG, stopping...");
	}
}

} // namespace webserv