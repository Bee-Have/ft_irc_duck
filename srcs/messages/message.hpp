#pragma once

// CONTAINERS
#include <set>

// GENERAL
#include <string>

// CLASSES
#include "server.hpp"
// class server;

class message
{
private:
	int	_emmiter;
	struct command {
		std::string	prefix;
		std::string	name;
		std::string	params;
	};

	message();

public:
	command			cmd;
	std::set<int>	target;
	std::string		text;

	message(const message &cpy);
	message(const server::client &emmiter);
	message(int emmiter);
	~message();

	message	&operator=(const message &assign);

	int		get_emmiter() const;

};