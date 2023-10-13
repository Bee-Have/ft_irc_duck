#include "Server.hpp"
#include "ICommand.hpp"
#include <exception>
#include "Quit.hpp"
#include "Logger.hpp"

/**
 * This should never be used. Server MUST be created with a PORT and PASWORD
 */
Server::Server(void):
	_oper_name("Cthulhu"), _oper_pass("R'lyeh"),
	socket_id(socket(AF_INET, SOCK_STREAM, 0)), port(8080), pass("Dragon")
{
	if (socket_id < 0)
	{
		throw std::invalid_argument(ERR_SOCKCREATEFAIL);
	}
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(port);
	_server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(socket_id, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0)
	{
		throw std::invalid_argument(ERR_SOCKBINDFAIL);
	}
	if (listen(socket_id, MAX_CLIENT) < 0)
	{
		throw std::invalid_argument(ERR_SOCKLISTENFAIL);
	}
	_register_replies.reserve(5);
	_register_replies.push_back(RPL_WELCOME);
	_register_replies.push_back(RPL_YOURHOST);
	_register_replies.push_back(RPL_CREATED);
	_register_replies.push_back(RPL_MYINFO);
	_register_replies.push_back(RPL_ISUPPORT);
	_i_support = "CHANMODES=,ok,l,it CHANTYPES=# MODES=5 NETWORK=" SERVERNAME " NICKLEN=9 PREFIX=(o)@ CASEMAPPING=ascii TARGMAX=";
}

/**
 * @brief Construct a new Server::server object.
 * It also opens and bind the server socket
 * as well as start to listen on said socket.
 * It will also setup the function pointer for all the commands
 * 
 * @param new_port the new port of the server
 * @param new_pass the password of the server
 */
Server::Server(int new_port, char *new_pass):
	_oper_name("Cthulhu"), _oper_pass("R'lyeh"),
	socket_id(socket(AF_INET, SOCK_STREAM, 0)), port(new_port), pass(new_pass)
{
	// socket_id = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_id < 0)
	{
		throw std::invalid_argument(ERR_SOCKCREATEFAIL);
	}
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(port);
	_server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(socket_id, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0)
	{
		throw std::invalid_argument(ERR_SOCKBINDFAIL);
	}
	if (listen(socket_id, MAX_CLIENT) < 0)
	{
		throw std::invalid_argument(ERR_SOCKLISTENFAIL);
	}
	_register_replies.reserve(5);
	_register_replies.push_back(RPL_WELCOME);
	_register_replies.push_back(RPL_YOURHOST);
	_register_replies.push_back(RPL_CREATED);
	_register_replies.push_back(RPL_MYINFO);
	_register_replies.push_back(RPL_ISUPPORT);
	_i_support = "CHANMODES=,ok,l,it CHANTYPES=# MODES=5 NETWORK=" SERVERNAME " NICKLEN=9 PREFIX=(o)@ CASEMAPPING=ascii TARGMAX=";
	Logger(major_lvl) << "Server started on socket [" << socket_id << ']';
}

/**
 * This should never be used. Server MUST be created with a PORT and PASWORD
 */
Server::Server(const Server &cpy):
	_register_replies(cpy._register_replies), _i_support(cpy._i_support),
	_oper_name(cpy._oper_name), _oper_pass(cpy._oper_pass), _oper_socket(cpy._oper_socket),
	socket_id(cpy.socket_id), port(cpy.port), pass(cpy.pass)
{
	(void)cpy;
}

/**
 * @brief Destroy the Server::server object
 * @note It also closes all client sockets as well as the server socket
 */
Server::~Server(void)
{
	for (std::map<std::string, ICommand *>::iterator it = commands.begin();
		it != commands.end(); ++it)
	{
		delete it->second;
	}
	// Remove all clients
	for (std::map<int, Client>::iterator it = client_list.begin();
		it != client_list.end(); ++it)
	{
		close(it->first);
	}
	close(socket_id);
	Logger(major_lvl) << "Server stopped";
}

/**
 * This should never be used. Server MUST be created with a PORT and PASWORD
 */
Server	&Server::operator=(const Server &assign)
{
	if (this != &assign)
	{
		_server_addr = assign._server_addr;
		_client_addr = assign._client_addr;
		msgs.clear();
		msgs.insert(msgs.begin(), assign.msgs.begin(), assign.msgs.end());
		client_list.clear();
		client_list.insert(assign.client_list.begin(), assign.client_list.end());
	}
	return (*this);
}

/**
 * @brief Adds a new client to the client_list

 * @note A new socket will be given to the client using "accept()".
 * The socket will then be checked using "getsockopt()"
 */
void	Server::add_client(void)
{
	socklen_t		client_addr_len = sizeof(_client_addr);
	Client	new_client(accept(socket_id, (struct sockaddr *)&_client_addr, &client_addr_len));

	if (new_client._socket < 0)
	{
		Logger(error_type, error_lvl) << errno << ' ' << SERVERNAME << " :" << strerror(errno);
		return ;
	}

	int	socket_error;
	socklen_t optlen = sizeof(socket_error);
	if (getsockopt(new_client._socket, SOL_SOCKET, SO_ERROR, &socket_error, &optlen) >= 0)
	{
		if (socket_error != 0)
		{
			close(new_client._socket);
			return ;
		}
	}
	else
		Logger(error_type, error_lvl) << "Error : " << errno << " : " << strerror(errno);
	client_list.insert(std::make_pair(new_client._socket, new_client));
}

/**
 * @brief Delete specific client from client list.
 * 
 * @note This implies :
 * 1.deleting client fd from messages to be sent.
 * 2.closing client socket before deleting it's object
 * !(if a message send by the client exists, it will only be deleted if they are no target to send the message to)
 * 
 * @param fd the client defined by it's socket
*/
void	Server::del_client(int fd)
{
	if (client_list.find(fd) == client_list.end())
		return ;
	client_list.erase(client_list.find(fd));
	if (_oper_socket.find(fd) == _oper_socket.end())
		_oper_socket.erase(fd);
	close(fd);
	Logger(major_lvl) << "Client disconnected on socket " << fd;
}

/**
 * @brief Finds the maximum value fd existing in client_list and returns it for "select()"
 * 
 * @return the maximum fd found in client_list
 */
int	Server::get_max_fd(void) const
{
	int	max_fd = socket_id;

	for (std::map<int, Client>::const_iterator it = client_list.begin(); it != client_list.end(); ++it)
	{
		if (it->first > max_fd)
			max_fd = it->first;
	}
	return (max_fd);
}

/**
 * @brief Adds server fd and all client fd we wish to read on for "select()"
 * @note (this will always return all fds in client_list)
 * 
 * @return fd_set of the client fd and server socket
 */
fd_set	Server::get_read_fds(void) const
{
	fd_set	read_fds;

	FD_ZERO(&read_fds);
	FD_SET(socket_id, &read_fds);
	for (std::map<int, Client>::const_iterator it = client_list.begin()
		; it != client_list.end(); ++it)
	{
		FD_SET(it->first, &read_fds);
	}
	return (read_fds);
}

/**
 * @brief Adds all fds from messages to be sent for "select()"
 * @note this is based on the fd found in message::target
 * 
 * @return fd_set of the clients fd we want to write on
 */
fd_set	Server::get_write_fds(void) const
{
	fd_set	write_fds;

	FD_ZERO(&write_fds);

	for (std::vector<Message>::const_iterator	it_msg = msgs.begin();
		it_msg != msgs.end(); ++it_msg)
	{
		for (std::set<int>::iterator it_fd = it_msg->target.begin();
			it_fd != it_msg->target.end(); ++it_fd)
		{
			if (FD_ISSET(*it_fd, &write_fds) == 0)
				FD_SET(*it_fd, &write_fds);
		}
	}
	return (write_fds);
}

/**
 * @brief Checks wethere param nickname exists in client_list
 * 
 * @param nickname the nickname to check
 * @return int -1 if no client is found.
 * else it will return the fd/socket of the client
 */
int	Server::get_client_by_nickname(std::string nickname)
{
	for (std::map<int, Client>::iterator it = client_list.begin();
		it != client_list.end(); ++it)
	{
		if (nickname.compare(it->second.nickname) == 0)
			return (it->first);
	}
	return (-1);
}

Channel*	Server::get_channel_by_name(std::string nickname)
{
	std::map<std::string, Channel>::iterator channel(_channel_list.find(nickname));
	if (channel == _channel_list.end())
		return (NULL);
	return (&channel->second);
}

std::string	Server::oper_command_check(int client, std::string oper, std::string pass)
{
	if (_oper_name != oper)
		return (ERR_NOSUCHOPER);
	if (_oper_pass != pass)
		return (ERR_PASSWDMISMATCH);
	_oper_socket.insert(client);
	return (RPL_YOUREOPER);
}

std::string Server::current_date(void)
{
	std::time_t			time = std::time(0);
	std::tm* now = std::localtime(&time);
	std::stringstream	ss;
	std::string			date;

	ss << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday;
	ss >> date;

	return (date);
}

void	Server::register_client_if_able(int client)
{
	Client	*check(&client_list.find(client)->second);
	Message	register_warning(*check);
	std::vector<std::string>	_register_replace;

	if (check->get_is_registered() == true)
		return ;
	if (check->nickname.empty() == true)
		return ;
	if (check->_realname.empty() == true && check->_username.empty() == true)
		return ;

	check->_is_registered = true;
	_register_replace.push_back(check->nickname);
	_register_replace.push_back(check->host);
	_register_replace.push_back(current_date());
	_register_replace.push_back(_i_support);
	register_warning.reply_format(_register_replies, _register_replace);
	msgs.push_back(register_warning);
}
