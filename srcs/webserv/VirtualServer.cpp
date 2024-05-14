/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 02:08:16 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/14 19:54:11 by hnogared         ###   ########.fr       */
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
		return (false);

	if (*(uri.end() - 1) == '/')
		return (this->_tryDirectoryResponse(client, *bestLocation));

	if (this->_tryFileResponse(client, *bestLocation))
		return (true);

	return (this->_tryDirectoryResponse(client, *bestLocation));
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

bool	VirtualServer::_tryFileResponse(Client &client,
	const LocationConfiguration &location)
{
	std::string	uri = client.getRequest().getTarget();
	std::string	path;

	path = tool::files::joinPaths(location.getRoot(), uri);

	try
	{
		http::HttpResponse	response(200);

		response.setBody(tool::files::readFile(path),
			http::HttpMessage::getMimeType(path));
		this->_log(Harl::INFO, &client, "200");
		client.sendResponse(response);

		return (true);
	}
	catch (const FileException &e)
	{
		if (e.code() == EACCES)
			throw http::HttpRequest::RequestException("Forbidden", 403);
		if (e.code() != ENOENT && e.code() != EISDIR)
			throw;

		return (false);
	}
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

			response.setBody(tool::files::readFile(path),
				http::HttpMessage::TEXT_HTML);
			this->_log(Harl::INFO, &client, "200");
			client.sendResponse(response);
			return (true);
		}
		catch (const FileException &e)
		{
			if (e.code() == EACCES)
				throw http::HttpRequest::RequestException("Forbidden", 403);
			if (e.code() != ENOENT && e.code() != EISDIR)
				throw;
		}
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
	struct stat							statbuf;
	std::string							icon;
	std::string							size;
	std::string							body;
	std::vector<dirent>					entries;
	std::vector<dirent>::const_iterator	it;

	try
	{
		entries = tool::files::readDirectory(path);
	}
	catch (const RuntimeError &e)
	{
		if (e.code() == EACCES)
			throw http::HttpRequest::RequestException("Forbidden", 403);
		if (e.code() != ENOENT && e.code() != ENOTDIR)
			throw;

		return (false);
	}

	body = "<html>\n"
		"<head><title>Index of " + uri + "</title></head>\n\n"
		"<body>\n"
		"  <h1>Index of " + uri + "</h1>\n"
		"  <hr><pre>\n"
		"  <table>\n";

	for (it = entries.begin(); it != entries.end(); it++)
	{
		if ((it->d_name[0] == '.' && it->d_name[1] == 0))
			continue ;

		if (stat(it->d_name, &statbuf) == 0)
			size = tool::strings::toStr(statbuf.st_size);
		else
			size = "-";

		if (it->d_type == DT_DIR)
			icon = "&#128193;";
		else if (it->d_type == DT_LNK)
			icon = "&#128279;";
		else
			icon = "&#128196;";

		body += "    <tr>\n"
			"      <td>" + icon + "</td>\n"
			"      <td style=\"padding-right: 100\"><a href=\""
				+ tool::files::joinPaths(uri, it->d_name)
				+ "\">\n" + it->d_name + "</a></td>\n"
			"      <td>" + size + "<td>\n"
			"    </tr>\n";
	}

	body += "  <table>\n"
		"  </pre><hr>\n"
		"</body>\n"
		"</html>\n";

	http::HttpResponse	response(200);

	response.setBody(body, http::HttpMessage::TEXT_HTML);
	this->_log(Harl::INFO, &client, "200");
	client.sendResponse(response);

	return (true);
}

} // namespace webserv