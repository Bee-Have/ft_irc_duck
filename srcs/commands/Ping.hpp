#pragma once

#include "ICommand.hpp"

/**
 * @brief upon receiving PING, the server answers "PONG" with msg.cmd_param
 */
struct Ping : public ICommand
{
	Ping(Server &p_serv);

	void	execute(Message &msg);
};

