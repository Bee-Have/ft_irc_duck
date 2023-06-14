#pragma once

// CONTAINERS
#include <set>

// GENERAL
#include <string>

// CLASSES
#include "client.hpp"

class message
{
private:
	int	_emmiter;
	struct commands {
		std::string	prefix;
		std::string	name;
		std::string	params;
	};

	message();

public:
	commands		command;
	std::set<int>	target;
	std::string		text;

	message(const message &cpy);
	message(const client &emmiter);
	message(int emmiter);
	~message();

	message	&operator=(const message &assign);

	int		get_emmiter() const;

};