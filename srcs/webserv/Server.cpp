/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 17:06:34 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/12 04:16:00 by hnogared         ###   ########.fr       */
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
Server::Server(const std::string &configPath) : _logger(WS_LOG_FILE)
{
	if (Server::_initialized)
		throw RuntimeError("Another instance is already initialized", 11);

	{
		std::ifstream	lockFile(WS_LOCK_FILE);

		if (lockFile.is_open())
		{
			lockFile.close();
			throw RuntimeError("Another instance is already initialized", 11);
		}

		std::ofstream	lockFileOut(WS_LOCK_FILE);

		if (!lockFileOut.is_open())
			throw RuntimeError("Failed to create the lock file", 10);
		lockFileOut.close();
	}

	this->_init(configPath);
}


/* Destructor */
Server::~Server(void)
{
	this->_cleanup();
}

/* ************************************************************************** */
/* Public methods */

void	Server::run(void)
{
	int									res;
	std::vector<pollfd>					fds;
	std::vector<pollfd>::const_iterator	pollfdIt;
	std::map<std::pair<
		std::string, int>, VirtualServerManager*>::const_iterator	it;

	Server::_running = true;
	while (Server::_running)
	{
		this->_updateFds(fds);

		res = poll(fds.data(), fds.size(), -1);
		
		if (Server::_running && res == -1)
			throw SocketError("Failed poll: " + std::string(strerror(errno)));
	
		if (!res)
			continue ;

		for (pollfdIt = fds.begin(); pollfdIt != fds.end(); pollfdIt++)
		{
			if (!pollfdIt->revents & POLLIN)
				continue ;

			for (it = this->_managers.begin(); it != this->_managers.end();
				it++)
			{
				if (!(it->second)->handlesFd(pollfdIt->fd))
					continue ;

				it->second->serveFd(pollfdIt->fd);
				break ;
			}
		}
	}
}


/* ************************************************************************** */
/* Private methods */

void	Server::_init(const std::string &configPath)
{
	std::vector<Configuration>	*configs = NULL;

	signal(SIGINT, Server::_sigHandler);
	signal(SIGQUIT, Server::_sigHandler);
	signal(SIGTERM, Server::_sigHandler);

	try
	{
		configs = this->_makeConfigs(configPath);

		this->_initVirtualServerManagers(configs);
		delete configs;
	}
	catch(const std::exception &e)
	{
		delete configs;
		remove(WS_LOCK_FILE);
		throw;
	}

	Server::_initialized = true;
	this->_logger.complain(Harl::INFO, "Webserv up and running...");
	this->_logger.complain(Harl::INFO, "Logging at "
		+ this->_logger.getLogFilePath());
}

void	Server::_initVirtualServerManagers(const std::vector<Configuration>
	*configs)
{
	std::vector<Configuration>::const_iterator			it;
	std::vector<const Configuration*>					specificConfigs;
	std::vector<const Configuration*>::const_iterator	specificIt;

	try
	{
		for (it = configs->begin(); it != configs->end(); it++)
		{
			if (it->getConstAddress().sin_addr.s_addr == INADDR_ANY)
			{
				this->_initVirtualServer(*it);
				continue ;
			}

			specificConfigs.push_back(&(*it));
		}

		for (specificIt = specificConfigs.begin();
				specificIt != specificConfigs.end();
				specificIt++)
			this->_initVirtualServer(**specificIt);
	}
	catch (const std::exception &e)
	{
		this->_cleanup();
		throw;
	}
}

void	Server::_initVirtualServer(const Configuration &config)
{
	std::pair<std::string, int>	key;

	key = std::make_pair(config.getAddressString(), config.getPort());

	if (this->_managers.find(key) == this->_managers.end())
		this->_managers[key] = new VirtualServerManager(&this->_logger);
	
	this->_managers[key]->addServer(new VirtualServer(config, &this->_logger));
}

std::vector<Configuration>	*Server::_makeConfigs(const std::string &configPath)
{
	std::vector<Configuration>	*configs = NULL;

	this->_logger.log(Harl::INFO, "Parsing configuration file: " + configPath);

	try
	{
		configs = ConfigurationParser::parse(configPath);
	}
	catch (const ConfigurationParser::ConfigException &e)
	{
		this->_logger.log(Harl::ERROR, configPath + ": " + e.what());

		if (configPath == WS_DFL_CONFIG_PATH)
		{
			this->_logger.log(Harl::ERROR,
				"Invalid default configuration file: " + configPath + ": "
				+ e.what());
			throw ConfigurationParser::ConfigException(
				"Invalid default configuration file: " + configPath + ": "
				+ e.what());
		}

		this->_logger.log(Harl::INFO, "Fallback to default configuration file");
		return (Server::_makeConfigs(WS_DFL_CONFIG_PATH));
	}
	catch (const std::exception &e)
	{
		this->_logger.log(Harl::ERROR, configPath + ": " + e.what()
			+ ". Abort.");
		throw;
	}
	
	this->_logger.log(Harl::INFO, "Parsed configuration file: " + configPath);

	return (configs);
}

void	Server::_updateFds(std::vector<pollfd> &fds)
{
	size_t	i = 0;
	size_t	size;
	std::vector<const Socket*>::const_iterator	socketIt;
	std::map<std::pair<
		std::string, int>, VirtualServerManager*>::const_iterator	it;

	size = this->_managers.size();
	for (it = this->_managers.begin(); it != this->_managers.end(); it++)
		size += it->second->getSocketsCount();

	if (fds.capacity() != size)
		fds.resize(size);

	for (it = this->_managers.begin(); it != this->_managers.end(); it++)
	{
		const std::vector<const Socket*>	&sockets = it->second->getSockets();

		for (socketIt = sockets.begin(); socketIt != sockets.end(); socketIt++)
		{
			fds[i].fd = (*socketIt)->getFd();
			fds[i].events = POLLIN;
			i++;
		}
	}

}

void	Server::_cleanup(void)
{
	std::map<std::pair<std::string, int>, VirtualServerManager*>::iterator	it;

	for (it = this->_managers.begin(); it != this->_managers.end(); it++)
		delete it->second;

	remove(WS_LOCK_FILE);
	Server::_initialized = false;
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
