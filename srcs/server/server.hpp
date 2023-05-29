#pragma once

#include <map>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "client.hpp"

#define MAX_CLIENT 10

class server
{
private:
	int			_port;
	std::string	_pass;

	int					_server_fd;
	struct sockaddr_in	_server_addr;
	struct sockaddr_in	_client_addr;

	server();
	server(const server &cpy);

public:
	std::map<int, client>	client_list;

	server(int new_port, char *new_pass);
	~server();

	server	&operator=(const server &assign);
	
	int		add_client();
	void	del_client(int fd);
};
