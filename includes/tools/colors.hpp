/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   colors.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 22:28:27 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/06 23:09:51 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLORS_HPP
# define COLORS_HPP

#include <iostream>
#include <stdexcept>

namespace	ansi
{

/* Ansi escape sequence */
const std::string	ESQ_SEQ = "\e[";

/* Ansi codes enums */

enum	e_ansi_formatting
{
	RESET = 0,
	BOLD,
	DIM,
	UNDERLINED,
	BLINK,
	REVERSE,
	HIDDEN,
};

enum	e_ansi_colorFG
{
	FG_BLACK = 30,
	FG_RED,
	FG_GREEN,
	FG_YELLOW,
	FG_BLUE,
	FG_MAGENTA,
	FG_CYAN,
	FG_WHITE,
};

enum	e_ansi_colorBG
{
	BG_BLACK = 40,
	BG_RED,
	BG_GREEN,
	BG_YELLOW,
	BG_BLUE,
	BG_MAGENTA,
	BG_CYAN,
	BG_WHITE,
};

/* Ansi codes insertion overloads */
std::ostream &operator<<(std::ostream &os, e_ansi_formatting format);
std::ostream &operator<<(std::ostream &os, e_ansi_colorFG color);
std::ostream &operator<<(std::ostream &os, e_ansi_colorBG color);

} // namespace ansi

#endif // COLORS_HPP