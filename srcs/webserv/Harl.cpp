/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Harl.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 03:05:45 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/19 22:34:04 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Harl.hpp"

/* ************************************************************************** */
/* Static attributes initialization */

void (*const Harl::_methods_array[4])(const std::string &msg, bool colored,
                                      std::ostream &os) = {
    &Harl::debug,
    &Harl::info,
    &Harl::warning,
    &Harl::error,
};

/* ************************************************************************** */

/* Log file constructor */
Harl::Harl(const std::string &logFilePath) : _logFilePath(logFilePath)
{
    if (access(logFilePath.c_str(), R_OK | W_OK) == -1)
    {
        std::ofstream openLogFile(logFilePath.c_str());

        if (!openLogFile.is_open())
        {
            throw std::runtime_error(
                "Harl: " + logFilePath +
                ": Failed to open log file: " + std::string(strerror(errno)));
        }

        openLogFile.close();
    }

    this->_logFile.open(logFilePath.c_str(),
                        std::ios::in | std::ios::out | std::ios::trunc);

    if (!this->_logFile.is_open())
    {
        throw std::runtime_error(
            "Harl: " + logFilePath +
            ": Failed to open log file: " + std::string(strerror(errno)));
    }
}

/* Destructor */
Harl::~Harl(void) { this->_logFile.close(); }

/* ************************************************************************** */
/* Getters */

const std::string &Harl::getLogFilePath(void) const
{
    return (this->_logFilePath);
}

/* ************************************************************************** */
/* Public methods */

/* Method to display a message of a given level */
void Harl::complain(e_level level, const std::string &msg, bool colored,
                    std::ostream &os)
{
    (*Harl::_methods_array[level])(msg, colored, os);
}

/* Method to display a debug level message */
void Harl::debug(const std::string &msg, bool colored, std::ostream &os)
{
    (void)colored;
    os << "[DEBUG] " << msg << '\n';
}

/* Method to display an info level message */
void Harl::info(const std::string &msg, bool colored, std::ostream &os)
{
    if (colored)
        os << tool::ansi::FG_BLUE << "[INFO]" << tool::ansi::RESET << " ";
    else
        os << "[INFO] ";

    os << msg << '\n';
}

/* Method to display a warning level message */
void Harl::warning(const std::string &msg, bool colored, std::ostream &os)
{
    if (colored)
        os << tool::ansi::FG_YELLOW << "[WARNING]" << tool::ansi::RESET;
    else
        os << "[WARNING] ";

    os << " " << msg << '\n';
}

/* Method to display an error level message */
void Harl::error(const std::string &msg, bool colored, std::ostream &os)
{
    if (colored)
        os << tool::ansi::FG_RED << "[ERROR]" << tool::ansi::RESET << " ";
    else
        os << "[ERROR] ";

    os << msg << '\n';
}

/* Method to log a message of a given importance to the object's log file */
void Harl::log(e_level level, const std::string &msg)
{
    if (!this->_logFile.is_open()) return;

    Harl::complain(level, msg, false, this->_logFile);
    this->_logFile.flush();
}

void Harl::reopen(void)
{
    this->_logFile.close();
    this->_logFile.open(this->_logFilePath.c_str(),
                        std::ios::in | std::ios::out | std::ios::app);

    if (!this->_logFile.is_open())
    {
        throw std::runtime_error(
            "Harl: " + this->_logFilePath +
            ": Failed to open log file: " + std::string(strerror(errno)));
    }
}