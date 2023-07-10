#pragma once

// CONTAINERS
#include <set>

// GENERAL
#include <string>

// CLASSES
#include "Server.hpp"
// class server;

class Message
{
private:
	int	_emmiter;

	Message();

public:
	std::set<int>	target;
	std::string		text;
	std::string		cmd;
	std::string		cmd_param;

	Message(const Message &cpy);
	Message(const Server::Client &emmiter);
	Message(int emmiter);
	~Message();

	Message	&operator=(const Message &assign);

	int		get_emmiter() const;

};