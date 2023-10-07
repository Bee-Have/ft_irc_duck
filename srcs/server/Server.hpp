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

// ERRORS :
#include <string.h>
#include <errno.h>
#include "define.hpp"

// CLASSES
#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"

struct ICommand;

#define MAX_CLIENT 10

class Server
{
	friend struct Join;
	friend struct Part;
	friend struct Quit;
private:

	// Server socket creation and identification
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
	Server	&operator=(const Server &assign);

public:
	// Server authentification
	const int			socket_id;
	const int			port;
	const std::string	pass;

	std::vector<Message>	msgs;
	std::map<int, Client>	client_list;
	std::map<std::string, ICommand *>	commands;

	Server(int new_port, char *new_pass);
	~Server();

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
		// TODO : GUARD : "CommandType" MUST inherit ICommand
		if (commands.find(name) != commands.end())
		{
			std::cerr << ERR_NONUNIQUECOMMAND;
			return ;
		}
		commands[name] = new CommandType(*this);
	};
	int	get_client_by_nickname(std::string nickname);
	Channel*	get_channel_by_name(std::string nickname);
	std::string	oper_command_check(int client, std::string oper, std::string pass);
};
