/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 21:03:07 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/09 19:45:42 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

# include <stdexcept>
# include <string>

namespace	webserv
{

class	RuntimeError : public std::runtime_error
{
	public:
		/* Constructors */
		explicit RuntimeError(int code = 3);
		explicit RuntimeError(const std::string &message, int code = 3);
		RuntimeError(const RuntimeError &original);

		/* Destructor */
		~RuntimeError(void) throw();

		/* Operator overloads */
		RuntimeError	&operator=(const RuntimeError &original);

		/* Getters */
		int	code(void) const;


	private:
		/* Private attributes */
		int	_code;
};


class	SocketError : public RuntimeError
{
	public:
		/* Constructors */
		explicit SocketError(int code = 6);
		explicit SocketError(const std::string &message, int code = 6);
		SocketError(const SocketError &original);

		/* Destructor */
		~SocketError(void) throw();

		/* Operator overloads */
		SocketError	&operator=(const SocketError &original);
};


class	SocketCreationError : public SocketError
{
	public:
		/* Constructors */
		explicit SocketCreationError(int code = 7);
		explicit SocketCreationError(const std::string &message, int code = 7);
		SocketCreationError(const SocketCreationError &original);

		/* Destructor */
		~SocketCreationError(void) throw();

		/* Operator overloads */
		SocketCreationError	&operator=(const SocketCreationError &original);
};


class	SocketConnectionError : public SocketError
{
	public:
		/* Constructors */
		explicit SocketConnectionError(int code = 8);
		explicit SocketConnectionError(const std::string &message,
			int code = 8);
		SocketConnectionError(const SocketConnectionError &original);

		/* Destructor */
		~SocketConnectionError(void) throw();

		/* Operator overloads */
		SocketConnectionError	&operator=(const SocketConnectionError
				&original);
};


class	SocketConnectionClosed : public SocketError
{
	public:
		/* Constructors */
		explicit SocketConnectionClosed(int code = 9);
		explicit SocketConnectionClosed(const std::string &message,
			int code = 9);
		SocketConnectionClosed(const SocketConnectionClosed &original);

		/* Destructor */
		~SocketConnectionClosed(void) throw();

		/* Operator overloads */
		SocketConnectionClosed	&operator=(const SocketConnectionClosed
				&original);
};

} // namespace webserv

#endif // EXCEPTIONS_HPP