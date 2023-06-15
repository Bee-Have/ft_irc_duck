#include "server.hpp"

server::server(void)
{}

server::server(int new_port, char *new_pass): _port(new_port), _pass(new_pass)
{
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0)
	{
		std::cerr << "Error: server socket creation failed" << std::endl;
		return ;
	}
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(_port);
	_server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(_socket, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0)
	{
		std::cerr << "Error: bind socket to PORT failed" << std::endl;
		return ;
	}
	if (listen(_socket, MAX_CLIENT) < 0)
	{
		std::cerr << "Error: socket listen failed" << std::endl;
		return ;
	}

	commands[0] = &server::nick;
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
	msg.target.clear();
	msg.target.insert(msg.get_emmiter());
	msg.text = prefix;
	msg.text.append(error);
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
	socklen_t	client_addr_len = sizeof(_client_addr);
	client		new_client(accept(_socket, (struct sockaddr *)&_client_addr, &client_addr_len));

	if (new_client.get_socket() < 0)
	{
		std::cerr << "Error : " << errno << " : " << strerror(errno) << std::endl;
		return (1);
	}

	int	socket_error;
	socklen_t optlen = sizeof(socket_error);
	if (getsockopt(new_client.get_socket(), SOL_SOCKET, SO_ERROR, &socket_error, &optlen) >= 0)
	{
		if (socket_error != 0)
		{
			std::cout << "INVALID SOCKET :(" << std::endl;
			close(new_client.get_socket());
			return (1);
		}
	}
	else
		std::cerr << "Error : " << errno << " : " << strerror(errno) << std::endl;

	std::cout << "NEW CLIENT :D" << std::endl;
	client_list.insert(std::make_pair(new_client.get_socket(), new_client));
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

	for (std::map<int, client>::const_iterator it = client_list.begin(); it != client_list.end(); ++it)
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
	for (std::map<int, client>::const_iterator it = client_list.begin()
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

static bool	is_nickname_printable(std::string nickname)
{
	for (size_t i = 0; i < nickname.size(); ++i)
	{
		if (std::isprint(nickname[i]) == 0)
			return (false);
	}
	return (true);
}

void	server::nick(message &msg)
{
	std::string	nickname;

	if (msg.cmd.params.empty() == true)
	{
		_error_message(msg, msg.cmd.name, ERR_NONICKNAMEGIVEN);
		return ;
	}
	nickname = msg.cmd.params.substr(0, msg.cmd.params.find(' '));
	if (nickname.size() > 9 || is_nickname_printable(nickname) == false)
	{
		_error_message(msg, nickname, ERR_ERRONEUSNICKNAME);
		return ;
	}
	for (std::map<int, client>::iterator it = client_list.begin();
		it != client_list.end(); ++it)
	{
		if (nickname.compare(it->second.nickname) == 0)
		{
			_error_message(msg, nickname, ERR_NICKNAMEINUSE);
			return ;
		}
	}
	client_list.find(msg.get_emmiter())->second.nickname = nickname;
	msg.text.clear();
	std::cout << "worked:" << client_list.find(msg.get_emmiter())->second.nickname << std::endl;
}
