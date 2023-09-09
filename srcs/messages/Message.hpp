#pragma once

// CONTAINERS
#include <set>

// GENERAL
#include <vector>
#include <string>

// CLASSES
#include "define.hpp"
// #include "Client.hpp"
// #include "Server.hpp"
// class server;

class Client;

class Message
{
private:
	// TODO wouldn't it be more practical if this value was a public const static ?
	int	_emitter;

	Message();
	
	void	replace_rpl_err_text(std::string replace);

public:
	// TODO :maybe change this to nickname since there are : nickname, username, realname
	std::string	emitter_name;
	std::set<int>	target;
	std::string		text;
	std::string		cmd;
	std::string		cmd_param;

	Message(const Message &cpy);
	Message(const Client &emitter);
	Message(int emitter, std::string p_name);
	~Message();

	Message	&operator=(const Message &assign);

	int		get_emitter() const;

	void	reply_format(std::string reply, std::string replace, int socket);
	void	reply_format(std::vector<std::string> &errors, std::vector<std::string> &replace);
	void	reply_replace_curly_brackets(std::string &reply, int replace_count);
};