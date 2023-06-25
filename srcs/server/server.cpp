#include "server.hpp"
#include "message.hpp"

server::server(void)
{}

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

	commands[0] = &server::nick;
	commands[1] = &server::pass;
	commands[2] = &server::user;
	commands[3] = &server::ping;
}

server::server(const server &cpy)
{
	(void)cpy;
}

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

// private
void	server::_error_message(message &msg, std::string prefix, std::string error)
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

int	server::get_socket(void) const
{
	return (_socket);
}

/*
	Adds a new client to the server.
	A new socket will be given to the client using 'accept()'.
	The socket will then be checked using 'getsockopt()'
	if everything worked it will be returned.
*/
int	server::add_client(void)
{
	socklen_t		client_addr_len = sizeof(_client_addr);
	server::client	new_client(accept(_socket, (struct sockaddr *)&_client_addr, &client_addr_len));

	if (new_client._socket < 0)
	{
		std::cerr << "Error : " << errno << " : " << strerror(errno) << std::endl;
		return (1);
	}

	int	socket_error;
	socklen_t optlen = sizeof(socket_error);
	if (getsockopt(new_client._socket, SOL_SOCKET, SO_ERROR, &socket_error, &optlen) >= 0)
	{
		if (socket_error != 0)
		{
			std::cout << "INVALID SOCKET :(" << std::endl;
			close(new_client._socket);
			return (1);
		}
	}
	else
		std::cerr << "Error : " << errno << " : " << strerror(errno) << std::endl;

	std::cout << "NEW CLIENT :D" << std::endl;
	client_list.insert(std::make_pair(new_client._socket, new_client));
	return (0);
}

/*
	Delete specific client (identified by 'fd') from client list. This implies :
	- deleting client fd from messages to be sent
		(if a message send by the client exists, it will only be deleted if they are no target to send the message to)
	- closing client socket before deleting it's object
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

/*
	Finds the maximum value fd existing in all the clients and the server and returns it for select()
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

/*
	Adds server fd and all client fds to read_fds for select()
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

/*
	Adds all fds from messages to be sent to write_fds for select()
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

void	server::pass(message &msg)
{
	if (msg.cmd.params.empty() == true)
	{
		_error_message(msg, msg.cmd.name, ERR_NEEDMOREPARAMS);
		return ;
	}
	if (client_list.find(msg.get_emmiter())->second._is_registered == true)
	{
		_error_message(msg, "", ERR_ALREADYREGISTRED);
		return ;
	}
	if (_pass.compare(msg.cmd.params) != 0)
	{
		_error_message(msg, "", ERR_BADPASS);
		return ;
	}
	client_list.find(msg.get_emmiter())->second._is_registered = true;
	msg.text.clear();
}

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

void	server::nick(message &msg)
{
	std::string	nickname;

	if (client_list.find(msg.get_emmiter())->second._is_registered == false)
	{
		_error_message(msg, "", ERR_UNREGISTERED);
		return ;
	}
	if (msg.cmd.params.empty() == true)
	{
		_error_message(msg, "", ERR_NONICKNAMEGIVEN);
		return ;
	}
	nickname = msg.cmd.params.substr(0, msg.cmd.params.find(' '));
	if (is_nickname_allowed(nickname) == false)
	{
		_error_message(msg, nickname, ERR_ERRONEUSNICKNAME);
		return ;
	}
	for (std::map<int, server::client>::iterator it = client_list.begin();
		it != client_list.end(); ++it)
	{
		if (nickname.compare(it->second._nickname) == 0)
		{
			_error_message(msg, nickname, ERR_NICKNAMEINUSE);
			return ;
		}
	}
	client_list.find(msg.get_emmiter())->second._nickname = nickname;
	msg.text.clear();
	// std::cout << "worked:" << client_list.find(msg.get_emmiter())->second._nickname << std::endl;
}

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
		_error_message(msg, "", ERR_UNREGISTERED);
		return ;
	}
	if (tmp._nickname.empty() == true)
	{
		_error_message(msg, "", ERR_NONICKNAMEGIVEN);
		return ;
	}
	if (msg.cmd.params.empty() == true
		|| msg.cmd.params.find(':') == std::string::npos
		|| msg.cmd.params.find(' ') == std::string::npos)
	{
		_error_message(msg, msg.cmd.name, ERR_NEEDMOREPARAMS);
		return ;
	}
	if (tmp._realname.empty() == false)
	{
		_error_message(msg, "", ERR_ALREADYREGISTRED);
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

void	server::ping(message &msg)
{
	if (msg.cmd.params.empty() == true)
	{
		_error_message(msg, msg.cmd.name, ERR_NEEDMOREPARAMS);
		return ;
	}
	msg.target.clear();
	msg.target.insert(msg.get_emmiter());
	msg.text.assign("PONG ");
	msg.text.append(SERVERNAME);
	msg.text.append(" ");
	msg.text.append(msg.cmd.params);
}
