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

	message();

public:
	std::set<int>	target;
	std::string		text;
	std::string		cmd;
	std::string		cmd_param;

	message(const message &cpy);
	message(const server::client &emmiter);
	message(int emmiter);
	~message();

	message	&operator=(const message &assign);

	int		get_emmiter() const;

};