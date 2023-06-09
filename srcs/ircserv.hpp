#pragma once

// ERRORS
#include "define.hpp"

// CLASSES
#include "server.hpp"
#include "message.hpp"

// SERVER
void	server_loop(server &serv);

// COMMANDS
void	check_for_cmds(server &serv, message &msg);

// MESSAGES
void	send_messages(server &serv, fd_set &write_fds);
void	receive_messages(server &serv, fd_set read_fds);
