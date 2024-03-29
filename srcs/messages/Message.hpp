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
	friend struct Pass;
	friend struct Nick;
	friend struct User;
	friend struct Quit;
private:
	int	_emitter;
	std::string	_username;

	Message();
	
	void	replace_rpl_err_text(std::string replace);
	void	replace_prefix(std::string text, int socket);

public:

	std::string	emitter_nick;
	std::string	host;
	std::set<int>	target;
	std::string		text;
	std::string		cmd;
	std::string		cmd_param;

	Message(const Message &cpy);
	Message(const Client &emitter);
	~Message();

	Message	&operator=(const Message &assign);

	int		get_emitter() const;

	void	reply_format(std::string reply, std::string replace, int socket);
	void	reply_format(std::vector<std::string> &errors, std::vector<std::string> &replace);
	void	reply_replace_curly_brackets(std::string &reply, int replace_count);
};