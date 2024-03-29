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
#include "Mode.hpp"
#include "Privmsg.hpp"
#include "Oper.hpp"
#include "Quit.hpp"
#include "Join.hpp"
#include "Part.hpp"
#include "Invite.hpp"
#include "Topic.hpp"
#include "Kick.hpp"
#include "Ping.hpp"
#include "Who.hpp"///< This is not fully implemented

// SERVER
void	server_loop(Server &serv);

// COMMANDS
void	check_for_cmds(Server &serv, Message &msg);

// MESSAGES
void	send_messages(Server &serv, fd_set &write_fds);
void	receive_messages(Server &serv, fd_set read_fds);
