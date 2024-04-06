/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Harl.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 03:05:45 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/06 23:14:30 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Harl.hpp"

/* Private class constants initialization */

void	(* const Harl::_methods_array[4])(const std::string &msg) = {
	&Harl::debug,
	&Harl::info,
	&Harl::warning,
	&Harl::error,
};

/* ************************************************************************** */
/* Static methods */

/* Method to display a message of a given level */
void	Harl::complain(e_level level, const std::string &msg)
{
	(*Harl::_methods_array[level])(msg);
}

/* Method to display a debug level message */
void	Harl::debug(const std::string &msg)
{
	std::cout << "[DEBUG] " << msg << '\n';
}

/* Method to display an info level message */
void	Harl::info(const std::string &msg)
{
	std::cout << ansi::FG_BLUE << "[INFO]" << ansi::RESET << " "
		<< msg << '\n';
}

/* Method to display a warning level message */
void	Harl::warning(const std::string &msg)
{
	std::cout << ansi::FG_YELLOW << "[WARNING]" << ansi::RESET << " "
		<< msg << '\n';
}

/* Method to display an error level message */
void	Harl::error(const std::string &msg)
{
	std::cout << ansi::FG_RED << "[ERROR]" << ansi::RESET << " "
		<< msg << '\n';
}
