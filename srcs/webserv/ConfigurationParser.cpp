/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigurationParser.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 10:21:20 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/02 16:54:05 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigurationParser.hpp"

namespace	webserv
{

/* ************************************************************************** */
/* Static attributes initialization */

const std::map<std::string, ConfigurationParser::t_serverDirectiveParser>
		ConfigurationParser::_serverDirectives
	= ConfigurationParser::_initializeServerDirectives();

const std::map<std::string, ConfigurationParser::t_locationDirectiveParser>
		ConfigurationParser::_locationDirectives
	= ConfigurationParser::_initializeLocationDirectives();


/* ************************************************************************** */
/* Static methods */

std::vector<Configuration> *ConfigurationParser::parse(const std::string &path)
{
	std::ifstream				file;
	std::queue<t_token>			tokens;
	std::vector<Configuration>	*configurations = NULL;

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
		configurations = new std::vector<Configuration>;
		ConfigurationParser::_parseTokens(tokens, *configurations);
	}
	catch (const RuntimeError &e)
	{
		if (file.is_open())
			file.close();
		delete configurations;
		throw RuntimeError(path + ": " + e.what(), e.code());
	}
	catch (const std::exception &e)
	{
		if (file.is_open())
			file.close();
		delete configurations;
		throw std::runtime_error(path + ": " + e.what());
	}

	return (configurations);
}


/* ************************************************************************** */
/* Private methods */

std::map<std::string, ConfigurationParser::t_serverDirectiveParser>
	ConfigurationParser::_initializeServerDirectives(void)
{
	std::map<std::string, t_serverDirectiveParser>	directives;

	directives["listen"] = &_parseServerListen;
	directives["server_name"] = &_parseServerNames;
	directives["error_page"] = &_parseServerErrorPage;
	directives["client_max_body_size"] = &_parseServerClientMaxBodySize;
	directives["location"] = &_parseServerLocation;
	directives["root"] = &_parseServerRoot;
	directives["index"] = &_parseServerIndex;
	return (directives);
}

std::map<std::string, ConfigurationParser::t_locationDirectiveParser>
	ConfigurationParser::_initializeLocationDirectives(void)
{
	std::map<std::string, t_locationDirectiveParser>	directives;

	directives["autoindex"] = &_parseLocAutoindex;
	directives["limit_except"] = &_parseLocAllowedMethods;
	directives["return"] = &_parseLocReturn;
	directives["root"] = &_parseLocRoot;
	directives["index"] = &_parseLocIndex;
	directives["fastcgi_pass"] = &_parseLocFCGIPass;
	directives["fastcgi_param"] = &_parseLocFCGIParam;
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
	std::map<std::string, t_serverDirectiveParser>::const_iterator dirIt;

	while (!tokens.empty())
	{
		token = tokens.front();
		tokens.pop();
		if (token.type == CLOSE_BRACE && context == SERVER)
			break ;
		
		if (token.type == STRING)
		{
			dirIt = ConfigurationParser::_serverDirectives.find(token.content);

			if (dirIt != ConfigurationParser::_serverDirectives.end())
			{
				dirIt->second(tokens, config);
				continue ;
			}
		}

		throw UnexpectedToken(token);
	}

	configurations.push_back(config);
}

void	ConfigurationParser::_parseServerListen(std::queue<t_token> &tokens,
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

void	ConfigurationParser::_parseServerNames(std::queue<t_token> &tokens,
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

void	ConfigurationParser::_parseServerErrorPage(std::queue<t_token> &tokens,
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

void	ConfigurationParser::_parseServerClientMaxBodySize(
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

void	ConfigurationParser::_parseServerRoot(std::queue<t_token> &tokens,
	Configuration &config)
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
		config.setRoot(token.content);
	}
	catch (const InvalidPath &e)
	{
		throw InvalidConfigFile(tool::strings::toStr(token.lineNbr)
			+ ": root: " + e.what());
	}

	tokens.pop();
}

void	ConfigurationParser::_parseServerIndex(std::queue<t_token> &tokens,
	Configuration &config)
{
	t_token	token;

	if (tokens.empty())
		throw MissingToken("string");

	token = tokens.front();
	tokens.pop();

	if (token.type != STRING)
		throw UnexpectedToken(token, "string");
	if (tokens.front().type != SEMICOLON)
		throw UnexpectedToken(tokens.front(), ";");

	config.setIndex(token.content);
	tokens.pop();
}

void	ConfigurationParser::_parseServerLocation(std::queue<t_token> &tokens,
	Configuration &config)
{
	t_token	token;
	std::map<std::string, t_locationDirectiveParser>::const_iterator it;

	token = tokens.front();
	tokens.pop();

	if (tokens.front().type != OPEN_BRACE)
		throw UnexpectedToken(tokens.front(), "{");
	if (token.type != STRING)
		throw UnexpectedToken(token, "string");

	LocationConfiguration	location(token.content);
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

	ConfigurationParser::_completeLocation(location, config);

	config.addLocation(location);
}

void	ConfigurationParser::_completeLocation(LocationConfiguration &locConfig,
	const Configuration &servConfig)
{
	if (locConfig.getRoot().empty() && !servConfig.getRoot().empty())
		locConfig.setRoot(servConfig.getRoot());
	if (locConfig.getIndex().empty())
		locConfig.setIndex(servConfig.getIndex());
}

void	ConfigurationParser::_parseLocAutoindex(std::queue<t_token> &tokens,
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

void	ConfigurationParser::_parseLocAllowedMethods(
	std::queue<t_token> &tokens, LocationConfiguration &config)
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

void	ConfigurationParser::_parseLocReturn(std::queue<t_token> &tokens,
	LocationConfiguration &config)
{
	int		code;
	t_token	token;

	if (tokens.empty())
		throw MissingToken("string");
	
	token = tokens.front();
	tokens.pop();

	if (tokens.empty())
		throw MissingToken("string");

	code = tool::strings::stoi(token.content);
	if (code < 100 || code > 599)
		throw InvalidConfigFile(tool::strings::toStr(token.lineNbr)
			+ ": return: Invalid status code: `" + token.content + "`");

	config.setReturnCode(code);

	token = tokens.front();
	tokens.pop();

	if (token.type == SEMICOLON)
		return ;
	if (token.type != STRING)
		throw UnexpectedToken(token, "string");
	
	config.setReturnMessage(token.content);

	token = tokens.front();
	tokens.pop();

	if (token.type != SEMICOLON)
		throw UnexpectedToken(token, ";");
}

void	ConfigurationParser::_parseLocRoot(std::queue<t_token> &tokens,
	LocationConfiguration &config)
{
	t_token	token;

	if (tokens.empty())
		throw MissingToken("string");

	token = tokens.front();
	tokens.pop();

	if (tokens.empty())
		throw MissingToken(";");
	if (token.type != STRING)
		throw UnexpectedToken(token, "string");
	if (tokens.front().type != SEMICOLON)
		throw UnexpectedToken(tokens.front(), ";");

	try
	{
		config.setRoot(token.content);
	}
	catch (const InvalidPath &e)
	{
		throw InvalidConfigFile(tool::strings::toStr(token.lineNbr)
			+ ": root: " + e.what());
	}

	tokens.pop();
}

void	ConfigurationParser::_parseLocIndex(std::queue<t_token> &tokens,
	LocationConfiguration &config)
{
	t_token	token;

	if (tokens.empty())
		throw MissingToken("string");

	token = tokens.front();
	tokens.pop();

	if (tokens.empty())
		throw MissingToken(";");
	if (token.type != STRING)
		throw UnexpectedToken(token, "string");
	if (tokens.front().type != SEMICOLON)
		throw UnexpectedToken(tokens.front(), ";");
	
	config.setIndex(token.content);
	tokens.pop();
}

void	ConfigurationParser::_parseLocFCGIPass(std::queue<t_token> &tokens,
	LocationConfiguration &config)
{
	t_token	token;

	if (tokens.empty())
		throw MissingToken("string");
	
	token = tokens.front();
	tokens.pop();

	if (tokens.empty())
		throw MissingToken(";");
	if (token.type != STRING)
		throw UnexpectedToken(token, "string");
	
	config.setFCGIServer(token.content);
	tokens.pop();
}

void	ConfigurationParser::_parseLocFCGIParam(std::queue<t_token> &tokens,
	LocationConfiguration &config)
{
	std::string	key;
	t_token		token;

	if (tokens.empty())
		throw MissingToken("string");

	token = tokens.front();
	tokens.pop();

	if (tokens.empty())
		throw MissingToken("string");
	if (token.type != STRING)
		throw UnexpectedToken(token, "string");

	key = token.content;

	token = tokens.front();
	tokens.pop();

	if (tokens.empty())
		throw MissingToken("string");
	if (token.type != STRING)
		throw UnexpectedToken(token, "string");

	config.addFCGIParam(key, token.content);

	token = tokens.front();
	tokens.pop();

	if (token.type != SEMICOLON)
		throw UnexpectedToken(token, ";");
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