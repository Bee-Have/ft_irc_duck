#pragma once

#include <utility>

#include "ICommand.hpp"

/**
 * @brief sends a message to a specific user.
 * 
 * @param msg the message to send
 */
struct Privmsg : public ICommand
{
	Privmsg(Server &p_serv);

	void	execute(Message &msg);
};
