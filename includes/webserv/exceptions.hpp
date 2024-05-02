/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 21:03:07 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/02 12:36:50 by hnogared         ###   ########.fr       */
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
		explicit SocketCreationError();
		explicit SocketCreationError(const std::string &message);
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
		explicit SocketConnectionError();
		explicit SocketConnectionError(const std::string &message);
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
		explicit SocketConnectionClosed();
		explicit SocketConnectionClosed(const std::string &message);
		SocketConnectionClosed(const SocketConnectionClosed &original);

		/* Destructor */
		~SocketConnectionClosed(void) throw();

		/* Operator overloads */
		SocketConnectionClosed	&operator=(const SocketConnectionClosed
				&original);
};


class	InvalidPath : public RuntimeError
{
	public:
		/* Constructors */
		explicit InvalidPath(void);
		explicit InvalidPath(const std::string &message);
		InvalidPath(const InvalidPath &original);

		/* Destructor */
		~InvalidPath(void) throw();

		/* Operator overloads */
		InvalidPath	&operator=(const InvalidPath &original);
};

} // namespace webserv

#endif // EXCEPTIONS_HPP