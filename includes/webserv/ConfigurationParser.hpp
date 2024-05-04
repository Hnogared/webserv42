/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigurationParser.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 10:19:18 by hnogared          #+#    #+#             */
/*   Updated: 2024/05/04 17:56:11 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATIONPARSER_HPP
# define CONFIGURATIONPARSER_HPP

# include <cerrno>
# include <cstring>

# include <string>
# include <vector>
# include <map>
# include <queue>
# include <fstream>
# include <iostream>

# include "Harl.hpp"
# include "strings.hpp"
# include "exceptions.hpp"
# include "Configuration.hpp"

namespace	webserv
{

class	ConfigurationParser
{
	public:
		typedef enum	e_contextType
		{
			GLOBAL,
			HTTP,
			SERVER,
			LOCATION
		}	t_contextType;

		typedef enum	e_tokenType
		{
			OPEN_BRACE,
			CLOSE_BRACE,
			SEMICOLON,
			STRING
		}	t_tokenType;

		typedef struct	s_token
		{
			int			lineNbr;
			t_tokenType	type;
			std::string	content;
		}	t_token;

		/* Typedefs */
		typedef void (*t_serverDirectiveParser)(std::queue<t_token>&,
			Configuration&);
		typedef void (*t_locationDirectiveParser)(std::queue<t_token>&,
			LocationConfiguration&);

		/* Static methods */
		static std::vector<Configuration> *parse(const std::string &path);


	private:
		/* Private attributes */
		static const std::map<std::string, t_serverDirectiveParser>
			_serverDirectives;
		static const std::map<std::string, t_locationDirectiveParser>
			_locationDirectives;


		/* Private methods */
		static std::map<std::string, t_serverDirectiveParser>
			_initializeServerDirectives(void);
		static std::map<std::string, t_locationDirectiveParser>
			_initializeLocationDirectives(void);

		static void	_openConfigFile(const std::string &path,
			std::ifstream &file);

		static std::queue<t_token>	_tokenizeFile(std::ifstream &file,
			std::queue<t_token> &tokens);
		static void	_tokenizeLine(const std::string &line,
			std::queue<t_token> &tokens, int lineNbr);
		
		static void	_parseTokens(std::queue<t_token> &tokens,
			std::vector<Configuration> &configurations);
		static void	_parseHttpConfig(std::queue<t_token> &tokens,
			std::vector<Configuration> &configurations);

		static void	_parseServerConfig(std::queue<t_token> &tokens,
			std::vector<Configuration> &configurations);
		static void	_parseServerListen(std::queue<t_token> &tokens,
			Configuration &config);
		static void	_parseServerNames(std::queue<t_token> &tokens,
			Configuration &config);
		static void	_parseServerErrorPage(std::queue<t_token> &tokens,
			Configuration &config);
		static void	_parseServerClientMaxBodySize(std::queue<t_token> &tokens,
			Configuration &config);
		static void	_parseServerRoot(std::queue<t_token> &tokens,
			Configuration &config);
		static void	_parseServerIndex(std::queue<t_token> &tokens,
			Configuration &config);
		static void	_parseServerBacklog(std::queue<t_token> &tokens,
			Configuration &config);
		static void	_parseServerLocation(std::queue<t_token> &tokens,
			Configuration &config);

		static void	_completeLocation(LocationConfiguration &locConfig,
			const Configuration &servConfig);
		static void	_parseLocAutoindex(std::queue<t_token> &tokens,
			LocationConfiguration &config);
		static void	_parseLocAllowedMethods(std::queue<t_token> &tokens,
			LocationConfiguration &config);
		static void	_parseLocReturn(std::queue<t_token> &tokens,
			LocationConfiguration &config);
		static void	_parseLocRoot(std::queue<t_token> &tokens,
			LocationConfiguration &config);
		static void	_parseLocIndex(std::queue<t_token> &tokens,
			LocationConfiguration &config);
		static void	_parseLocFCGIPass(std::queue<t_token> &tokens,
			LocationConfiguration &config);
		static void	_parseLocFCGIParam(std::queue<t_token> &tokens,
			LocationConfiguration &config);

		/* [delete] */
		ConfigurationParser();
		ConfigurationParser(const ConfigurationParser &original);
		~ConfigurationParser();
		ConfigurationParser	&operator=(const ConfigurationParser &original);


	/* Exceptions */
	public:
		class	ConfigException : public RuntimeError
		{
			public:
				/* Constructors */
				explicit ConfigException();
				explicit ConfigException(const std::string &message,
					int code = 10);
				ConfigException(const ConfigException &original);

				/* Destructor */
				~ConfigException() throw();
		
				/* Operator overloads */
				ConfigException	&operator=(const ConfigException &original);
		}; // class ConfigException

		class	InvalidConfigFile : public ConfigException
		{
			public:
				/* Constructors */
				explicit InvalidConfigFile();
				explicit InvalidConfigFile(const std::string &message);
				InvalidConfigFile(int lineNbr, const std::string &context,
					const std::string &message);
				InvalidConfigFile(const InvalidConfigFile &original);
	
				/* Destructor */
				~InvalidConfigFile(void) throw();
	
				/* Operator overloads */
				InvalidConfigFile	&operator=(
					const InvalidConfigFile &original);
		}; // class InvalidConfigFile
	
		class	UnexpectedToken : public ConfigException
		{
			public:
				/* Constructors */
				explicit UnexpectedToken();
				explicit UnexpectedToken(const std::string &message);
				UnexpectedToken(const t_token &token,
					const std::string &context);
				UnexpectedToken(const t_token &token,
					const std::string &context, const std::string &exp);
				UnexpectedToken(const UnexpectedToken &original);
	
				/* Destructor */
				~UnexpectedToken(void) throw();
	
				/* Operator overloads */
				UnexpectedToken	&operator=(const UnexpectedToken &original);
		}; // class UnexpectedToken

		class	MissingToken : public ConfigException
		{
			public:
				/* Constructors */
				explicit MissingToken();
				explicit MissingToken(const std::string &context,
					const std::string &type);
				MissingToken(const t_token &token);
				MissingToken(const t_token &token, const std::string &exp);
				MissingToken(const MissingToken &original);
	
				/* Destructor */
				~MissingToken(void) throw();
	
				/* Operator overloads */
				MissingToken	&operator=(const MissingToken &original);
		}; // class MissingToken
}; // class ConfigurationParser

} // namespace webserv

#endif // CONFIGURATIONPARSER_HPP