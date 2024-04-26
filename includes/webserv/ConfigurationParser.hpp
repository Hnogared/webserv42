/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigurationParser.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 10:19:18 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/26 13:43:27 by hnogared         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATIONPARSER_HPP
# define CONFIGURATIONPARSER_HPP

# include <cerrno>
# include <string.h>

# include <string>
# include <vector>
# include <map>
# include <queue>
# include <fstream>
# include <iostream>

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

		/* Static methods */
		static std::vector<Configuration> parse(const std::string &path);


	private:
		/* Private methods */
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
		static void	_parseListen(std::queue<t_token> &tokens,
			Configuration &config);


		/* [delete] */
		ConfigurationParser();
		ConfigurationParser(const ConfigurationParser &original);
		~ConfigurationParser();
		ConfigurationParser	&operator=(const ConfigurationParser &original);


	/* Exceptions */
	public:
		class	InvalidConfigFile : public RuntimeError
		{
			public:
				/* Constructors */
				explicit InvalidConfigFile();
				explicit InvalidConfigFile(const std::string &message);
				InvalidConfigFile(const InvalidConfigFile &original);
	
				/* Destructor */
				~InvalidConfigFile(void) throw();
	
				/* Operator overloads */
				InvalidConfigFile	&operator=(
					const InvalidConfigFile &original);
		}; // class InvalidConfigFile
	
		class	UnexpectedToken : public RuntimeError
		{
			public:
				/* Constructors */
				explicit UnexpectedToken();
				explicit UnexpectedToken(const std::string &message);
				UnexpectedToken(const t_token &token);
				UnexpectedToken(const t_token &token, const std::string &exp);
				UnexpectedToken(const UnexpectedToken &original);
	
				/* Destructor */
				~UnexpectedToken(void) throw();
	
				/* Operator overloads */
				UnexpectedToken	&operator=(const UnexpectedToken &original);
		}; // class UnexpectedToken
}; // class ConfigurationParser

} // namespace webserv

#endif // CONFIGURATIONPARSER_HPP