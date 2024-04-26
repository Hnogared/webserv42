/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   colors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 22:34:16 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/26 12:40:20 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "colors.hpp"

namespace tool
{

namespace ansi
{

std::ostream	&operator<<(std::ostream &os, e_ansi_formatting format)
{
	if (!os.good())
		throw std::runtime_error("Stream is not in a good state");

	os << ESQ_SEQ << static_cast<int>(format) << "m";

	if (!os.good())
		throw std::runtime_error("Failed to write to stream");
	return (os);
}

std::ostream	&operator<<(std::ostream &os, e_ansi_colorFG color)
{
	if (!os.good())
		throw std::runtime_error("Stream is not in a good state");

	os << ESQ_SEQ << static_cast<int>(color) << "m";

	if (!os.good())
		throw std::runtime_error("Failed to write to stream");
	return (os);
}

std::ostream	&operator<<(std::ostream &os, e_ansi_colorBG color)
{
	if (!os.good())
		throw std::runtime_error("Stream is not in a good state");

	os << ESQ_SEQ << static_cast<int>(color) << "m";

	if (!os.good())
		throw std::runtime_error("Failed to write to stream");
	return (os);
}

} // namespace ansi

} // namespace tool