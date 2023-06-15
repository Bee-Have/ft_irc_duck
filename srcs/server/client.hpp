#pragma once

#include <string>
#include <unistd.h>

class client
{
private:
	client();

	int			_socket;

public:
	std::string	nickname;
	bool		registered;

	client(int new_socket);
	client(const client &cpy);
	~client();

	client	&operator=(const client &assign);

	int		get_socket() const;
};