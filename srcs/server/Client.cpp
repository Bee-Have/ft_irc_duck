#include "Client.hpp"
#include "Logger.hpp"

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
Client::Client(int new_socket) :
	_socket(new_socket), _is_authenticated(false),
	_is_registered(false), _is_invisible(false)
{
	Logger(major_lvl) << "New client connected on socket [" << _socket << ']';
}

/**
 * @brief Construct a new Client::Client object
 * @note this constructor is called by the containers upon manipulation of client_list

 * @param cpy the client to construct the newclient from
 */
Client::Client(const Client &cpy) :
	_socket(cpy._socket), _is_authenticated(cpy._is_authenticated),
	_is_registered(cpy._is_registered), _username(cpy._username),
	_realname(cpy._realname), _is_invisible(cpy._is_invisible),
	nickname(cpy.nickname), host(cpy.host)
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
		_is_authenticated = assign._is_authenticated;
		_is_registered = assign._is_registered;
		_realname.assign(assign._realname);
		_username.assign(assign._username);
		_is_invisible = assign._is_invisible;
		nickname.assign(assign.nickname);
		host.assign(assign.host);
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

bool	Client::get_is_invisible(void) const
{
	return (_is_invisible);
}

bool	Client::get_is_authenticated(void) const
{
	return (_is_authenticated);
}

std::string	Client::get_username(void) const
{
	return (_username);
}
