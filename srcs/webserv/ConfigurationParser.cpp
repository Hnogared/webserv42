/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigurationParser.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 10:21:20 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/25 12:03:03 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigurationParser.hpp"

namespace	webserv
{

/* ************************************************************************** */
/* Static const attributes initialization */

// const std::map<std::string, ConfigurationParser::t_contextType>
// 	ConfigurationParser::_contexts = {
// 		{"http", ConfigurationParser::HTTP},
// 		{"server", ConfigurationParser::SERVER}
// 	};


/* ************************************************************************** */
/* Static methods */

std::vector<Configuration> ConfigurationParser::parse(const std::string &path)
{
	std::ifstream				file;
	std::vector<Configuration>	configurations;
	std::queue<t_token>			tokens;

	if (path.size() < 8 || path.substr(path.size() - 7) != ".config")
	{
		throw InvalidConfigFile("`" + path + "`: Invalid file extension. "
			"Expected `.config`.");
	}

	file.open(path.c_str());
	if (!file.is_open())
	{
		throw InvalidConfigFile("`" + path + "`: Failed to open file: "
			+ strerror(errno));
	}

	ConfigurationParser::_tokenizeFile(file, tokens);

	ConfigurationParser::_parseTokens(tokens, configurations);

	file.close();
	return (configurations);
}


/* ************************************************************************** */
/* Private methods */

std::queue<ConfigurationParser::t_token>	ConfigurationParser::_tokenizeFile(
	std::ifstream &file, std::queue<t_token> &tokens)
{
	std::string			line;
	std::size_t			pos;

	while (std::getline(file, line))
	{
		line = line.substr(0, line.find('#'));

		pos = line.find_first_not_of(" \t\v\f\r");
		if (pos == std::string::npos)
			continue;
		line = line.substr(pos, line.find_last_not_of(" \t\v\f\r") + 1);
		if (line.empty())
			continue;

		ConfigurationParser::_tokenizeLine(line, tokens);
	}
	return (tokens);
}

void	ConfigurationParser::_tokenizeLine(const std::string &line,
	std::queue<t_token> &tokens)
{
	size_t	pos = 0;
	size_t	end;

	while (pos < line.size())
	{
		switch (line[pos])
		{
			case ' ': case '\t': case '\v': case '\f': case '\r':
				++pos;
				break;
			case '{':
				tokens.push((t_token){OPEN_BRACE, "{"});
				++pos;
				break;
			case '}':
				tokens.push((t_token){CLOSE_BRACE, "}"});
				++pos;
				break;
			case ';':
				tokens.push((t_token){SEMICOLON, ";"});
				++pos;
				break;
			default:
				end = line.find_first_of(" \t\v\f\r{};", pos);
				tokens.push((t_token){STRING, line.substr(pos, end - pos)});
				pos = end;
		}
	}
}

void	ConfigurationParser::_parseTokens(std::queue<t_token> &tokens,
	std::vector<Configuration> &configurations)
{
	t_token			token;
	t_contextType	context = UNKNOWN;

	while (tokens.size())
	{
		token = tokens.front();
		tokens.pop();

		if (token.type == STRING)
		{
			if (token.content == "http")
			{
				if (context != UNKNOWN)
					throw InvalidConfigFile("Unexpected token `http`");
				context = HTTP;
			}
			else if (token.content == "server")
			{
				if (context != HTTP)
					throw InvalidConfigFile("Unexpected token `server`");
				context = SERVER;
				configurations.push_back(Configuration());
			}
			else
				throw InvalidConfigFile("Unexpected token `" + token.content + "`");
			}
		}
		else
			throw InvalidConfigFile("Unexpected token `" + token.content + "`");
	}
}


/* ************************************************************************** */
/* Exceptions */

/* ************************************ */
/* InvalidConfigFile                    */
/* ************************************ */

/* Code constructor */
ConfigurationParser::InvalidConfigFile::InvalidConfigFile(int code)
	: RuntimeError("Invalid configuration file", code) {}

/* Message and code constructor */
ConfigurationParser::InvalidConfigFile::InvalidConfigFile(
	const std::string &message, int code)
	: RuntimeError(message, code) {}

/* Copy constructor */
ConfigurationParser::InvalidConfigFile::InvalidConfigFile(
	const InvalidConfigFile &original)
	: RuntimeError(original) {}


/* Destructor */
ConfigurationParser::InvalidConfigFile::~InvalidConfigFile(void) throw() {}


/* ************************************ */
/* Operator overloads */

ConfigurationParser::InvalidConfigFile
		&ConfigurationParser::InvalidConfigFile::operator=(
	const InvalidConfigFile &original)
{
	if (this == &original)
		return (*this);
	RuntimeError::operator=(original);
	return (*this);
}

} // namespace webserv