#pragma once

#include <string>

class Client
{
friend class Server;
friend struct Pass;
friend struct User;
friend struct Mode;
private:
	Client();
	Client(int new_socket);

	int			_socket;
	bool		_is_authenticated;
	bool		_is_registered;
	std::string	_username;
	std::string	_realname;

	bool	_is_invisible;

public:
	std::string	nickname;
	std::string	host;

	Client(const Client &cpy);
	~Client();

	Client	&operator=(const Client &assign);

	int		get_socket() const;
	bool	get_is_registered() const;
	bool	get_is_invisible() const;
	bool	get_is_authenticated() const;
	std::string	get_username() const;
};
