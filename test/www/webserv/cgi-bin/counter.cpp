/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   counter.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 14:39:52 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/29 15:45:27 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

#include <fstream>
#include <iostream>

static void printError(const std::string &message, std::ostream &os)
{
    os << "Status: 500 Internal Server Error\r\n"
       << "Content-type: text/html\r\n\r\n"
          "<html><head><title>Counter Error</title></head><body>"
          "<h1>Counter Error</h1>"
          "<p>"
       << message
       << "</p>"
          "</body></html>";
}

static std::string getCounterFilePath(char **envp)
{
    size_t pos;
    std::string filePath;

    for (int i = 0; envp[i]; i++)
    {
        filePath = envp[i];

        if (filePath.find("SCRIPT_FILENAME") == 0)
        {
            pos = filePath.find("=");

            if (pos == std::string::npos) return ("");
            filePath = filePath.substr(pos + 1);

            pos = filePath.rfind('/');

            if (pos != std::string::npos)
                return (filePath.substr(0, pos + 1) + "counter.txt");

            return ("counter.txt");
        }
    }

    return ("");
}

static int readCounterFile(const std::string &filePath, int *count)
{
    std::ifstream file(filePath.c_str(), std::ios::in);

    if (!file)
    {
        *count = 0;
        return (0);
    }

    file.seekg(0);
    file >> *count;
    file.close();

    return (0);
}

static int writeCounterFile(const std::string &filePath, int count)
{
    std::ofstream file(filePath.c_str(), std::ios::out);

    if (!file) return (1);

    file << count;
    file.close();

    return (0);
}

int main(int argc, char **argv, char **envp)
{
    int count = 0;
    std::string filePath;

    (void)argc;
    (void)argv;

    filePath = getCounterFilePath(envp);

    if (filePath.empty() || readCounterFile(filePath, &count))
    {
        printError("Could not open counter counterFile", std::cout);
        return (1);
    }

    if (writeCounterFile(filePath, ++count))
    {
        printError("Could not write counter counterFile", std::cout);
        return (1);
    }

    std::cout << "Content-type: text/html\r\n\r\n"
              << "<html><head><title>Counter.io</title></head><body>"
              << "<h1>Counter</h1>"
              << "<p>" << count << "</p>"
              << "<button><a href=\"/cgi-bin/\">More !!</a></button>"
              << "</body></html>";

    return (0);
}