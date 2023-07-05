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
		bool		_is_registered;
		std::string	_nickname;
		std::string	_username;
		std::string	_realname;

	public:
		client(const client &cpy);
		~client();

		client	&operator=(const client &assign);

		int		get_socket() const;
	};
private:
	// server authentification
	int			_port;
	std::string	_pass;

	// server socket creation and identification
	int					_socket;
	struct sockaddr_in	_server_addr;
	struct sockaddr_in	_client_addr;

	// server operator
	const std::string	_oper_name;
	const std::string	_oper_pass;
	int					_oper_socket;

	server();
	server(const server &cpy);
	// tools
	int	_get_client_by_nickname(std::string nickname);

public:
	std::vector<message>			msgs;
	std::map<int, server::client>	client_list;

	server(int new_port, char *new_pass);
	~server();

	server	&operator=(const server &assign);

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
	typedef void(server::*command)(message &);
	std::map<std::string, command>	commands;
	// commands
	void	error_message(message &msg, std::string prefix, std::string error);
	void	reply_message(message &msg, std::string reply, std::string replace);
	void	reply_message(message &msg, std::vector<std::string> &errors, std::vector<std::string> &replace);
	//		connect to IRSSI
	void	pass(message &msg);
	void	nick(message &msg);
	void	user(message &msg);
	// Requirements
	void	oper(message &msg);
	void	privmsg(message &msg);

	//		noice
	void	ping(message &msg);
};
