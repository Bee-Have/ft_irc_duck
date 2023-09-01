#pragma once

#include "ICommand.hpp"

/**
 * @brief Attempts to register a client into our server
 * 
 * @note this command will call "error_message()" if msg.cmd.param does not fit server password
 */
struct Pass : public ICommand
{
	Pass(Server &p_serv);

	void	execute(Message &msg);
};
