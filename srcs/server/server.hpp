#pragma once

// CONTAINERS
#include <map>
#include <vector>

// GENERAL
#include <iostream>
#include <string>

// SOCKETS
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// ERRORS :
#include <string.h>
#include <errno.h>
#include "errors.hpp"

// CLASSES
// #include "message.hpp"
class message;

#define MAX_CLIENT 10

class server
{
public:
	class client
	{
	friend class server;
	private:
		client();
		client(int new_socket);

		int			_socket;
		std::string	_realname;
		std::string	_username;
		std::string	_hostname;
		std::string	_servername;
		std::string	_nickname;
		bool		_is_registered;

	public:
		client(const client &cpy);
		~client();

		client	&operator=(const client &assign);

		int		get_socket() const;
	};
private:
	int			_port;
	std::string	_pass;

	int					_socket;
	struct sockaddr_in	_server_addr;
	struct sockaddr_in	_client_addr;

	server();
	server(const server &cpy);
	void	_error_message(message &msg, std::string prefix, std::string error);

public:
	std::vector<message>	msgs;
	std::map<int, server::client>	client_list;

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
	fd_set	get_read_fds() const;
	fd_set	get_write_fds() const;

	// command function pointer
	void	(server::*commands[10])(message &) ;

	// commands
	void	nick(message &msg);
	void	pass(message &msg);
	// void	user(message &msg);
};
