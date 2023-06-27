#include "server.hpp"

/**
 * @brief Construct a new server::client::client object
 * @note this constructor should never be used.
 * Clients should always be created with either socket or by copy
 */
server::client::client(void)
{}

/**
 * @brief construct a new server::client::client object
 * @note this constructor is called by server::add_client with "access()"
 * 
 * @param new_socket the socket the new client is identified by
 */
server::client::client(int new_socket): _socket(new_socket), _is_registered(false)
{}

/**
 * @brief Construct a new server::client::client object
 * @note this constructor is called by the containers upon manipulation of server::client_list

 * @param cpy the client to construct the newclient from
 */
server::client::client(const server::client &cpy): _socket(cpy._socket), _is_registered(cpy._is_registered)
{}

/**
 * @brief Destroy the server::client::client object
 * @note you will notice we are not closing the client socket here.
 * This is normal. If we close it here when making copies of a client
 *  it will close the socket and therefore invalidate the purpose of the copy
 */
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
