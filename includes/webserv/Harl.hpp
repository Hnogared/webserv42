/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Harl.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 03:03:18 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/12 04:14:43 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HARL_HPP
# define HARL_HPP

# include <iostream>
# include <fstream>
# include <stdexcept>

# include <cerrno>
# include <cstring>
# include <unistd.h>

# include "colors.hpp"

class	Harl
{
	public:
		/* Available harl levels */
		enum	e_level
		{
			DEBUG = 0,
			INFO,
			WARNING,
			ERROR,
		};

		/* Constructors */
		explicit Harl(const std::string &logFilePath);

		/* Destructor */
		~Harl(void);

		/* Getters */
		const std::string	&getLogFilePath(void) const;

		/* Public methods */
		static void	complain(e_level level, const std::string &msg,
			bool colored = true, std::ostream &os = std::cout);
		static void	debug(const std::string &msg, bool colored,
			std::ostream &os);
		static void	info(const std::string &msg, bool colored,
			std::ostream &os);
		static void	warning(const std::string &msg, bool colored,
			std::ostream &os);
		static void	error(const std::string &msg, bool colored,
			std::ostream &os);
		void		log(e_level level, const std::string &msg);


	private:
		/* Private attributes */
		static void		(* const _methods_array[4])(const std::string &msg,
			bool colored, std::ostream &os);
		std::string		_logFilePath;
		std::ofstream	_logFile;

		/* [delete] */
		Harl(const Harl&);
		Harl	&operator=(const Harl&);
};

#endif
