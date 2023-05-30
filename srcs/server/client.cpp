#include "client.hpp"

client::client(void)
{}

client::client(int new_socket): _socket(new_socket)
{}

client::client(const client &cpy): _socket(cpy._socket)
{}

client::~client(void)
{}

client	&client::operator=(const client &assign)
{
	if (this != &assign)
		_socket = assign._socket;
	return (*this);
}

int	client::get_socket(void)
{
	return (_socket);
}
