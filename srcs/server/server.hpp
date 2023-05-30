#pragma once

// CONTAINERS
#include <map>

// GENERAL
#include <iostream>
#include <string>

// SOCKETS
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// ERRNO :
#include <string.h>
#include <errno.h>

// CLASSES
#include "client.hpp"

#define MAX_CLIENT 10

class server
{
private:
	int			_port;
	std::string	_pass;

	int					_socket;
	struct sockaddr_in	_server_addr;
	struct sockaddr_in	_client_addr;

	server();
	server(const server &cpy);

public:
	std::map<int, client>	client_list;

	server(int new_port, char *new_pass);
	~server();

	server	&operator=(const server &assign);

	// encapsulation
	int		get_socket() const;

	// client managment
	int		add_client();
	void	del_client(int fd);

	// select prerequisites
	int		get_max_fd() const;
	fd_set	get_read_fds();
	fd_set	get_write_fds();
};
