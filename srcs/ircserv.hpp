#pragma once

#include "server.hpp"

// SERVER
void	server_loop(server &serv);

// MESSAGES
void	send_messages(server &serv, fd_set &write_fds);
void	receive_message(server &serv, fd_set read_fds);
