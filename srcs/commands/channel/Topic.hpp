#pragma once

#include <ctime>
#include <sstream>

#include "ICommand.hpp"

struct Topic : public ICommand
{
	Topic(Server &p_server);

	void	execute(Message &msg);
	private :
	bool	are_client_credentials_correct(Message& msg, Channel *channel);
	void	return_topic(Message &msg, Channel *channel);
	void	change_topic(Message &msg, Channel *channel, std::string new_topic);
};
