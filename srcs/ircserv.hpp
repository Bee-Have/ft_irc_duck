#pragma once

// ERRORS
#include "define.hpp"

// CLASSES
// #include "Server.hpp"
// #include "Message.hpp"

// COMMANDS
#include "Pass.hpp"
#include "Nick.hpp"
#include "User.hpp"
#include "Privmsg.hpp"
#include "Oper.hpp"
#include "Join.hpp"
#include "Part.hpp"
#include "Ping.hpp"

// SERVER
void	server_loop(Server &serv);

// COMMANDS
void	check_for_cmds(Server &serv, Message &msg);

// MESSAGES
void	send_messages(Server &serv, fd_set &write_fds);
void	receive_messages(Server &serv, fd_set read_fds);
