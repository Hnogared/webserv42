/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigurationParser.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 10:21:20 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/01 16:48:54 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigurationParser.hpp"

namespace	webserv
{

/* ************************************************************************** */
/* Static attributes initialization */

const std::map<std::string, ConfigurationParser::t_directiveParser>
		ConfigurationParser::_serverDirectives
	= ConfigurationParser::_initializeServerDirectives();

const std::map<std::string, ConfigurationParser::t_locationDirectiveParser>
		ConfigurationParser::_locationDirectives
	= ConfigurationParser::_initializeLocationDirectives();


/* ************************************************************************** */
/* Static methods */

std::vector<Configuration> ConfigurationParser::parse(const std::string &path)
{
	std::ifstream				file;
	std::vector<Configuration>	configurations;
	std::queue<t_token>			tokens;

	if (path.size() < 8 || path.substr(path.size() - 5) != ".conf")
	{
		throw InvalidConfigFile(path + ": Invalid file extension. "
			"Expected `.conf`");
	}

	file.open(path.c_str());
	if (!file.is_open())
	{
		throw InvalidConfigFile(path + ": Failed to open file: "
			+ strerror(errno));
	}

	try
	{
		ConfigurationParser::_tokenizeFile(file, tokens);
		file.close();
	}
	catch(const RuntimeError& e)
	{
		file.close();
		throw RuntimeError(path + ": " + e.what(), e.code());
	}

	try
	{
		ConfigurationParser::_parseTokens(tokens, configurations);
	}
	catch (const RuntimeError &e)
	{
		throw RuntimeError(path + ": " + e.what(), e.code());
	}

	return (configurations);
}


/* ************************************************************************** */
/* Private methods */

std::map<std::string, ConfigurationParser::t_directiveParser>
	ConfigurationParser::_initializeServerDirectives(void)
{
	std::map<std::string, t_directiveParser>	directives;

	directives["listen"] = &_parseListen;
	directives["server_name"] = &_parseNames;
	directives["error_page"] = &_parseErrorPage;
	directives["client_max_body_size"] = &_parseClientMaxBodySize;
	directives["location"] = &_parseLocation;
	return (directives);
}

std::map<std::string, ConfigurationParser::t_locationDirectiveParser>
	ConfigurationParser::_initializeLocationDirectives(void)
{
	std::map<std::string, t_locationDirectiveParser>	directives;

	directives["autoindex"] = &_parseAutoindex;
	directives["limit_except"] = &_parseAllowedMethods;
	return (directives);
}

std::queue<ConfigurationParser::t_token>	ConfigurationParser::_tokenizeFile(
	std::ifstream &file, std::queue<t_token> &tokens)
{
	int					lineNbr = 0;
	std::string			line;
	std::size_t			pos;

	while (std::getline(file, line))
	{
		lineNbr++;

		line = line.substr(0, line.find('#'));

		pos = line.find_first_not_of(" \t\v\f\r");
		if (pos == std::string::npos)
			continue;
		line = line.substr(pos, line.find_last_not_of(" \t\v\f\r") + 1);
		if (line.empty())
			continue;

		ConfigurationParser::_tokenizeLine(line, tokens, lineNbr);
	}
	return (tokens);
}

void	ConfigurationParser::_tokenizeLine(const std::string &line,
	std::queue<t_token> &tokens, int lineNbr)
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
				tokens.push((t_token){lineNbr, OPEN_BRACE, "{"});
				++pos;
				break;
			case '}':
				tokens.push((t_token){lineNbr, CLOSE_BRACE, "}"});
				++pos;
				break;
			case ';':
				tokens.push((t_token){lineNbr, SEMICOLON, ";"});
				++pos;
				break;
			case '"': case '\'':
				end = line.find(line[pos], pos + 1);
				if (end == std::string::npos)
					throw MissingToken(tool::strings::toStr(line[pos]));
				tokens.push((t_token){lineNbr, STRING,
					line.substr(pos + 1, end - pos - 1)});
				pos = end + 1;
				break;
			default:
				end = line.find_first_of(" \t\v\f\r{};", pos);
				tokens.push((t_token){lineNbr, STRING,
					line.substr(pos, end - pos)});
				pos = end;
		}
	}
}

void	ConfigurationParser::_parseTokens(std::queue<t_token> &tokens,
	std::vector<Configuration> &configurations)
{
	t_token			token;

	while (!tokens.empty())
	{
		token = tokens.front();
		tokens.pop();

		if (token.type == STRING)
		{
			if (token.content == "http")
			{
				if (tokens.front().type != OPEN_BRACE)
					throw UnexpectedToken(tokens.front(), "{");
				tokens.pop();
				ConfigurationParser::_parseHttpConfig(tokens, configurations);
				continue ;
			}

			throw UnexpectedToken(token);
		}

		throw UnexpectedToken(token);
	}
}

void	ConfigurationParser::_parseHttpConfig(std::queue<t_token> &tokens,
	std::vector<Configuration> &configurations)
{
	t_token			token;

	while (!tokens.empty())
	{
		token = tokens.front();
		tokens.pop();

		if (token.type == CLOSE_BRACE)
			break ;

		if (token.type == STRING)
		{
			if (token.content == "server")
			{
				if (tokens.front().type != OPEN_BRACE)
					throw UnexpectedToken(tokens.front(), "{");
				tokens.pop();
				ConfigurationParser::_parseServerConfig(tokens, configurations);
				continue ;
			}

			throw UnexpectedToken(token);
		}

		throw UnexpectedToken(token);
	}

	if (token.type != CLOSE_BRACE)
		throw MissingToken("}");
}

void	ConfigurationParser::_parseServerConfig(std::queue<t_token> &tokens,
	std::vector<Configuration> &configurations)
{
	t_token			token;
	t_contextType	context = SERVER;
	Configuration	config;
	std::map<std::string, t_directiveParser>::const_iterator it;

	while (!tokens.empty())
	{
		token = tokens.front();
		tokens.pop();
		if (token.type == CLOSE_BRACE && context == SERVER)
			break ;
		
		if (token.type == STRING)
		{
			it = ConfigurationParser::_serverDirectives.find(token.content);

			if (it != ConfigurationParser::_serverDirectives.end())
			{
				it->second(tokens, config);
				continue ;
			}
		}

		throw UnexpectedToken(token);
	}

	configurations.push_back(config);
}

void	ConfigurationParser::_parseListen(std::queue<t_token> &tokens,
	Configuration &config)
{
	size_t		pos;
	std::string	port = "";
	std::string	address = "";
	t_token		token;

	token = tokens.front();
	tokens.pop();

	if (token.type != STRING)
		throw UnexpectedToken(token, "string");

	pos = token.content.find(':');
	if (pos == std::string::npos)
	{
		if (token.content.find('.') == std::string::npos)
			config.setPort(tool::strings::stoi(token.content));
		else if (!config.setAddress(token.content))
			throw InvalidConfigFile(tool::strings::toStr(token.lineNbr)
				+ ": listen: Invalid address: `" + token.content + "`");
	}
	else
	{
		address = token.content.substr(0, pos);
		port = token.content.substr(pos + 1);
		config.setPort(tool::strings::stoi(port));
		if (!config.setAddress(address))
			throw InvalidConfigFile(tool::strings::toStr(token.lineNbr)
				+ ": listen: Invalid address: `" + address + "`");
	}

	token = tokens.front();
	tokens.pop();

	if (token.type != SEMICOLON)
		throw UnexpectedToken(token, ";");
}

void	ConfigurationParser::_parseNames(std::queue<t_token> &tokens,
	Configuration &config)
{
	t_token		token;

	if (tokens.front().type != STRING)
		throw UnexpectedToken(token, "string");

	while (tokens.front().type == STRING)
	{
		token = tokens.front();
		tokens.pop();
		if (token.content != "_")
			config.addServerName(token.content);
	}

	token = tokens.front();
	tokens.pop();

	if (token.type != SEMICOLON)
		throw UnexpectedToken(token, ";");
}

void	ConfigurationParser::_parseErrorPage(std::queue<t_token> &tokens,
	Configuration &config)
{
	int					code;
	t_token				token;
	std::vector<int>	codes;

	token = tokens.front();
	tokens.pop();

	if (token.type != STRING)
		throw UnexpectedToken(token, "string");
	if (tokens.front().type != STRING)
		throw UnexpectedToken(tokens.front(), "string");

	while (tokens.front().type == STRING)
	{
		code = tool::strings::stoi(token.content);
		if (code < 300 || code > 599)
			throw InvalidConfigFile(tool::strings::toStr(token.lineNbr)
				+ ": error_page: Invalid error code: `" + token.content + "`");
		codes.push_back(code);
		token = tokens.front();
		tokens.pop();
	}

	for (std::vector<int>::iterator it = codes.begin(); it != codes.end(); ++it)
		config.addErrorRedirect(*it, token.content);

	token = tokens.front();
	tokens.pop();

	if (token.type != SEMICOLON)
		throw UnexpectedToken(token, ";");
}

void	ConfigurationParser::_parseClientMaxBodySize(
	std::queue<t_token> &tokens, Configuration &config)
{
	t_token	token;

	token = tokens.front();
	tokens.pop();

	if (token.type != STRING)
		throw UnexpectedToken(token, "string");
	if (tokens.front().type != SEMICOLON)
		throw UnexpectedToken(tokens.front(), ";");

	try
	{
		config.setClientMaxBodySize(tool::strings::bytestoul(token.content));
	}
	catch(const std::invalid_argument &e)
	{
		throw InvalidConfigFile(tool::strings::toStr(token.lineNbr)
			+ ": client_max_body_size: " + e.what());
	}

	tokens.pop();
}

void	ConfigurationParser::_parseLocation(std::queue<t_token> &tokens,
	Configuration &config)
{
	t_token					token;
	LocationConfiguration	location;
	std::map<std::string, t_locationDirectiveParser>::const_iterator it;

	token = tokens.front();
	tokens.pop();

	if (tokens.front().type != OPEN_BRACE)
		throw UnexpectedToken(tokens.front(), "{");
	if (token.type != STRING)
		throw UnexpectedToken(token, "string");

	location.setPath(token.content);
	tokens.pop();

	while (!tokens.empty())
	{
		token = tokens.front();
		tokens.pop();

		if (token.type == CLOSE_BRACE)
			break ;

		if (token.type == STRING)
		{
			it = ConfigurationParser::_locationDirectives.find(token.content);

			if (it != ConfigurationParser::_locationDirectives.end())
			{
				it->second(tokens, location);
				continue ;
			}
		}

		throw UnexpectedToken(token);
	}

	if (token.type != CLOSE_BRACE)
		throw UnexpectedToken(token, "}");

	config.addLocation(location);
}

void	ConfigurationParser::_parseAutoindex(std::queue<t_token> &tokens,
	LocationConfiguration &config)
{
	t_token	token;

	token = tokens.front();
	tokens.pop();

	if (token.type != STRING)
		throw UnexpectedToken(token, "string");
	if (tokens.front().type != SEMICOLON)
		throw UnexpectedToken(tokens.front(), ";");

	if (token.content != "on" && token.content != "off")
		throw UnexpectedToken(token, "on / off");

	config.setAutoindex(token.content == "on");
	tokens.pop();
}

void	ConfigurationParser::_parseAllowedMethods(std::queue<t_token> &tokens,
	LocationConfiguration &config)
{
	t_token	token;

	if (tokens.empty())
		throw MissingToken("string");

	if (tokens.front().type != STRING)
		throw UnexpectedToken(tokens.front(), "string");

	while (!tokens.empty() && tokens.front().type == STRING)
	{
		token = tokens.front();
		tokens.pop();
		config.addAllowedMethod(token.content);
	}

	if (tokens.empty())
		throw MissingToken(";");

	if (tokens.front().type != SEMICOLON)
		throw UnexpectedToken(tokens.front(), ";");

	tokens.pop();
}


/* ************************************************************************** */
/* Exceptions */

/* ************************************ */
/* InvalidConfigFile                    */
/* ************************************ */

/* Default constructor */
ConfigurationParser::InvalidConfigFile::InvalidConfigFile()
	: RuntimeError("Invalid configuration file", 10) {}

/* Message constructor */
ConfigurationParser::InvalidConfigFile::InvalidConfigFile(
		const std::string &message)
	: RuntimeError(message, 10) {}

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


/* ************************************ */
/* UnexpectedToken                      */
/* ************************************ */

/* Default constructor */
ConfigurationParser::UnexpectedToken::UnexpectedToken()
	: RuntimeError("Invalid token", 11) {}

/* Message constructor */
ConfigurationParser::UnexpectedToken::UnexpectedToken(const std::string &msg)
	: RuntimeError(msg, 11) {}

/* Token constructor */
ConfigurationParser::UnexpectedToken::UnexpectedToken(const t_token &token)
	: RuntimeError(tool::strings::toStr(token.lineNbr) + ": Unexpected token `"
		+ token.content + "`", 11) {}

/* Token and message constructor */
ConfigurationParser::UnexpectedToken::UnexpectedToken(const t_token &token,
		const std::string &exp)
	: RuntimeError(tool::strings::toStr(token.lineNbr) + ": Unexpected token `"
		+ token.content + "`. Expected `" + exp + "`", 11) {}

/* Copy constructor */
ConfigurationParser::UnexpectedToken::UnexpectedToken(
		const UnexpectedToken &original)
	: RuntimeError(original) {}


/* Destructor */
ConfigurationParser::UnexpectedToken::~UnexpectedToken(void) throw() {}


/* ************************************ */
/* Operator overloads */

ConfigurationParser::UnexpectedToken
	&ConfigurationParser::UnexpectedToken::operator=(
	const UnexpectedToken &original)
{
	if (this == &original)
		return (*this);
	RuntimeError::operator=(original);
	return (*this);
}


/* ************************************ */
/* MissingToken                         */
/* ************************************ */

/* Default constructor */
ConfigurationParser::MissingToken::MissingToken()
	: RuntimeError("Missing token", 12) {}

/* Token type constructor */
ConfigurationParser::MissingToken::MissingToken(const std::string &type)
	: RuntimeError("Missing token `" + type + "`", 12) {}

/* Copy constructor */
ConfigurationParser::MissingToken::MissingToken(const MissingToken &original)
	: RuntimeError(original) {}


/* Destructor */
ConfigurationParser::MissingToken::~MissingToken(void) throw() {}


/* ************************************ */
/* Operator overloads */

ConfigurationParser::MissingToken
	&ConfigurationParser::MissingToken::operator=(
	const MissingToken &original)
{
	if (this == &original)
		return (*this);
	RuntimeError::operator=(original);
	return (*this);
}

} // namespace webserv