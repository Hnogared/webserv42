/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 16:58:31 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/19 17:25:30 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# define WS_SERVER_VERSION	"1.0.0"
# define WS_SERVER_NAME		"webserv"

# define WS_DFL_PORT		8080
# define WS_DFL_HOST		"127.0.0.1"
# define WS_DFL_INDEX		"index.html"
# define WS_DFL_C_MAX_BODY	0
# define WS_DFL_BACKLOG		10

# define WS_LOCK_FILE		"/tmp/webserv42.lock"
# define WS_LOG_FILE		"test/log/webserv.log"
# define WS_DFL_CONFIG_PATH	"test/conf.d/webserv.conf"

# define WS_HTTP_VERSION	"HTTP/1.1"

# define WS_CLIENT_TIMEOUT_SEC	5
# define WS_CLIENT_TIMEOUT_USEC	0
# define WS_CLIENT_BUFF_SIZE	1024

#endif // WEBSERV_HPP
