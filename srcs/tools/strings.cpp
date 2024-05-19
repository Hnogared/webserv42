/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strings.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 12:31:15 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/19 22:33:24 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "strings.hpp"

namespace tool
{

namespace strings
{

int stoi(const std::string &str)
{
    int nbr;
    std::istringstream iss(str);

    iss >> nbr;

    if (iss.fail() || !iss.eof())
        throw std::invalid_argument("Invalid int string `" + str + "`");

    return (nbr);
}

unsigned int stoui(const std::string &str)
{
    unsigned int nbr;
    std::istringstream iss(str);

    if (str[0] == '-')
        throw std::invalid_argument("Negative uint string `" + str + "`");

    iss >> nbr;

    if (iss.fail() || !iss.eof())
        throw std::invalid_argument("Invalid uint string `" + str + "`");

    return (nbr);
}

unsigned long int bytestoul(const std::string &str)
{
    unsigned long int nbr;
    char factor = 0;
    std::istringstream iss(str);

    if (str[0] == '-')
        throw std::invalid_argument("Negative byte string `" + str + "`");

    iss >> nbr;

    if (!iss.eof()) iss >> factor >> std::ws;

    if (iss.fail() || !iss.eof())
        throw std::invalid_argument("Invalid byte string `" + str + "`");

    switch (factor)
    {
        case 'K':
        case 'k':
            nbr *= 1024;
            break;
        case 'M':
        case 'm':
            nbr *= 1024 * 1024;
            break;
        case 'G':
        case 'g':
            nbr *= 1024 * 1024 * 1024;
            break;
        case 0:
            break;
        default:
            throw std::invalid_argument("Invalid byte factor `" +
                                        tool::strings::toStr(factor) + "`");
    }

    return (nbr);
}

std::string trim(const std::string &str, const std::string &set)
{
    std::string::size_type start = str.find_first_not_of(set);
    std::string::size_type end = str.find_last_not_of(set);

    if (start == std::string::npos) return ("");
    return (str.substr(start, end - start + 1));
}

bool isValidPath(const std::string &path)
{
    return (access(path.c_str(), F_OK) == 0);
}

}  // namespace strings

}  // namespace tool