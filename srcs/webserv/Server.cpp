/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 17:06:34 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/02 17:47:38 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

namespace	webserv
{

/* ************************************************************************** */
/* Static attributes initialization */

bool Server::_running = false;

/* ************************************************************************** */

/* Config path constructor */
Server::Server(const std::string &config_path)
{
	std::vector<Configuration>					*configs;
	std::vector<Configuration>::const_iterator	it;

	if (Server::_running)
		throw std::runtime_error("Another instance is already running");

	{
		std::ifstream	lock_file(WS_LOCK_FILE);

		if (lock_file.is_open())
		{
			lock_file.close();
			throw std::runtime_error("Another instance is already running");
		}
	}

	{
		std::ofstream	lock_file_out(WS_LOCK_FILE);

		if (!lock_file_out.is_open())
			throw std::runtime_error("Failed to create lock file");
		lock_file_out.close();
	}

	Server::_running = true;

	signal(SIGINT, Server::sigHandler);
	signal(SIGQUIT, Server::sigHandler);
	signal(SIGTERM, Server::sigHandler);

	configs = ConfigurationParser::parse(config_path);
	for (it = configs->begin(); it != configs->end(); it++)
		this->_virtualServers.push_back(new VirtualServer(*it));
	delete configs;
}


/* Destructor */
Server::~Server(void)
{
	std::vector<VirtualServer*>::iterator	it = this->_virtualServers.begin();

	while (it != this->_virtualServers.end())
		delete *(it++);
	Server::_running = false;
}


/* ************************************************************************** */
/* Private methods */

/* Method to execute when receiving signals */
void	Server::sigHandler(int signal)
{
	if (signal == SIGINT || signal == SIGQUIT || signal == SIGTERM)
	{
		remove(WS_LOCK_FILE);
		Server::_running = false;

		Harl::complain(Harl::INFO, "Received SIG, stopping...");
	}
}

} // namespace webserv