/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 19:02:56 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/07 19:24:57 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <unistd.h>

namespace webserv
{

class Socket
{
	public:
		/* Constructors */
		explicit Socket(int fd = -1);
		Socket(const Socket &original);

		/* Destructor */
		~Socket(void);

		/* Operator overloads */
		Socket	&operator=(const Socket &original);

		/* Getters */
		int	getFd(void) const;
		int	*getRefCountPtr(void) const;

		/* Public methods */
		void	close(void);


	private:
		/* Private attributes */
		int	_fd;
		int	*_ref_count;
};

} // namespace webserv

#endif // SOCKET_HPP