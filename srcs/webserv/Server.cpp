/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 17:06:34 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/08 22:05:47 by hnogared         ###   ########.fr       */
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
Server::Server(const std::string &configPath)
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

	this->_init(configPath);
}


/* Destructor */
Server::~Server(void)
{
	std::map<std::pair<std::string, int>, VirtualServerManager*>::iterator	it;

	for (it = this->_managers.begin(); it != this->_managers.end(); it++)
		delete it->second;

	remove(WS_LOCK_FILE);
	Server::_initialized = false;
}

/* ************************************************************************** */
/* Public methods */

/*
void	Server::run(void)
{
	int					fdId;
	int					serverId;
	int					res;
	int					totalFdsCount;
	std::vector<pollfd>	fds;
	std::vector<size_t>	clientsCounts(this->_virtualServers.size(), 0);
	std::vector<size_t>::const_iterator	it;

	Server::_running = true;
	while (Server::_running)
	{
		this->_updateClientsCounts(clientsCounts);
		this->_updateVServersFds(fds, clientsCounts);

		totalFdsCount = clientsCounts.size();
		for (size_t i = 0; i < clientsCounts.size(); i++)
			totalFdsCount += clientsCounts[i];

		res = poll(fds.data(), totalFdsCount, -1);

		if (Server::_running && res == -1)
			throw SocketError("Failed poll: " + std::string(strerror(errno)));

		if (res)
		{
			for (size_t i = 0; res && i < clientsCounts.size(); i++)
			{
				if (!fds[i].revents & POLLIN)
					continue ;
				res--;
				this->_virtualServers[i]->acceptConnection();
			}

			fdId = clientsCounts.size();
			serverId = 0;
			for (it = clientsCounts.begin(); res && it != clientsCounts.end();
				it++)
			{
				for (size_t i = 0; res && i < *it; i++)
				{
					if (!fds[fdId + i].revents & POLLIN)
						continue ;
					res--;
					this->_virtualServers[serverId]->handleRequest(
						this->_virtualServers[serverId]->getClients()[i]);
				}
				fdId += *it;
				serverId++;
			}
		}
	}
}
*/


/* ************************************************************************** */
/* Private methods */

void	Server::_init(const std::string &configPath)
{
	std::vector<Configuration>					*configs = NULL;
	std::vector<Configuration>::const_iterator	it;

	signal(SIGINT, Server::_sigHandler);
	signal(SIGQUIT, Server::_sigHandler);
	signal(SIGTERM, Server::_sigHandler);

	try
	{
		configs = Server::_makeConfigs(configPath);

		for (it = configs->begin(); it != configs->end(); it++)
			this->_initVirtualServer(*it);
		delete configs;
	}
	catch(const std::exception &e)
	{
		delete configs;
		remove(WS_LOCK_FILE);
		throw;
	}

	Server::_initialized = true;
}

void	Server::_initVirtualServer(const Configuration &config)
{
	VirtualServer				*server = new VirtualServer(config);
	std::pair<std::string, int>	key;

	if (!server)
		throw std::bad_alloc();

	key = std::make_pair(config.getAddressString(), config.getPort());

	if (this->_managers.find(key) == this->_managers.end())
	{
		VirtualServerManager* manager = new VirtualServerManager();

		if (!manager)
		{
			delete server;
			throw std::bad_alloc();
		}

		this->_managers[key] = manager;
	}

	this->_managers[key]->addServer(server);
}

/*
void	Server::_updateClientsCounts(std::vector<size_t> &clientsCounts) const
{
	size_t	serversCount = this->_virtualServers.size();

	if (clientsCounts.size() != serversCount)
		clientsCounts.resize(serversCount, 0);

	for (size_t i = 0; i < serversCount; i++)
		clientsCounts[i] = (this->_virtualServers[i])->getClients().size();
}

void	Server::_updateVServersFds(std::vector<pollfd> &fds,
	const std::vector<size_t> &clientsCounts) const
{
	size_t	i, j, fdId;
	size_t	size;

	size = clientsCounts.size();
	i = size;
	while (i)
		size += clientsCounts[i-- - 1];

	if (fds.capacity() != size)
		fds.resize(size);

	for (i = 0; i < this->_virtualServers.size(); i++)
	{
		fds[i].fd = (this->_virtualServers[i])->getSocket().getFd();
		fds[i].events = POLLIN;
	}

	fdId = i;
	for (i = 0; i < clientsCounts.size(); i++)
	{
		for (j = 0; j < clientsCounts[i]; j++)
		{
			fds[fdId].fd
				= (this->_virtualServers[i])->getClients()[j].getSocketFd();
			fds[fdId++].events = POLLIN;
		}
	}
}
*/

std::vector<Configuration>	*Server::_makeConfigs(const std::string &configPath)
{
	std::vector<Configuration>	*configs = NULL;

	Harl::complain(Harl::INFO, "Parsing configuration file: " + configPath);

	try
	{
		configs = ConfigurationParser::parse(configPath);
	}
	catch (const ConfigurationParser::ConfigException &e)
	{
		Harl::complain(Harl::ERROR, configPath + ": " + e.what());

		if (configPath == WS_DFL_CONFIG_PATH)
		{
			throw ConfigurationParser::ConfigException(
				"Invalid default configuration file: " + configPath + ": "
				+ e.what());
		}

		Harl::complain(Harl::INFO, "Fall back to default configuration file");
		return (Server::_makeConfigs(WS_DFL_CONFIG_PATH));
	}
	catch (const std::exception &e)
	{
		Harl::complain(Harl::ERROR, configPath + ": " + e.what() + ". Abort.");
		throw;
	}
	
	Harl::complain(Harl::INFO, "Parsed configuration file: " + configPath);

	return (configs);
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
