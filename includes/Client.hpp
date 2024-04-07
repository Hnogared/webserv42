/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 14:42:17 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 14:54:15 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <unistd.h>

namespace	webserv
{

class	Client
{
	public:
		/* Constructors */
		explicit Client(int sock_fd);
		Client(const Client &original);

		/* Destructor */
		~Client(void);

		/* Operator overloads */
		Client	&operator=(const Client &original);

		/* Getters */
		int	getSockFd(void) const;


	private:
		/* Private attributes */
		int	_sock_fd;
};

} // namespace webserv

#endif // CLIENT_HPP