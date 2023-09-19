#pragma once

#include <string>

class Client
{
friend struct Pass;
friend struct User;
friend struct Server;
private:
	Client();
	Client(int new_socket);

	int			_socket;
	bool		_is_registered;
	std::string	_username;
	std::string	_realname;

public:
	std::string	nickname;

	Client(const Client &cpy);
	~Client();

	Client	&operator=(const Client &assign);

	int		get_socket() const;
	bool	get_is_registered() const;
};
