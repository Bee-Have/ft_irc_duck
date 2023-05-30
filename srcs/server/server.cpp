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
	}
	return (*this);
}

int	server::get_socket(void) const
{
	return (_socket);
}

int	server::add_client(void)
{
	socklen_t	client_addr_len = sizeof(_client_addr);
	client		new_client(accept(_socket, (struct sockaddr *)&_client_addr, &client_addr_len));

	if (new_client.get_socket() < 0)
	{
		std::cerr << "Error : " << errno << " : " << strerror(errno) << std::endl;
		return (1);
	}

	// ! TESTING
	int	socket_error;
	socklen_t optlen = sizeof(socket_error);
	if (getsockopt(new_client.get_socket(), SOL_SOCKET, SO_ERROR, &socket_error, &optlen) >= 0)
	{
		if (socket_error != 0)
			std::cout << "INVALID SOCKET :(" << std::endl;
	}
	else
		std::cerr << "Error : " << errno << " : " << strerror(errno) << std::endl;

	std::cout << new_client.get_socket() << std::endl;
	std::cout << "NEW CLIENT :D" << std::endl;
	client_list.insert(std::make_pair(new_client.get_socket(), new_client));
	std::cout << new_client.get_socket() << std::endl;

	return (0);
}

void	server::del_client(int fd)
{
	close(fd);
	client_list.erase(client_list.find(fd));
	std::cout << "BYE BYE CLIENT" << std::endl;
}

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

fd_set	server::get_read_fds(void)
{
	fd_set	read_fds;

	FD_ZERO(&read_fds);
	FD_SET(_socket, &read_fds);
	for (std::map<int, client>::iterator it = client_list.begin(); it != client_list.end(); ++it)
	{
		FD_SET(it->first, &read_fds);
	}
	return (read_fds);
}

fd_set	server::get_write_fds(void)
{
	fd_set	write_fds;

	FD_ZERO(&write_fds);

	// TODO: add behavior here with messages
	// *if there are messages to send, add them to write_fds, otherwise skip
	// !for now write_fds will be empty
	// !TMP
	FD_SET(_socket, &write_fds);

	return (write_fds);
}
