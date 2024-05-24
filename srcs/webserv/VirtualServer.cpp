/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 02:08:16 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/24 12:22:40 by hnogared         ###   ########.fr       */
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

bool VirtualServer::_tryResponse(Client &client,
                                 const LocationConfiguration &location)
{
    const http::HttpRequest &request = client.getRequest();
    const http::HttpRequest::e_method method = request.getMethod();

    if (location.getLocationType() == LocationConfiguration::REDIRECT)
    {
        http::HttpResponse response(location.getReturnCode(), request);

        response.setStatusLine(location.getReturnMessage());
        response.setHeader("Location", location.getReturnMessage());
        this->_log(Harl::INFO, &client,
                   tool::strings::toStr(location.getReturnCode()));
        client.sendResponse(response);
        return (true);
    }

    if (location.getLocationType() == LocationConfiguration::DYNAMIC)
    {
        this->_doCGIResponse(client, location);
        return (true);
    }

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
            throw http::HttpRequest::RequestException("Method Not Allowed",
                                                      405);
    }
}

void VirtualServer::_doCGIResponse(Client &client,
                                   const LocationConfiguration &location)
{
    int pid;
    int pipeOut[2];
    std::string uri = client.getRequest().getUri();
    std::map<std::string, std::string> params = location.getFCGIParams();

    this->_completeParams(client, location, params);

    if (pipe(pipeOut) == -1)
        throw http::HttpRequest::RequestException("Internal Server Error", 500);

    if ((pid = fork()) == -1)
    {
        close(pipeOut[0]);
        close(pipeOut[1]);
        throw http::HttpRequest::RequestException("Internal Server Error", 500);
    }

    if (pid == 0)
    {
        dup2(pipeOut[1], STDOUT_FILENO);
        close(pipeOut[0]);
        close(pipeOut[1]);

        std::map<std::string, std::string>::const_iterator it;
        char *argv[] = {NULL};
        char **env = new char *[params.size() + 1];
        int index = 0;

        for (it = params.begin(); it != params.end(); it++)
        {
            std::string envVar = it->first + "=" + it->second;

            env[index] = new char[envVar.size() + 1];
            std::strcpy(env[index], envVar.c_str());
            index++;
        }

        env[index] = NULL;

        execve(params["SCRIPT_FILENAME"].c_str(), argv, env);

        for (int i = 0; i < index; i++) delete[] env[i];
        delete[] env;
        exit(errno);
    }

    waitpid(pid, NULL, 0);

    this->_readAndSendCGIResponse(client, pipeOut);
}

static void insertParam(std::map<std::string, std::string> &params,
                        const std::string &key, const std::string &value)
{
    if (!value.empty() && !key.empty())
        params.insert(std::make_pair(key, value));
}

void VirtualServer::_completeParams(
    const Client &client, const LocationConfiguration &location,
    std::map<std::string, std::string> &params) const
{
    const http::HttpRequest &request = client.getRequest();
    std::string uri = request.getUri();

    insertParam(params, "QUERY_STRING", request.getQueryString());
    insertParam(params, "REQUEST_METHOD", request.getMethodStr());

    insertParam(params, "DOCUMENT_URI", uri);
    insertParam(params, "DOCUMENT_ROOT", location.getRoot());

    if (request.getBody().empty())
        insertParam(params, "CONTENT_LENGTH", "0");
    else
    {
        insertParam(params, "CONTENT_LENGTH",
                    tool::strings::toStr(request.getBody().size()));
    }

    insertParam(params, "CONTENT_TYPE", request.getHeader("Content-Type"));
    insertParam(params, "GATEWAY_INTERFACE", WS_CGI_VERSION);

    {
        const std::string &remoteUser =
            client.getSocket().getAddrStr(Socket::PEER);

        insertParam(params, "REMOTE_ADDR",
                    remoteUser.substr(0, remoteUser.find(':')));
        insertParam(params, "REMOTE_PORT",
                    remoteUser.substr(remoteUser.find(':') + 1));
    }

    {
        const std::string &localUser =
            client.getSocket().getAddrStr(Socket::LOCAL);

        insertParam(params, "SERVER_ADDR",
                    localUser.substr(0, localUser.find(':')));
        insertParam(params, "SERVER_PORT",
                    localUser.substr(localUser.find(':') + 1));
    }

    insertParam(params, "SERVER_NAME",
                request.getHeader("Host").substr(
                    0, request.getHeader("Host").find(':')));

    insertParam(params, "SERVER_PROTOCOL", WS_HTTP_VERSION);
    insertParam(params, "SERVER_SOFTWARE",
                WS_SERVER_NAME "/" WS_SERVER_VERSION);

    if (uri == location.getPath() &&
        location.getFCGIParams().find("SCRIPT_NAME") ==
            location.getFCGIParams().end())
    {
        if (location.getFCGIIndex().empty())
            throw http::HttpRequest::RequestException("Not found", 404);

        const std::string scriptFilenameDir =
            tool::files::joinPaths(location.getRoot(), uri);

        insertParam(
            params, "SCRIPT_FILENAME",
            tool::files::joinPaths(scriptFilenameDir, location.getFCGIIndex()));
    }

    insertParam(params, "SCRIPT_FILENAME",
                tool::files::joinPaths(location.getRoot(), uri));
    insertParam(params, "SCRIPT_NAME", uri);

    insertParam(params, "PATH_INFO", uri);

    insertParam(params, "PATH_TRANSLATED",
                tool::files::joinPaths(location.getRoot(), uri));

    insertParam(params, "REDIRECT_STATUS", "200");
}

void VirtualServer::_readAndSendCGIResponse(Client &client, int pipeOut[2])
{
    int readSize;
    int flags = fcntl(pipeOut[0], F_GETFL, 0);
    char buffer[WS_READ_BUFF_SIZE];
    std::string output;

    fcntl(pipeOut[0], F_SETFL, flags | O_NONBLOCK);

    do
    {
        readSize = read(pipeOut[0], buffer, WS_READ_BUFF_SIZE - 1);
        if (readSize == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) continue;

            close(pipeOut[0]);
            close(pipeOut[1]);
            throw http::HttpRequest::RequestException("Internal Server Error",
                                                      500);
        }

        buffer[readSize] = 0;
        output += buffer;
    } while (readSize > 0);

    close(pipeOut[0]);
    close(pipeOut[1]);

    http::HttpResponse response(200, client.getRequest());

    response.parseCGIResponse(output);
    this->_log(Harl::INFO, &client,
               tool::strings::toStr(response.getStatusCode()));
    client.sendResponse(response);
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

    client.fetchRequestBody(location.getClientMaxBodySize());

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
    std::string logMessage = client->getSocket().getAddrStr(Socket::PEER);
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