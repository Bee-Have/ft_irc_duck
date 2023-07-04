#include "server.hpp"
#include "message.hpp"

/**
 * This should never be used. Server MUST be created with a PORT and PASSWORD
 */
server::server(void)
{}

/**
 * @brief Construct a new server::server object.
 * It also opens and bind the server socket
 * as well as start to listen on said socket.
 * It will also setup the function pointer for all the commands
 * 
 * @param new_port the new port of the server
 * @param new_pass the password of the server
 */
server::server(int new_port, char *new_pass): _port(new_port), _pass(new_pass)
{
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0)
	{
		std::cerr << ERR_SOCKCREATEFAIL;
		return ;
	}
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(_port);
	_server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(_socket, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0)
	{
		std::cerr << ERR_SOCKBINDFAIL;
		return ;
	}
	if (listen(_socket, MAX_CLIENT) < 0)
	{
		std::cerr << ERR_SOCKLISTENFAIL;
		return ;
	}

	commands["PASS"] = &server::pass;
	commands["NICK"] = &server::nick;
	commands["USER"] = &server::user;
	commands["PRIVMSG"] = &server::privmsg;
	commands["PING"] = &server::ping;
}

server::server(const server &cpy)
{
	(void)cpy;
}

/**
 * @brief Destroy the server::server object
 * @note It also closes all client sockets as well as the server socket
 */
server::~server(void)
{
	close(_socket);
	client_list.clear();
}

server	&server::operator=(const server &assign)
{
	if (this != &assign)
	{
		_port = assign._port;
		_pass.assign(assign._pass);
		_socket = assign._socket;
		_server_addr = assign._server_addr;
		_client_addr = assign._client_addr;
		msgs.clear();
		msgs.insert(msgs.begin(), assign.msgs.begin(), assign.msgs.end());
		client_list.clear();
		client_list.insert(assign.client_list.begin(), assign.client_list.end());
	}
	return (*this);
}

int	server::get_socket(void) const
{
	return (_socket);
}

/**
 * @brief Adds a new client to the server::client_list

 * @note A new socket will be given to the client using "accept()".
 * The socket will then be checked using "getsockopt()"
 */
void	server::add_client(void)
{
	socklen_t		client_addr_len = sizeof(_client_addr);
	server::client	new_client(accept(_socket, (struct sockaddr *)&_client_addr, &client_addr_len));

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
void	server::del_client(int fd)
{
	for (std::vector<message>::iterator it = msgs.begin(); it != msgs.end(); ++it)
	{
		if (it->get_emmiter() == fd && (it->target.empty() == true
			|| (it->target.size() == 1 && it->target.find(fd) != it->target.end())))
		{
			it = msgs.erase(it);
			if (msgs.empty() == true)
				break ;
		}
		else if (it->target.find(fd) != it->target.end())
			it->target.erase(it->target.find(fd));
	}
	close(fd);
	client_list.erase(client_list.find(fd));
	std::cout << "BYE BYE CLIENT" << std::endl;
}

/**
 * @brief Finds the maximum value fd existing in server::client_list and returns it for "select()"
 * 
 * @return the maximum fd found in server::client_list
 */
int	server::get_max_fd(void) const
{
	int	max_fd = _socket;

	for (std::map<int, server::client>::const_iterator it = client_list.begin(); it != client_list.end(); ++it)
	{
		if (it->first > max_fd)
			max_fd = it->first;
	}
	return (max_fd);
}

/**
 * @brief Adds server fd and all client fd we wish to read on for "select()"
 * @note (this will always return all fds in server::client_list)
 * 
 * @return fd_set of the client fd and server socket
 */
fd_set	server::get_read_fds(void) const
{
	fd_set	read_fds;

	FD_ZERO(&read_fds);
	FD_SET(_socket, &read_fds);
	for (std::map<int, server::client>::const_iterator it = client_list.begin()
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
fd_set	server::get_write_fds(void) const
{
	fd_set	write_fds;

	FD_ZERO(&write_fds);

	for (std::vector<message>::const_iterator	it_msg = msgs.begin();
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
 * @brief Checks wethere param nickname exists in server::client_list
 * 
 * @param nickname the nickname to check
 * @return int -1 if no client is found.
 * else it will return the fd/socket of the client
 */
int	server::_get_client_by_nickname(std::string nickname)
{
	for (std::map<int, server::client>::iterator it = client_list.begin();
		it != client_list.end(); ++it)
	{
		if (nickname.compare(it->second._nickname) == 0)
			return (it->first);
	}
	return (-1);
}

/**
 * private fuction
 * @brief Setup an error message with the parameters
 * 
 * @param msg the message to turn into an error
 * @param prefix if there is something to replace in the prefix of the error
 * @param error the error declared in : "define.hpp"
 */
void	server::error_message(message &msg, std::string prefix, std::string error)
{
	int	begin = 0;
	int	end = 0;

	msg.target.clear();
	msg.target.insert(msg.get_emmiter());

	msg.text = error;
	msg.text.replace(msg.text.find("<client>"), 8, client_list.find(msg.get_emmiter())->second._nickname);
	if (prefix.empty() == false)
	{
		begin = msg.text.find('<');
		end = msg.text.find('>') - begin;
		msg.text.replace(begin, end, prefix);
	}
}

/**
 * @brief Attempts to register a client into our server
 * 
 * @param msg the message containing the command.
 * this command will call "error_message()" if msg.cmd.param does not fit server password
 */
void	server::pass(message &msg)
{
	if (client_list.find(msg.get_emmiter())->second._is_registered == true)
	{
		error_message(msg, "", ERR_ALREADYREGISTRED);
		return ;
	}
	if (_pass.compare(msg.cmd.params) != 0)
	{
		error_message(msg, "", ERR_BADPASS);
		return ;
	}
	client_list.find(msg.get_emmiter())->second._is_registered = true;
	msg.text.clear();
}

/**
 * @brief checks wether nickname is allowed.
 * @note check irssi RFC for specification of nickname policy.
 * This function is only called and used by "nick()"
 * 
 * @param nickname the nickname to check
 * @return true if the nickname is allowed
 * @return false if the nickname isn't allowed
 */
static bool	is_nickname_allowed(std::string nickname)
{
	if (nickname.size() > 9)
		return (false);
	if (std::isdigit(nickname[0]) != 0 || nickname[0] == '-')
		return (false);
	if (nickname.find_first_not_of(NICK_GOOD_CHARACTERS) != std::string::npos)
		return (false);
	return (true);
}

/**
 * @brief changes the nickname of a user ("msg.get_emmiter()")
 * 
 * @param msg the message containing the command
 * @note if the nickname in msg.cmd.param is not allowed or missing or already in use, "error_message()" will be called
 */
void	server::nick(message &msg)
{
	std::string	nickname;

	if (client_list.find(msg.get_emmiter())->second._is_registered == false)
	{
		error_message(msg, "", ERR_UNREGISTERED);
		return ;
	}
	if (msg.cmd.params.empty() == true)
	{
		error_message(msg, "", ERR_NONICKNAMEGIVEN);
		return ;
	}
	nickname = msg.cmd.params.substr(0, msg.cmd.params.find(' '));
	if (is_nickname_allowed(nickname) == false)
	{
		error_message(msg, nickname, ERR_ERRONEUSNICKNAME);
		return ;
	}
	if (_get_client_by_nickname(nickname) != -1)
	{
		error_message(msg, nickname, ERR_NICKNAMEINUSE);
		return ;
	}
	client_list.find(msg.get_emmiter())->second._nickname = nickname;
	msg.text.clear();
	// std::cout << "worked:" << client_list.find(msg.get_emmiter())->second._nickname << std::endl;
}

/**
 * @brief assigns a username and realname to a specific client ("msg.get_emmiter()")
 * 
 * @param msg the message containing the command
 * @note if client is unregistered of no username or realname is given, call "error_message()".
 * Likewise if everything is here replies will be added to msg.text as an answer
 */
void	server::user(message &msg)
{
	server::client		&tmp(client_list.find(msg.get_emmiter())->second);
	message				reply(msg.get_emmiter());
	std::time_t			time = std::time(0);
	std::tm				*now = std::localtime(&time);
	std::stringstream	ss;
	std::string			date;

	ss << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday;
	ss >> date;
	if (tmp._is_registered == false)
	{
		error_message(msg, "", ERR_UNREGISTERED);
		return ;
	}
	if (tmp._nickname.empty() == true)
	{
		error_message(msg, "", ERR_NONICKNAMEGIVEN);
		return ;
	}
	if (msg.cmd.params.find(':') == std::string::npos
		|| msg.cmd.params.find(' ') == std::string::npos)
	{
		error_message(msg, msg.cmd.name, ERR_NEEDMOREPARAMS);
		return ;
	}
	if (tmp._realname.empty() == false)
	{
		error_message(msg, "", ERR_ALREADYREGISTRED);
		return ;
	}

	tmp._username = msg.cmd.params.substr(0, msg.cmd.params.find(' '));
	tmp._realname = msg.cmd.params.substr(msg.cmd.params.find(':') + 1, msg.cmd.params.size());

	msg.target.clear();
	msg.target.insert(msg.get_emmiter());

	msg.text = RPL_WELCOME;
	msg.text.replace(msg.text.find("<nick>"), 6, tmp._nickname);
	msg.text.append(RPL_YOURHOST);
	msg.text.append(RPL_CREATED);
	msg.text.replace(msg.text.find("<datetime>"), 10, date);
	msg.text.append(RPL_MYINFO);
	msg.text.append(RPL_ISUPPORT);

	while (msg.text.find("<client>") != std::string::npos)
		msg.text.replace(msg.text.find("<client>"), 8, tmp._nickname);
}

/**
 * @brief sends a message to a specific user.
 * 
 * @param msg the message to send
 */
void	server::privmsg(message &msg)
{
	std::pair<int, std::string>	target;
	std::string	text;

	if (msg.cmd.params.find(':') == std::string::npos)
	{
		error_message(msg, "", ERR_NOTEXTTOSEND);
		return ;
	}
	if (msg.cmd.params[0] == ':')
	{
		error_message(msg, "", ERR_NONICKNAMEGIVEN);
		return ;
	}
	target.second = msg.cmd.params.substr(0, msg.cmd.params.find(':') - 1);
	target.first = _get_client_by_nickname(target.second);
	text = msg.cmd.params.substr(msg.cmd.params.find(':'), msg.cmd.params.size());
	if (target.first == -1)
	{
		error_message(msg, target.second, ERR_NOSUCHNICK);
		return ;
	}
	msg.target.clear();
	msg.target.insert(target.first);
	msg.text = ":";
	msg.text.append(client_list.find(msg.get_emmiter())->second._nickname);
	msg.text.append(" PRIVMSG ");
	msg.text.append(text);
	msg.text.append("\r\n");
}

/**
 * @brief upon receiving PING, the server answers "PONG" with msg.cmd.param
 * 
 * @param msg the message containing the command
 * @note if there is no msg.cmd.param "error_message()" will be called
 */
void	server::ping(message &msg)
{
	msg.target.clear();
	msg.target.insert(msg.get_emmiter());
	msg.text.assign("PONG ");
	msg.text.append(SERVERNAME);
	msg.text.append(" ");
	msg.text.append(msg.cmd.params);
	msg.text.append("\r\n");
}
