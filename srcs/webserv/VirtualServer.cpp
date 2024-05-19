/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 02:08:16 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/19 22:34:33 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VirtualServer.hpp"

namespace webserv
{

/* ************************************************************************** */

/* Configuration constructor */
VirtualServer::VirtualServer(const Configuration &config, Harl *logger)
    : _config(config), _logger(logger)
{
}

/* Destructor */
VirtualServer::~VirtualServer(void) {}

/* ************************************************************************** */
/* Getters */

const Configuration &VirtualServer::getConfiguration(void) const
{
    return (this->_config);
}

/* ************************************************************************** */
/* Public methods */

bool VirtualServer::tryHandleClientRequest(Client &client, bool lastTry)
{
    try
    {
        const http::HttpRequest &request = client.getRequest();
        const LocationConfiguration *bestLocation = NULL;

        if (!this->_checkServerNames(request.getHeader("Host")) ||
            !(bestLocation = this->_config.findBestLocation(request.getUri())))
        {
            if (lastTry)
                throw http::HttpRequest::RequestException("Not found", 404);
            return (false);
        }

        if (!bestLocation->ismethodAllowed(request.getMethod()))
            throw http::HttpRequest::RequestException("Method not allowed",
                                                      405);

        if (this->_tryResponse(client, *bestLocation)) return (true);

        if (lastTry)
            throw http::HttpRequest::RequestException("Not found", 404);

        return (false);
    }
    catch (const http::HttpRequest::RequestException &e)
    {
        this->_log(Harl::INFO, &client,
                   tool::strings::toStr(e.code()) + " (" +
                       std::string(e.what()) + ")");
        this->_sendErrorResponse(client, e.code());
        throw;
    }
    catch (const std::exception &e)
    {
        this->_log(Harl::ERROR, &client,
                   "500 (Internal server error) - " + std::string(e.what()));
        this->_sendErrorResponse(client, 500);
        throw;
    }
}

/* ************************************************************************** */
/* Private methods */

bool VirtualServer::_checkServerNames(const std::string &host) const
{
    size_t colonPos = host.find(':');
    std::string hostName = host;
    const std::set<std::string> &serverNames = this->_config.getServerNames();

    if (colonPos != std::string::npos) hostName = host.substr(0, colonPos);

    return (serverNames.empty() ||
            serverNames.find(hostName) != serverNames.end());
}

const LocationConfiguration *VirtualServer::_findBestLocation(
    const std::string &uri, const LocationConfiguration **bestLocation) const
{
    if (!bestLocation) return (NULL);

    *bestLocation = this->_config.findBestLocation(uri);
    return (*bestLocation);
}

bool VirtualServer::_tryResponse(Client &client,
                                 const LocationConfiguration &location)
{
    const http::HttpRequest &request = client.getRequest();
    const http::HttpRequest::e_method method = request.getMethod();

    switch (method)
    {
        case http::HttpRequest::GET:
        case http::HttpRequest::HEAD:
            return (this->_tryGetOrHeadResponse(client, location));
        case http::HttpRequest::POST:
        case http::HttpRequest::PUT:
            return (this->_tryPostOrPutResponse(client, location));
        case http::HttpRequest::DELETE:
            return (this->_tryDeleteResponse(client, location));
        default:
            throw http::HttpRequest::RequestException("Method not allowed",
                                                      405);
    }
}

bool VirtualServer::_tryGetOrHeadResponse(Client &client,
                                          const LocationConfiguration &location)
{
    const std::string &uri = client.getRequest().getUri();

    if (*(uri.end() - 1) == '/')
        return (this->_tryDirectoryResponse(uri, client, location));

    if (this->_tryFileResponse(uri, client, location) ||
        this->_tryDirectoryResponse(uri, client, location))
        return (true);

    return (false);
}

bool VirtualServer::_tryPostOrPutResponse(Client &client,
                                          const LocationConfiguration &location)
{
    const std::string &uri = client.getRequest().getUri();

    client.fetchRequestBody(this->_config.getClientMaxBodySize());

    if (*(uri.end() - 1) == '/')
        throw http::HttpRequest::RequestException("Forbidden", 403);

    const std::string path = tool::files::joinPaths(location.getRoot(), uri);
    const bool fileExisted = tool::files::fileExists(path);
    std::ofstream file;

    if (client.getRequest().getMethod() == http::HttpRequest::PUT)
        file.open(path.c_str(), std::ios::binary | std::ios::trunc);
    else
        file.open(path.c_str(), std::ios::binary | std::ios::app);

    if (!file)
    {
        if (errno == EACCES || errno == EISDIR)
            throw http::HttpRequest::RequestException("Forbidden", 403);
        throw http::HttpRequest::RequestException("Internal Server Error", 500);
    }

    file << client.getRequest().getBody();
    file.close();

    this->_log(Harl::INFO, &client, "20" + tool::strings::toStr(!fileExisted));
    client.sendResponse(
        http::HttpResponse(200 + !fileExisted, client.getRequest()));

    return (true);
}

bool VirtualServer::_tryDeleteResponse(Client &client,
                                       const LocationConfiguration &location)
{
    const std::string &uri = client.getRequest().getUri();

    if (*(uri.end() - 1) == '/')
        throw http::HttpRequest::RequestException("Forbidden", 403);

    const std::string path = tool::files::joinPaths(location.getRoot(), uri);

    if (std::remove(path.c_str()) == -1)
    {
        if (errno == ENOENT) return (false);

        if (errno == EACCES)
            throw http::HttpRequest::RequestException("Forbidden", 403);

        throw http::HttpRequest::RequestException("Internal Server Error", 500);
    }

    this->_log(Harl::INFO, &client, "204");
    client.sendResponse(http::HttpResponse(204, client.getRequest()));

    return (true);
}

bool VirtualServer::_tryFileResponse(const std::string &uri, Client &client,
                                     const LocationConfiguration &location)
{
    std::string path = tool::files::joinPaths(location.getRoot(), uri);

    try
    {
        http::HttpResponse response(200, client.getRequest());

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
        if (e.code() != ENOENT && e.code() != EISDIR && e.code() != ENOTDIR)
            throw;

        return (false);
    }
}

bool VirtualServer::_tryDirectoryResponse(const std::string &uri,
                                          Client &client,
                                          const LocationConfiguration &location)
{
    std::string path;
    std::string fileStr;

    if (!location.getIndex().empty())
    {
        path = tool::files::joinPaths(location.getRoot(), uri);
        path = tool::files::joinPaths(path, location.getIndex());

        try
        {
            http::HttpResponse response(200, client.getRequest());

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
            if (e.code() != ENOENT && e.code() != EISDIR && e.code() != ENOTDIR)
                throw;
        }
    }

    if (location.isAutoindex())
    {
        path = tool::files::joinPaths(location.getRoot(), uri);
        return (this->_tryDirectoryListing(uri, path, client));
    }

    return (false);
}

bool VirtualServer::_tryDirectoryListing(const std::string &uri,
                                         const std::string &path,
                                         Client &client)
{
    struct stat statbuf;
    std::string icon;
    std::string size;
    std::string body;
    std::vector<dirent> entries;
    std::vector<dirent>::const_iterator it;

    try
    {
        entries = tool::files::readDirectory(path);
    }
    catch (const RuntimeError &e)
    {
        if (e.code() == EACCES)
            throw http::HttpRequest::RequestException("Forbidden", 403);
        if (e.code() == ENOENT || e.code() == ENOTDIR) return (false);

        throw;
    }

    body =
        "<html>\n"
        "<head>\n"
        "  <meta charset=\"UTF-8\">\n"
        "  <title>Index of " +
        uri +
        "</title>\n"
        "</head>\n\n"
        "<body>\n"
        "  <h1>Index of " +
        uri +
        "</h1>\n"
        "  <hr><pre>\n"
        "  <table>\n";

    for (it = entries.begin(); it != entries.end(); it++)
    {
        if ((it->d_name[0] == '.' && it->d_name[1] == 0)) continue;

        if (stat(it->d_name, &statbuf) == 0)
            size = tool::strings::toStr(statbuf.st_size);
        else
            size = "-";

        std::string entryPath(it->d_name);
        std::string encodedPath = http::urlEncode(entryPath);

        if (it->d_type == DT_DIR)
        {
            icon = "&#128193;";
            entryPath += "/";
            encodedPath += "/";
        }
        else if (it->d_type == DT_LNK)
            icon = "&#128279;";
        else
            icon = "&#128196;";

        body +=
            "    <tr>\n"
            "      <td>" +
            icon +
            "</td>\n"
            "      <td style=\"padding-right: 100\"><a href=\"" +
            encodedPath + "\">" + entryPath +
            "</a></td>\n"
            "      <td>" +
            size +
            "<td>\n"
            "    </tr>\n";
    }

    body +=
        "  <table>\n"
        "  </pre><hr>\n"
        "</body>\n"
        "</html>\n";

    http::HttpResponse response(200, client.getRequest());

    response.setBody(body, http::HttpMessage::TEXT_HTML);
    this->_log(Harl::INFO, &client, "200");
    client.sendResponse(response);

    return (true);
}

void VirtualServer::_sendErrorResponse(Client &client, int code)
{
    try
    {
        std::string path;
        const Configuration &config = this->getConfiguration();
        http::HttpResponse response(code, client.getRequest());
        std::map<int, std::string>::const_iterator it;

        it = config.getErrorPages().find(code);

        if (it != config.getErrorPages().end())
        {
            path = tool::files::joinPaths(config.getRoot(), it->second);

            response.setBody(tool::files::readFile(path),
                             http::HttpMessage::TEXT_HTML);
        }

        client.sendResponse(response);
    }
    catch (const std::exception &e)
    {
        this->_log(Harl::ERROR, &client,
                   "500 - Failed to read error page: " + std::string(e.what()));

        if (code != 500)
            this->_sendErrorResponse(client, 500);
        else
            client.sendResponse(http::HttpResponse(500, client.getRequest()));
    }
}

void VirtualServer::_log(Harl::e_level level, const Client *client,
                         const std::string &message) const
{
    if (!client)
    {
        if (this->_logger)
            this->_logger->log(level, message);
        else
            Harl::complain(level, message);
        return;
    }

    const http::HttpRequest &request = client->getRequest();
    std::string logMessage = client->getAddrStr(Client::PEER);
    std::string statusLine = request.getStatusLine();

    if (!statusLine.empty())
        logMessage += " REQ '" + request.getStatusLine() + "'";

    logMessage += " " + message;

    if (!request.getHeader("Host").empty())
        logMessage += " - " + request.getHeader("Host");

    if (!request.getHeader("User-Agent").empty())
        logMessage += " - '" + request.getHeader("User-Agent") + "'";

    if (this->_logger)
        this->_logger->log(level, logMessage);
    else
        Harl::complain(level, logMessage);
}

}  // namespace webserv