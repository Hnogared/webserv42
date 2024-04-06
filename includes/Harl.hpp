/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Harl.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 03:03:18 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/06 23:14:22 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HARL_HPP
# define HARL_HPP

# include <iostream>
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

		/* Static methods */
		static void	complain(e_level level, const std::string &msg);
		static void	debug(const std::string &msg);
		static void	info(const std::string &msg);
		static void	warning(const std::string &msg);
		static void	error(const std::string &msg);


	private:
		/* [delete] Default constructor */
		Harl(void);

		/* [delete] Copy constructor */
		Harl(const Harl &original);

		/* [delete] Destructor */
		~Harl(void);

		/* [delete] Copy assignment operator overload */
		Harl	&operator=(const Harl &original);

		/* Private class constants */
		static void	(* const _methods_array[4])(const std::string &msg);
};

#endif
