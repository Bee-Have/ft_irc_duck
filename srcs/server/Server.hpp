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
// #include "ICommand.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"

struct ICommand;

#define MAX_CLIENT 10

class Server
{
	friend class Join;
	friend class Part;
private:
	// Server authentification
	// TODO : shouldn't this value be a public const static instead of a private ?
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

public:
	std::vector<Message>			msgs;
	std::map<int, Client>	client_list;
	std::map<std::string, ICommand *>	commands;

	Server(int new_port, char *new_pass);
	~Server();

	Server	&operator=(const Server &assign);
 
	// encapsulation
	int			get_socket() const;
	std::string	get_pass() const;

	// client managment
	void	add_client();
	void	del_client(int fd);

	// select prerequisites
	int		get_max_fd() const;
	fd_set	get_read_fds() const;
	fd_set	get_write_fds() const;

	// tools
	template <typename CommandType>
	void	register_command(const std::string &name)
	{
		// TODO : GUARD CommandType MUST inherit ICommand
		// TODO : GUARD name MUST be unique
		commands[name] = new CommandType(*this);
	};
	int	get_client_by_nickname(std::string nickname);
	std::string	oper_command_check(int client, std::string oper, std::string pass);
};
