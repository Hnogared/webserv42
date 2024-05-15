/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigurationParser.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 10:21:20 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/15 12:47:09 by hnogared         ###   ########.fr       */
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

	try
	{
		ConfigurationParser::_openConfigFile(path, file);
		ConfigurationParser::_tokenizeFile(file, tokens);
		file.close();

		configurations = new std::vector<Configuration>;
		ConfigurationParser::_parseTokens(tokens, *configurations);
	}
	catch (const std::exception &e)
	{
		delete configurations;
		file.close();
		throw;
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
	directives["root"] = &_parseServerRoot;
	directives["index"] = &_parseServerIndex;
	directives["backlog"] = &_parseServerBacklog;
	directives["location"] = &_parseServerLocation;
	return (directives);
}

std::map<std::string, ConfigurationParser::t_locationDirectiveParser>
	ConfigurationParser::_initializeLocationDirectives(void)
{
	std::map<std::string, t_locationDirectiveParser>	directives;

	directives["autoindex"] = &_parseLocAutoindex;
	directives["allow_methods"] = &_parseLocAllowedMethods;
	directives["return"] = &_parseLocReturn;
	directives["root"] = &_parseLocRoot;
	directives["index"] = &_parseLocIndex;
	directives["fastcgi_pass"] = &_parseLocFCGIPass;
	directives["fastcgi_param"] = &_parseLocFCGIParam;
	return (directives);
}

void	ConfigurationParser::_openConfigFile(const std::string &path,
	std::ifstream &file)
{
	if (path.size() < 8 || path.substr(path.size() - 5) != ".conf")
		throw InvalidConfigFile(path + ": Invalid file extension. "
			"Expected `.conf`");

	file.open(path.c_str());
	if (!file.is_open())
		throw InvalidConfigFile(path + ": Failed to open file: "
			+ strerror(errno));
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
					throw MissingToken("", tool::strings::toStr(line[pos]));
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
					throw UnexpectedToken(tokens.front(), "http", "{");
				tokens.pop();
				ConfigurationParser::_parseHttpConfig(tokens, configurations);
				continue ;
			}

			throw UnexpectedToken(token, "GLOBAL_CONTEXT");
		}

		throw UnexpectedToken(token, "GLOBAL_CONTEXT");
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
					throw UnexpectedToken(tokens.front(), "server", "{");
				tokens.pop();
				ConfigurationParser::_parseServerConfig(tokens, configurations);
				continue ;
			}

			throw UnexpectedToken(token, "HTTP_CONTEXT");
		}

		throw UnexpectedToken(token, "HTTP_CONTEXT");
	}

	if (token.type != CLOSE_BRACE)
		throw MissingToken("http", "}");
}

void	ConfigurationParser::_parseServerConfig(std::queue<t_token> &tokens,
	std::vector<Configuration> &configurations)
{
	t_token			token;
	Configuration	config;
	std::map<std::string, t_serverDirectiveParser>::const_iterator dirIt;

	while (!tokens.empty())
	{
		token = tokens.front();
		tokens.pop();
		if (token.type == CLOSE_BRACE)
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

		throw UnexpectedToken(token, "SERVER_CONTEXT");
	}

	if (config.getLocations().empty())
	{
		LocationConfiguration	defaultLocation("/");

		ConfigurationParser::_completeLocation(defaultLocation, config);
		config.addLocation(defaultLocation);
	}

	configurations.push_back(config);
}

void	ConfigurationParser::_parseServerListen(std::queue<t_token> &tokens,
	Configuration &config)
{
	size_t		pos;
	t_token		token;

	token = tokens.front();
	tokens.pop();

	if (token.type != STRING)
		throw UnexpectedToken(token, "listen", "string");

	
	try
	{
		pos = token.content.find(':');

		if (pos == std::string::npos)
		{
			if (token.content.find('.') != std::string::npos)
				config.setAddress(token.content);
			else
				config.setPort(tool::strings::stoi(token.content));
		}
		else
		{
			config.setPort(tool::strings::stoi(token.content.substr(pos + 1)));
			config.setAddress(token.content.substr(0, pos));
		}
	}
	catch(const std::exception& e)
	{
		throw InvalidConfigFile(token.lineNbr, "listen", e.what());
	}

	token = tokens.front();
	tokens.pop();

	if (token.type != SEMICOLON)
		throw UnexpectedToken(token, "listen", ";");
}

void	ConfigurationParser::_parseServerNames(std::queue<t_token> &tokens,
	Configuration &config)
{
	t_token		token;

	if (tokens.front().type != STRING)
		throw UnexpectedToken(token, "server_names", "string");

	while (tokens.front().type == STRING)
	{
		token = tokens.front();
		tokens.pop();

		if (token.content != "_")
		{
			config.addServerName(token.content);
			continue ;
		}

		if (!config.getServerNames().empty() || tokens.front().type == STRING)
		{
			throw InvalidConfigFile(token.lineNbr, "server_names",
				"Cannot use `_` with other server names");
		}
	}

	token = tokens.front();
	tokens.pop();

	if (token.type != SEMICOLON)
		throw UnexpectedToken(token, "server_names", ";");
}

void	ConfigurationParser::_parseServerErrorPage(std::queue<t_token> &tokens,
	Configuration &config)
{
	t_token						token;
	std::vector<int>			codes;
	std::vector<int>::iterator	it;

	token = tokens.front();
	tokens.pop();

	if (token.type != STRING)
		throw UnexpectedToken(token, "error_page", "string");
	if (tokens.front().type != STRING)
		throw UnexpectedToken(tokens.front(), "error_page", "string");

	try
	{
		while (tokens.front().type == STRING)
		{
			codes.push_back(tool::strings::stoi(token.content));
			token = tokens.front();
			tokens.pop();
		}

		for (it = codes.begin(); it != codes.end(); ++it)
			config.addErrorRedirect(*it, token.content);
	}
	catch (const std::exception &e)
	{
		throw InvalidConfigFile(token.lineNbr, "error_page", e.what());
	}

	token = tokens.front();
	tokens.pop();

	if (token.type != SEMICOLON)
		throw UnexpectedToken(token, "error_page", ";");
}

void	ConfigurationParser::_parseServerClientMaxBodySize(
	std::queue<t_token> &tokens, Configuration &config)
{
	t_token	token;

	token = tokens.front();
	tokens.pop();

	if (token.type != STRING)
		throw UnexpectedToken(token, "client_max_body_size", "string");
	if (tokens.front().type != SEMICOLON)
		throw UnexpectedToken(tokens.front(), "client_max_body_size", ";");

	try
	{
		config.setClientMaxBodySize(tool::strings::bytestoul(token.content));
	}
	catch(const std::invalid_argument &e)
	{
		throw InvalidConfigFile(token.lineNbr, "client_max_body_size",e.what());
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
		throw UnexpectedToken(token, "root", "string");
	if (tokens.front().type != SEMICOLON)
		throw UnexpectedToken(tokens.front(), "root", ";");

	try
	{
		config.setRoot(token.content);
	}
	catch (const InvalidPath &e)
	{
		throw InvalidConfigFile(token.lineNbr, "root", e.what());
	}

	tokens.pop();
}

void	ConfigurationParser::_parseServerIndex(std::queue<t_token> &tokens,
	Configuration &config)
{
	t_token	token;

	if (tokens.empty())
		throw MissingToken("index", "string");

	token = tokens.front();
	tokens.pop();

	if (token.type != STRING)
		throw UnexpectedToken(token, "index", "string");
	if (tokens.front().type != SEMICOLON)
		throw UnexpectedToken(tokens.front(), "index", ";");

	config.setIndex(token.content);
	tokens.pop();
}

void	ConfigurationParser::_parseServerBacklog(std::queue<t_token> &tokens,
	Configuration &config)
{
	t_token	token;

	token = tokens.front();
	tokens.pop();

	if (token.type != STRING)
		throw UnexpectedToken(token, "backlog", "string");
	if (tokens.front().type != SEMICOLON)
		throw UnexpectedToken(tokens.front(), "backlog", ";");

	try
	{
		config.setBacklog(tool::strings::stoui(token.content));
	}
	catch(const std::exception& e)
	{
		throw InvalidConfigFile(token.lineNbr, "backlog", e.what());
	}
	
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
		throw UnexpectedToken(tokens.front(), "location", "{");
	if (token.type != STRING)
		throw UnexpectedToken(token, "location", "string");

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

		throw UnexpectedToken(token, "LOCATION_CONTEXT");
	}

	if (token.type != CLOSE_BRACE)
		throw UnexpectedToken(token, "location", "}");

	ConfigurationParser::_completeLocation(location, config);

	config.addLocation(location);
}

void	ConfigurationParser::_completeLocation(LocationConfiguration &locConfig,
	const Configuration &servConfig)
{
	if (locConfig.getRoot().empty() && !servConfig.getRoot().empty())
		locConfig.setRoot(servConfig.getRoot());
	if (locConfig.getIndex().empty())
	{
		if (servConfig.getIndex().empty())
			locConfig.setIndex(WS_DFL_INDEX);
		else
			locConfig.setIndex(servConfig.getIndex());
	}
}

void	ConfigurationParser::_parseLocAutoindex(std::queue<t_token> &tokens,
	LocationConfiguration &config)
{
	t_token	token;

	token = tokens.front();
	tokens.pop();

	if (token.type != STRING)
		throw UnexpectedToken(token, "autoindex" ,"string");
	if (tokens.front().type != SEMICOLON)
		throw UnexpectedToken(tokens.front(), "autoindex", ";");

	if (token.content != "on" && token.content != "off")
		throw UnexpectedToken(token, "autoindex", "on / off");

	config.setAutoindex(token.content == "on");
	tokens.pop();
}

void	ConfigurationParser::_parseLocAllowedMethods(
	std::queue<t_token> &tokens, LocationConfiguration &config)
{
	t_token	token;

	if (tokens.empty())
		throw MissingToken("allow_methods", "string");

	if (tokens.front().type != STRING)
		throw UnexpectedToken(tokens.front(), "allow_methods", "string");

	while (!tokens.empty() && tokens.front().type == STRING)
	{
		token = tokens.front();
		tokens.pop();

		if (token.content == "all" && (!config.getAllowedMethods().empty()
			|| tokens.front().type == STRING))
		{
			throw InvalidConfigFile(token.lineNbr, "allow_methods",
				"Cannot mix `all` with other methods");
		}

		config.addAllowedMethod(token.content);
	}

	if (tokens.empty())
		throw MissingToken("allow_methods", ";");

	if (tokens.front().type != SEMICOLON)
		throw UnexpectedToken(tokens.front(), "allow_methods", ";");

	tokens.pop();
}

void	ConfigurationParser::_parseLocReturn(std::queue<t_token> &tokens,
	LocationConfiguration &config)
{
	t_token	token;

	if (tokens.empty())
		throw MissingToken("return", "string");
	
	token = tokens.front();
	tokens.pop();

	if (tokens.empty())
		throw MissingToken("return", "string");

	try
	{
		config.setReturnCode(tool::strings::stoi(token.content));
	}
	catch(const std::exception& e)
	{
		throw InvalidConfigFile(token.lineNbr, "return", e.what());
	}

	token = tokens.front();
	tokens.pop();

	if (token.type == SEMICOLON)
		return ;
	if (token.type != STRING)
		throw UnexpectedToken(token, "return", "string");
	
	config.setReturnMessage(token.content);

	token = tokens.front();
	tokens.pop();

	if (token.type != SEMICOLON)
		throw UnexpectedToken(token, "return", ";");
}

void	ConfigurationParser::_parseLocRoot(std::queue<t_token> &tokens,
	LocationConfiguration &config)
{
	t_token	token;

	if (tokens.empty())
		throw MissingToken("root", "string");

	token = tokens.front();
	tokens.pop();

	if (tokens.empty())
		throw MissingToken("root", ";");
	if (token.type != STRING)
		throw UnexpectedToken(token, "root", "string");
	if (tokens.front().type != SEMICOLON)
		throw UnexpectedToken(tokens.front(), "root", ";");

	try
	{
		config.setRoot(token.content);
	}
	catch (const InvalidPath &e)
	{
		throw InvalidConfigFile(token.lineNbr, "root", e.what());
	}

	tokens.pop();
}

void	ConfigurationParser::_parseLocIndex(std::queue<t_token> &tokens,
	LocationConfiguration &config)
{
	t_token	token;

	if (tokens.empty())
		throw MissingToken("index", "string");

	token = tokens.front();
	tokens.pop();

	if (tokens.empty())
		throw MissingToken("index", ";");
	if (token.type != STRING)
		throw UnexpectedToken(token, "index", "string");
	if (tokens.front().type != SEMICOLON)
		throw UnexpectedToken(tokens.front(), "index", ";");
	
	config.setIndex(token.content);
	tokens.pop();
}

void	ConfigurationParser::_parseLocFCGIPass(std::queue<t_token> &tokens,
	LocationConfiguration &config)
{
	t_token	token;

	if (tokens.empty())
		throw MissingToken("fastcgi_pass", "string");
	
	token = tokens.front();
	tokens.pop();

	if (tokens.empty())
		throw MissingToken("fastcgi_pass", ";");
	if (token.type != STRING)
		throw UnexpectedToken(token, "fastcgi_pass", "string");
	
	config.setFCGIServer(token.content);
	tokens.pop();
}

void	ConfigurationParser::_parseLocFCGIParam(std::queue<t_token> &tokens,
	LocationConfiguration &config)
{
	std::string	key;
	t_token		token;

	if (tokens.empty())
		throw MissingToken("fastcgi_param", "string");

	token = tokens.front();
	tokens.pop();

	if (tokens.empty())
		throw MissingToken("fastcgi_param", "string");
	if (token.type != STRING)
		throw UnexpectedToken(token, "fastcgi_param", "string");

	key = token.content;

	token = tokens.front();
	tokens.pop();

	if (tokens.empty())
		throw MissingToken("fastcgi_param", "string");
	if (token.type != STRING)
		throw UnexpectedToken(token, "fastcgi_param", "string");

	config.addFCGIParam(key, token.content);

	token = tokens.front();
	tokens.pop();

	if (token.type != SEMICOLON)
		throw UnexpectedToken(token, "fastcgi_param", ";");
}


/* ************************************************************************** */
/* Exceptions */

/* ************************************ */
/* ConfigException                      */
/* ************************************ */

/* Default constructor */
ConfigurationParser::ConfigException::ConfigException()
	: RuntimeError("Invalid configuration file", 10) {}

/* Message constructor */
ConfigurationParser::ConfigException::ConfigException(
		const std::string &message, int code) : RuntimeError(message, code) {}

/* Copy constructor */
ConfigurationParser::ConfigException::ConfigException(
	const ConfigException &original) : RuntimeError(original) {}


/* Destructor */
ConfigurationParser::ConfigException::~ConfigException(void) throw() {}


/* ************************************ */
/* Operator overloads */

ConfigurationParser::ConfigException
		&ConfigurationParser::ConfigException::operator=(
	const ConfigException &original)
{
	if (this == &original)
		return (*this);
	RuntimeError::operator=(original);
	return (*this);
}


/* ************************************ */
/* InvalidConfigFile                    */
/* ************************************ */

/* Default constructor */
ConfigurationParser::InvalidConfigFile::InvalidConfigFile()
	: ConfigException("Invalid configuration file", 10) {}

/* Message constructor */
ConfigurationParser::InvalidConfigFile::InvalidConfigFile(
		const std::string &message)
	: ConfigException(message, 10) {}

/* Line and message constructor */
ConfigurationParser::InvalidConfigFile::InvalidConfigFile(int lineNbr,
	const std::string &context, const std::string &message)
	: ConfigException(tool::strings::toStr(lineNbr) + ": " + context + ": "
		+ message) {}

/* Copy constructor */
ConfigurationParser::InvalidConfigFile::InvalidConfigFile(
	const InvalidConfigFile &original)
	: ConfigException(original) {}


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
	ConfigException::operator=(original);
	return (*this);
}


/* ************************************ */
/* UnexpectedToken                      */
/* ************************************ */

/* Default constructor */
ConfigurationParser::UnexpectedToken::UnexpectedToken()
	: ConfigException("Invalid token", 11) {}

/* Message constructor */
ConfigurationParser::UnexpectedToken::UnexpectedToken(const std::string &msg)
	: ConfigException(msg, 11) {}

/* Token constructor */
ConfigurationParser::UnexpectedToken::UnexpectedToken(const t_token &token,
		const std::string &context)
	: ConfigException(tool::strings::toStr(token.lineNbr)
		+ ": " + context + ": Unexpected token `" + token.content + "`", 11) {}

/* Token and message constructor */
ConfigurationParser::UnexpectedToken::UnexpectedToken(const t_token &token,
		const std::string &context, const std::string &exp)
	: ConfigException(tool::strings::toStr(token.lineNbr)
		+ ": " + context
		+ ": Unexpected token `" + token.content
		+ "`. Expected `" + exp + "`", 11) {}

/* Copy constructor */
ConfigurationParser::UnexpectedToken::UnexpectedToken(
		const UnexpectedToken &original)
	: ConfigException(original) {}


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
	ConfigException::operator=(original);
	return (*this);
}


/* ************************************ */
/* MissingToken                         */
/* ************************************ */

/* Default constructor */
ConfigurationParser::MissingToken::MissingToken()
	: ConfigException("Missing token", 12) {}

/* Token type constructor */
ConfigurationParser::MissingToken::MissingToken(const std::string &context,
		const std::string &type)
	: ConfigException(context + ": Missing token `" + type + "`", 12) {}

/* Copy constructor */
ConfigurationParser::MissingToken::MissingToken(const MissingToken &original)
	: ConfigException(original) {}


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
	ConfigException::operator=(original);
	return (*this);
}

} // namespace webserv
