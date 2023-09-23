#include "Server.hpp"
#include "ICommand.hpp"

/**
 * This should never be used. Server MUST be created with a PORT and PASWORD
 */
Server::Server(void): _oper_name("Cthulhu"), _oper_pass("R'lyeh"), _oper_socket(-1), socket_id(socket(AF_INET, SOCK_STREAM, 0)), port(8080), pass("Dragon")
{
	if (socket_id < 0)
	{
		std::cerr << ERR_SOCKCREATEFAIL;
		return ;
	}
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(port);
	_server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(socket_id, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0)
	{
		std::cerr << ERR_SOCKBINDFAIL;
		return ;
	}
	if (listen(socket_id, MAX_CLIENT) < 0)
	{
		std::cerr << ERR_SOCKLISTENFAIL;
		return ;
	}
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
Server::Server(int new_port, char *new_pass): _oper_name("Cthulhu"), _oper_pass("R'lyeh"), _oper_socket(-1), socket_id(socket(AF_INET, SOCK_STREAM, 0)), port(new_port), pass(new_pass)
{
	// socket_id = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_id < 0)
	{
		std::cerr << ERR_SOCKCREATEFAIL;
		return ;
	}
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(port);
	_server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(socket_id, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0)
	{
		std::cerr << ERR_SOCKBINDFAIL;
		return ;
	}
	if (listen(socket_id, MAX_CLIENT) < 0)
	{
		std::cerr << ERR_SOCKLISTENFAIL;
		return ;
	}
}

/**
 * This should never be used. Server MUST be created with a PORT and PASWORD
 */
Server::Server(const Server &cpy): socket_id(cpy.socket_id), port(cpy.port), pass(cpy.pass)
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
	close(socket_id);
	// client_list.clear();
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
		std::cerr << errno << ' ' << SERVERNAME << " :" << strerror(errno) << "\r\n";
		return ;
	}

	int	socket_error;
	socklen_t optlen = sizeof(socket_error);
	if (getsockopt(new_client._socket, SOL_SOCKET, SO_ERROR, &socket_error, &optlen) >= 0)
	{
		if (socket_error != 0)
		{
			std::cout << "INVALID SOCKET :(" << std::endl;
			close(new_client._socket);
			return ;
		}
	}
	else
		std::cerr << "Error : " << errno << " : " << strerror(errno) << std::endl;

	std::cout << "NEW CLIENT :D" << std::endl;
	client_list.insert(std::make_pair(new_client._socket, new_client));
}

void	Server::del_client_from_msgs(int fd)
{
	for (std::vector<Message>::iterator it = msgs.begin(); it != msgs.end(); ++it)
	{
		if (it->target.size() == 1 && it->target.find(fd) != it->target.end())
		{
			it = msgs.erase(it);
			if (msgs.empty() == true)
				break ;
		}
		else if (it->target.find(fd) != it->target.end())
			it->target.erase(it->target.find(fd));
	}
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
	Message	part_msg(client_list.find(fd)->second);
	// delete client from messages
	del_client_from_msgs(fd);
	// delete client from channels
	for (std::map<std::string, Channel>::iterator it = _channel_list.begin();
		it != _channel_list.end(); ++it)
	{
		if (it->second._clients.find(fd) != it->second._clients.end())
		{
			if (it->second._is(it->second._clients.find(fd)->second, it->second.MEMBER) == true)
			{
				if (part_msg.cmd_param.empty() == true)
					part_msg.cmd_param = it->first;
				else
					part_msg.cmd_param.append("," + it->first);
			}
			else
				it->second._clients.erase(it->second._clients.find(fd));
		}
	}
	if (part_msg.cmd_param.empty() == false)
		commands["PART"]->execute(part_msg);
	close(fd);
	client_list.erase(client_list.find(fd));
	std::cout << "BYE BYE CLIENT" << std::endl;
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

std::string	Server::oper_command_check(int client, std::string oper, std::string pass)
{
	if (_oper_socket != -1)
		return (ERR_CANNOTBECOMEOPER);
	if (_oper_name != oper)
		return (ERR_NOSUCHOPER);
	if (_oper_pass != pass)
		return (ERR_PASSWDMISMATCH);
	_oper_socket = client;
	return (RPL_YOUREOPER);
}
