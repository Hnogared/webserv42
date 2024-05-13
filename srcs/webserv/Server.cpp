/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 17:06:34 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/13 13:45:02 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

namespace	webserv
{

/* ************************************************************************** */
/* Singleton methods */

/* Unique instance getter */
Server	&Server::getInstance(const std::string &configPath)
{
	static Server	instance(configPath);

	return (instance);
}

/* ************************************************************************** */
/* Private handlers */

/* Config path constructor */
Server::Server(const std::string &configPath)
	: _protocol(WS_HTTP_VERSION), _logger(WS_LOG_FILE)
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

	this->_init(configPath);
}


/* Destructor */
Server::~Server(void)
{
	this->_cleanup();
}


/* ************************************************************************** */
/* Getters */

Harl	&Server::getLogger(void)
{
	return (this->_logger);
}


/* ************************************************************************** */
/* Setters */

void	Server::setForceContinue(bool forceContinue)
{
	this->_forceContinue = forceContinue;
}


/* ************************************************************************** */
/* Public methods */

void	Server::start(void)
{
	int									res;
	std::vector<pollfd>					fds;
	std::vector<pollfd>::const_iterator	pollfdIt;
	std::map<std::pair<
		std::string, int>, VirtualServerManager*>::const_iterator	it;

	this->_running = true;
	this->_forceContinue = false;
	while (this->_running)
	{
		this->_updateFds(fds);

		res = poll(fds.data(), fds.size(), -1);

		if (this->_forceContinue)
		{
			this->_forceContinue = false;
			continue ;
		}

		if (this->_running && res == -1)
			throw SocketError("Failed poll: " + std::string(strerror(errno)));

		for (pollfdIt = fds.begin(); res && pollfdIt != fds.end(); pollfdIt++)
		{
			if (!(pollfdIt->revents & (POLLIN | POLLHUP | POLLERR)))
				continue ;

			for (it = this->_managers.begin(); it != this->_managers.end();it++)
			{
				if (it->second->tryServeFd(pollfdIt->fd, pollfdIt->revents))
					break ;
			}

			res--;
		}
	}
}

void	Server::stop(void)
{
	std::map<std::pair<
		std::string, int>, webserv::VirtualServerManager *>::iterator	it;

	this->_logger.log(Harl::INFO, "STATUS Stopping...");

	for (it = this->_managers.begin(); it != this->_managers.end(); it++)
		it->second->stop();

	this->_running = false;
}

void	Server::reload(void)
{
	this->_logger.log(Harl::INFO, "STATUS Reloading...");
	this->stop();
	this->_cleanup(false);
	this->_init(this->_configPath);
	this->start();
}

void	Server::reopen(void)
{
	this->_logger.reopen();
	this->_logger.log(Harl::INFO, "STATUS Reopened log files");
}


/* ************************************************************************** */
/* Private methods */

void	Server::_init(const std::string &configPath)
{
	signal(SIGPIPE, SIG_IGN);
	signal(SIGUSR1, &Server::_sigHandler);
	signal(SIGINT, &Server::_sigHandler);
	signal(SIGTERM, &Server::_sigHandler);
	signal(SIGQUIT, &Server::_sigHandler);
	signal(SIGHUP, &Server::_sigHandler);

	try
	{
		this->_buildFromConfigFile(configPath);
	}
	catch(const std::exception &e)
	{
		remove(WS_LOCK_FILE);
		throw;
	}

	this->_logger.complain(Harl::INFO, "Webserv up and running");
	this->_logger.complain(Harl::INFO, "Logging at "
		+ this->_logger.getLogFilePath());
	this->_logger.complain(Harl::INFO, "Reload with SIGHUP");
	this->_logger.complain(Harl::INFO, "Stop with SIGINT / SIGTERM / SIGQUIT");
	this->_logger.complain(Harl::INFO, "Reopen log files with SIGUSR1");
	this->_logger.log(Harl::INFO, "STATUS Running...");
}

void	Server::_buildFromConfigFile(const std::string &configPath)
{
	std::vector<Configuration>	*configs = NULL;

	this->_logger.log(Harl::INFO, "Parsing configuration file: " + configPath);

	try
	{
		configs = ConfigurationParser::parse(configPath);
		this->_initVirtualServerManagers(configs);

		this->_logger.log(Harl::INFO, configPath + ": Success");
		this->_configPath = configPath;
		delete configs;
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
		return (this->_buildFromConfigFile(WS_DFL_CONFIG_PATH));
	}
	catch (const std::exception &e)
	{
		this->_logger.log(Harl::ERROR, configPath + ": " + e.what());
		throw;
	}
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
	{
		this->_managers[key] = new VirtualServerManager(this->_protocol,
			&this->_logger);
	}

	this->_managers[key]->addServer(new VirtualServer(config, &this->_logger));
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

void	Server::_cleanup(bool removeLockFile)
{
	std::map<std::pair<std::string, int>, VirtualServerManager*>::iterator	it;

	for (it = this->_managers.begin(); it != this->_managers.end(); it++)
		delete it->second;
	this->_managers.clear();

	if (removeLockFile)
		remove(WS_LOCK_FILE);
}


/* Static public methods */

/* Method to execute when receiving signals */
void	Server::_sigHandler(int signal)
{
	if (signal == SIGUSR1)
	{
		Harl::complain(Harl::INFO, "Received SIGUSR1, reopening log files...");
		Server::getInstance().reopen();
		Server::getInstance().setForceContinue(true);
		return ;
	}
	if (signal == SIGHUP)
	{
		Harl::complain(Harl::INFO, "Received SIGHUP, reloading...");
		Server::getInstance().reload();
		return ;
	}

	Harl::complain(Harl::INFO, "Received stop signal, stopping...");
	Server::getInstance().stop();
}

} // namespace webserv
