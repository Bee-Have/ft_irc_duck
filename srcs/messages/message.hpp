#pragma once

// CONTAINERS
#include <set>

// GENERAL
#include <string>

class message
{
private:
	int	_emmiter;

	message();

public:
	std::set<int>	target;
	std::string		text;

	message(const message &cpy);
	message(int new_emmiter);
	~message();

	message	&operator=(const message &assign);

	int		get_emmiter() const;

};