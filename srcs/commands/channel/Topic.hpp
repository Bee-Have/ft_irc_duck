#pragma once

#include <ctime>
#include <sstream>

#include "ICommand.hpp"

struct Topic : public ICommand
{
	Topic(Server &p_server);

	void	execute(Message &msg);
	private :
	void	return_topic(Message &msg, Channel *channel);
	void	change_topic(Message &msg, Channel *channel, std::string new_topic);
};
