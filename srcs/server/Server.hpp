#pragma once

// CONTAINERS
#include <map>
#include <vector>

// GENERAL
#include <iostream>
#include <string>
// #include <sstream>

// IRSSI && SOCKETS
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
// #include <ctime>

// ERRORS :
#include <string.h>
#include <errno.h>
#include "define.hpp"

// CLASSES
#include "Client.hpp"
#include "Channel.hpp"
class Message;

#define MAX_CLIENT 10

class Server
{
private:
	// Server authentification
	int			_port;
	std::string	_pass;

	// Server socket creation and identification
	int					_socket;
	struct sockaddr_in	_server_addr;
	struct sockaddr_in	_client_addr;

	// Server operator
	const std::string	_oper_name;
	const std::string	_oper_pass;
	int					_oper_socket;

	// channels
	std::map<std::string, Channel>	_channel_list;

	Server();
	Server(const Server &cpy);
	// tools
	int	_get_client_by_nickname(std::string nickname);

public:
	std::vector<Message>			msgs;
	std::map<int, Client>	client_list;

	Server(int new_port, char *new_pass);
	~Server();

	Server	&operator=(const Server &assign);
 
	// encapsulation
	int		get_socket() const;
	int		get_pass() const;

	// client managment
	void	add_client();
	void	del_client(int fd);

	// select prerequisites
	int		get_max_fd() const;
	fd_set	get_read_fds() const;
	fd_set	get_write_fds() const;

	// command function pointer
	typedef void(Server::*command)(Message &);
	std::map<std::string, command>	commands;
	// commands
	std::string	oper_command_check(int client, std::string oper, std::string pass);
	// 42 Requirements

	//		noice
	void	ping(Message &msg);
};
