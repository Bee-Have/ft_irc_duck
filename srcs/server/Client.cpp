#include "Client.hpp"

/**
 * @brief Construct a new Client::Client object
 * @note this constructor should never be used.
 * Clients should always be created with either socket or by copy
 */
Client::Client(void)
{}

/**
 * @brief construct a new Client::Client object
 * @note this constructor is called by Server::add_client with "access()"
 * 
 * @param new_socket the socket the new client is identified by
 */
Client::Client(int new_socket): _socket(new_socket), _is_registered(false)
{}

/**
 * @brief Construct a new Client::Client object
 * @note this constructor is called by the containers upon manipulation of client_list

 * @param cpy the client to construct the newclient from
 */
Client::Client(const Client &cpy): _socket(cpy._socket), _is_registered(cpy._is_registered)
{}

/**
 * @brief Destroy the Client::Client object
 * @note you will notice we are not closing the client socket here.
 * This is normal. If we close it here when making copies of a client
 *  it will close the socket and therefore invalidate the purpose of the copy
 */
Client::~Client(void)
{}

Client	&Client::operator=(const Client &assign)
{
	if (this != &assign)
	{
		_socket = assign._socket;
		_is_registered = assign._is_registered;
		nickname.assign(assign.nickname);
		_realname.assign(assign._realname);
		_username.assign(assign._username);
	}
	return (*this);
}

int	Client::get_socket(void) const
{
	return (_socket);
}

bool	Client::get_is_registered(void) const
{
	return (_is_registered);
}
