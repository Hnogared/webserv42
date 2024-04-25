/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigurationParser.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnogared <hnogared@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 10:19:18 by hnogared          #+#    #+#             */
/*   Updated: 2024/04/25 12:32:41 by hnogared         ###   ########.fr       */
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

# include "exceptions.hpp"
# include "Configuration.hpp"

# include <iostream>

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
			std::queue<t_token> &tokens);
		static void	_parseTokens(std::queue<t_token> &tokens,
			std::vector<Configuration> &configurations);
		static void	_parseServerConfig(std::queue<t_token> &tokens,
			std::vector<Configuration> &configurations);


		/* [delete] */
		ConfigurationParser(void);
		ConfigurationParser(const ConfigurationParser &original);
		~ConfigurationParser(void);
		ConfigurationParser	&operator=(const ConfigurationParser &original);


	/* Exceptions */
	public:
		class	InvalidConfigFile : public RuntimeError
		{
			public:
				/* Constructors */
				explicit InvalidConfigFile(int code = 7);
				explicit InvalidConfigFile(const std::string &message,
					int code = 7);
				InvalidConfigFile(const InvalidConfigFile &original);
	
				/* Destructor */
				~InvalidConfigFile(void) throw();
	
				/* Operator overloads */
				InvalidConfigFile	&operator=(
					const InvalidConfigFile &original);
		}; // class InvalidConfigFile
}; // class ConfigurationParser

} // namespace webserv

#endif // CONFIGURATIONPARSER_HPP