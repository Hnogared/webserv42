/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_common.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 21:31:56 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/20 18:11:48 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http_common.hpp"

namespace http
{

std::string urlEncode(const std::string &str)
{
    std::string encoded;
    const char *hex = "0123456789ABCDEF";

    for (size_t i = 0; i < str.size();)
    {
        // Handle ASCII characters
        if ((str[i] & 0x80) == 0)
        {
            if (isalnum(str[i]) || str[i] == '-' || str[i] == '_' ||
                str[i] == '.' || str[i] == '~')
                encoded += str[i];
            else if (str[i] == ' ')
                encoded += '+';
            else
            {
                encoded += '%';
                encoded += hex[str[i] >> 4];
                encoded += hex[str[i] & 0xf];
            }
            ++i;
        }
        // Handle non-ASCII characters
        else
        {
            int len = 0;
            if ((str[i] & 0xe0) == 0xc0)
                len = 2;
            else if ((str[i] & 0xf0) == 0xe0)
                len = 3;
            else if ((str[i] & 0xf8) == 0xf0)
                len = 4;
            else
                throw std::invalid_argument("urlEncode(): Invalid UTF-8");

            while (len--)
            {
                encoded += '%';
                encoded += hex[(unsigned char)str[i] >> 4];
                encoded += hex[(unsigned char)str[i] & 0xf];
                ++i;
            }
        }
    }

    return (encoded);
}

std::string urlDecode(const std::string &str)
{
    std::string decoded;
    size_t i = 0;

    while (i < str.size())
    {
        if (str[i] == '%')
        {
            if (i + 2 >= str.size() || !isxdigit(str[i + 1]) ||
                !isxdigit(str[i + 2]))
                throw std::invalid_argument("urlDecode(): Invalid URL");

            decoded += static_cast<char>(
                tool::strings::stoib(str.substr(i + 1, 2), NULL, 16));
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

}  // namespace http