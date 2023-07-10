#include "Server.hpp"

/**
 * @brief Construct a new Server::Client::Client object
 * @note this constructor should never be used.
 * Clients should always be created with either socket or by copy
 */
Server::Client::Client(void)
{}

/**
 * @brief construct a new Server::Client::Client object
 * @note this constructor is called by Server::add_client with "access()"
 * 
 * @param new_socket the socket the new client is identified by
 */
Server::Client::Client(int new_socket): _socket(new_socket), _is_registered(false)
{}

/**
 * @brief Construct a new Server::Client::Client object
 * @note this constructor is called by the containers upon manipulation of Server::Client_list

 * @param cpy the client to construct the newclient from
 */
Server::Client::Client(const Server::Client &cpy): _socket(cpy._socket), _is_registered(cpy._is_registered)
{}

/**
 * @brief Destroy the Server::Client::Client object
 * @note you will notice we are not closing the client socket here.
 * This is normal. If we close it here when making copies of a client
 *  it will close the socket and therefore invalidate the purpose of the copy
 */
Server::Client::~Client(void)
{}

Server::Client	&Server::Client::operator=(const Server::Client &assign)
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

int	Server::Client::get_socket(void) const
{
	return (_socket);
}
