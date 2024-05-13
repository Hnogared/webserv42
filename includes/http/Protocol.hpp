/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Protocol.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 18:16:27 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/13 12:22:39 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROTOCOL_HPP
# define PROTOCOL_HPP

# include <string>
# include <sstream>
# include <stdexcept>

namespace	http
{

class	Protocol
{
	public:
		/* Constructors */
		explicit Protocol(void);
		explicit Protocol(const std::string &str);
		Protocol(const Protocol &original);

		/* Destructor */
		~Protocol();

		/* Operator overloads */
		Protocol	&operator=(const Protocol &original);
		bool		operator==(const Protocol &other) const;
		bool		operator!=(const Protocol &other) const;
		bool		operator<(const Protocol &other) const;
		bool		operator>(const Protocol &other) const;
		bool		operator<=(const Protocol &other) const;
		bool		operator>=(const Protocol &other) const;

		/* Getters */
		const std::string	&getName(void) const;
		const int			*getVersion(void) const;


	private:
		/* Private attributes */
		std::string	_name;
		int			_version[2];
}; // class Protocol 

std::ostream	&operator<<(std::ostream &os, const Protocol &protocol);

} // namespace http

#endif // PROTOCOL_HPP