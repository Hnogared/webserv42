/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 02:08:16 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/13 12:12:39 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VirtualServer.hpp"

namespace	webserv
{

/* ************************************************************************** */

/* Configuration constructor */
VirtualServer::VirtualServer(const Configuration &config, Harl *logger)
	: _config(config), _logger(logger) {}


/* Destructor */
VirtualServer::~VirtualServer(void) {}


/* ************************************************************************** */
/* Getters */

const Configuration	&VirtualServer::getConfiguration(void) const
{
	return (this->_config);
}


/* ************************************************************************** */
/* Public methods */

bool	VirtualServer::tryHandleClientRequest(Client &client)
{
	std::string						uri = client.getRequest().getTarget();
	const LocationConfiguration*	bestLocation = NULL;

	bestLocation = this->_config.findBestLocation(uri);

	if (!bestLocation)
	{
		throw http::HttpRequest::RequestException(
			client.getRequest().getStatusLine(), 505);
	}

	if (*(uri.end() - 1) == '/')
		return (this->_tryDirectoryResponse(client, *bestLocation));


	//client.fetchRequestBody(this->_config.getClientMaxBodySize());

	return (false);
}


/* ************************************************************************** */
/* Private methods */

void	VirtualServer::_log(Harl::e_level level, const Client *client,
	const std::string &message) const
{
	std::string	statusLine;
	std::string	logMessage;

	if (!client)
	{
		if (this->_logger)
			this->_logger->log(level, message);
		else
			Harl::complain(level, message);
		return ;
	}

	logMessage = client->getAddrStr(Client::PEER);
	statusLine = client->getRequest().getStatusLine();

	if (!statusLine.empty())
		logMessage += " REQ '" + client->getRequest().getStatusLine() + "'";
		
	logMessage += " " + message;

	if (this->_logger)
		this->_logger->log(level, logMessage);
	else
		Harl::complain(level, logMessage);
}

bool	VirtualServer::_tryDirectoryResponse(Client &client,
	const LocationConfiguration &location)
{
	std::string	uri = client.getRequest().getTarget();
	std::string	path;
	std::string	fileStr;

	if (!location.getIndex().empty())
	{
		path = tool::files::joinPaths(location.getRoot(), uri);
		path = tool::files::joinPaths(path, location.getIndex());

		try
		{
			http::HttpResponse	response(200);

			response.setBody(tool::files::readFile(path));
			this->_log(Harl::INFO, &client, "200");
			client.sendResponse(response);
			return (true);
		}
		catch(const std::exception& /* e */) {}
	}

	if (location.isAutoindex())
	{
		path = tool::files::joinPaths(location.getRoot(), uri);
		return (this->_tryDirectoryListing(client, uri, path));
	}

	return (false);
}

bool	VirtualServer::_tryDirectoryListing(Client &client,
	const std::string &uri, const std::string &path)
{
	std::string		icon;
	std::string		body;
	DIR				*dir;
	struct dirent	*entry;

	dir = opendir(path.c_str());
	if (!dir)
		return (false);

	body = "<html>\n"
		"<head><title>Index of " + uri + "</title></head>\n"
		"<body>\n"
		"  <h1>Index of " + uri + "</h1>\n"
		"  <hr><pre>\n";

	while ((entry = readdir(dir)))
	{
		switch (entry->d_type)
		{
			case DT_REG:
				icon = "&#128196;";
				break;
			default:
				icon = "&#128193;";
				break;
		}
		body += "  <span>" + icon + " " + "</span>"
			+ "<a href=\"" + uri + entry->d_name + "\">"
			+ entry->d_name + "</a><br>\n";
	}

	body += "  </pre><hr>\n"
		"</body>\n"
		"</html>\n";

	closedir(dir);

	http::HttpResponse	response(200);

	response.setBody(body);
	this->_log(Harl::INFO, &client, "200");
	client.sendResponse(response);

	return (true);
}

} // namespace webserv