/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_common.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 21:31:56 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/14 21:48:36 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http_common.hpp"

namespace	http
{

std::string	urlEncode(const std::string &str)
{
	std::string	encoded;
	const char	*hex = "0123456789ABCDEF";

	for (size_t i = 0; i < str.size(); ++i)
	{
		if (isalnum(str[i]) || str[i] == '-' || str[i] == '_' || str[i] == '.'
				|| str[i] == '~')
			encoded += str[i];
		else if (str[i] == ' ')
			encoded += '+';
		else
		{
			encoded += '%';
			encoded += hex[str[i] >> 4];
			encoded += hex[str[i] & 0xf];
		}
	}

	return (encoded);
}

static int	stoi(const std::string &str, size_t *idx = NULL, int base = 10)
{
	int		n = 0;
	size_t	i = 0;

	while (i < str.size())
	{
		if (isdigit(str[i]))
			n = n * base + str[i] - '0';
		else if (base == 16 && isxdigit(str[i]))
			n = n * base + tolower(str[i]) - 'a' + 10;
		else
			break;
		++i;
	}

	if (idx)
		*idx = i;

	return (n);
}

std::string	urlDecode(const std::string &str)
{
	std::string	decoded;
	size_t		i = 0;

	while (i < str.size())
	{
		if (str[i] == '%')
		{
			if (i + 2 >= str.size() || !isxdigit(str[i + 1])
					|| !isxdigit(str[i + 2]))
				throw std::invalid_argument("urlDecode(): Invalid URL");

			decoded += static_cast<char>(stoi(str.substr(i + 1, 2), NULL, 16));
			i += 3;
		}
		else if (str[i] == '+')
		{
			decoded += ' ';
			++i;
		}
		else
			decoded += str[i++];
	}

	return (decoded);
}

} // namespace http