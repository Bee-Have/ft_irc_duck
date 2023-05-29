#include "server.hpp"

server::server(void)
{}

server::server(int new_port, char *new_pass): _port(new_port), _pass(new_pass)
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd < 0)
	{
		std::cerr << "Error: server socket creation failed\n";
		return ;
	}
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(_port);
	_server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(_server_fd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0)
	{
		std::cerr << "Error: bind socket to PORT failed\n";
		return ;
	}
	if (listen(_server_fd, MAX_CLIENT) < 0)
	{
		std::cerr << "Error: socket listen failed\n";
		return ;
	}
}

server::server(const server &cpy)
{
	(void)cpy;
}

server::~server(void)
{
	close(_server_fd);
}

server	&server::operator=(const server &assign)
{
	if (this != &assign)
	{
		_port = assign._port;
		_pass.assign(assign._pass);
		_server_fd = assign._server_fd;
		_server_addr = assign._server_addr;
	}
	return (*this);
}

int		server::add_client(void)
{
	socklen_t	client_addr_len = sizeof(_client_addr);
	client		new_client(accept(_server_fd, (struct sockaddr *)&_client_addr, &client_addr_len));

	if (new_client.get_socket() < 0)
	{
		std::cerr << "Error: could not accept new client connection to server\n";
		return (1);
	}
	std::cout << "NEW CLIENT :D\n";
	client_list.insert(std::make_pair(new_client.get_socket(), new_client));
	return (0);
}

void	server::del_client(int fd)
{
	client_list.erase(client_list.find(fd));
	std::cout << "BYE BYE CLIENT\n";
}
