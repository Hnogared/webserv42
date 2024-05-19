/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualVirtualServer.hpp                                  :+:      :+: :+:
 */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 12:33:54 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/24 10:47:09 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VIRTUALSERVER_HPP
#define VIRTUALSERVER_HPP

#include <arpa/inet.h>
#include <dirent.h>
#include <poll.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <exception>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "Client.hpp"
#include "Configuration.hpp"
#include "Harl.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Socket.hpp"
#include "exceptions.hpp"
#include "files.hpp"

namespace webserv
{

class VirtualServer
{
public:
    /* Constructors */
    explicit VirtualServer(const Configuration &config, Harl *loggr = NULL);

    /* Destructor */
    ~VirtualServer(void);

    /* Getters */
    const Configuration &getConfiguration(void) const;

    /* Public methods */
    bool tryHandleClientRequest(Client &client, bool lastTry = false);

private:
    /* Private attributes */
    Configuration _config;
    Harl *_logger;

    /* Private methods */
    bool _checkServerNames(const std::string &host) const;
    const LocationConfiguration *_findBestLocation(
        const std::string &uri,
        const LocationConfiguration **bestLocation) const;
    bool _tryResponse(Client &client, const LocationConfiguration &location);
    bool _tryGetOrHeadResponse(Client &client,
                               const LocationConfiguration &location);
    bool _tryPostOrPutResponse(Client &client,
                               const LocationConfiguration &location);
    bool _tryDeleteResponse(Client &client,
                            const LocationConfiguration &location);
    bool _tryFileResponse(const std::string &uri, Client &client,
                          const LocationConfiguration &location);
    bool _tryDirectoryResponse(const std::string &uri, Client &client,
                               const LocationConfiguration &location);
    bool _tryDirectoryListing(const std::string &uri, const std::string &path,
                              Client &client);
    void _sendErrorResponse(Client &client, int code);
    void _log(Harl::e_level level, const Client *client,
              const std::string &message) const;

    /* [delete] */
    VirtualServer(void);
    VirtualServer(const VirtualServer &);
    VirtualServer &operator=(const VirtualServer &);
};  // class VirtualServer

}  // namespace webserv

#endif  // VIRTUALSERVER_HPP