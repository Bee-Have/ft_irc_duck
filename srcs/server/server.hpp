#pragma once

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_CLIENT 10

class server
{
private:
	server(const server &cpy);

	int			_port;
	std::string	_pass;

	int					_server_fd;
	struct sockaddr_in	_server_addr;

public:
	server();
	server(int new_port, char *new_pass);
	~server();

	server	&operator=(const server &assign);
};
