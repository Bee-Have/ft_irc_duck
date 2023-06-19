#include "server.hpp"

server::client::client(void)
{}

server::client::client(int new_socket): _socket(new_socket), _is_registered(false)
{}

server::client::client(const server::client &cpy): _socket(cpy._socket), _is_registered(cpy._is_registered)
{}

server::client::~client(void)
{}

server::client	&server::client::operator=(const server::client &assign)
{
	if (this != &assign)
	{
		_socket = assign._socket;
		_is_registered = assign._is_registered;
		_nickname.assign(assign._nickname);
		_realname.assign(assign._realname);
		_username.assign(assign._username);
	}
	return (*this);
}

int	server::client::get_socket(void) const
{
	return (_socket);
}
