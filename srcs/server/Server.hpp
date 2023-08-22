#pragma once

// CONTAINERS
#include <map>
#include <vector>

// GENERAL
#include <iostream>
#include <string>
#include <sstream>

// IRSSI && SOCKETS
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>

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
	// JOIN command utils
	void	new_chan_member_sucess(Message &msg, std::string chan);
	void	join_space_error_behavior(Message &msg);

public:
	std::vector<Message>			msgs;
	std::map<int, Client>	client_list;

	Server(int new_port, char *new_pass);
	~Server();

	Server	&operator=(const Server &assign);

	// encapsulation
	int		get_socket() const;

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
	void	error_message(Message &msg, std::string prefix, std::string error);
	void	reply_message(Message &msg, std::string reply, std::string replace);
	void	reply_message(Message &msg, std::vector<std::string> &errors, std::vector<std::string> &replace);
	//		connect to IRSSI
	void	pass(Message &msg);
	void	nick(Message &msg);
	void	user(Message &msg);
	// Requirements
	void	oper(Message &msg);
	void	privmsg(Message &msg);
	void	join(Message &msg);
	// 42 Requirements

	//		noice
	void	ping(Message &msg);
};
