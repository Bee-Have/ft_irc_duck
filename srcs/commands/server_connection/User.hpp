#pragma once

#include <ctime>
#include <sstream>
#include <vector>

#include "ICommand.hpp"
#include "Client.hpp"

/**
 * @brief assigns a username and realname to a specific client ("msg.get_emitter()")
 * 
 * @note if client is unregistered of no username or realname is given, call "error_message()".
 * Likewise if everything is here replies will be added to msg.text as an answer
 */
struct User : public ICommand
{
	Client						&command_emitter;
	std::vector<std::string>	replies;
	std::vector<std::string>	replace;

	User(Server &p_server);

	void	execute(Message &msg);
};
