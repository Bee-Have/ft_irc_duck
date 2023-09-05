#pragma once

#include "ICommand.hpp"

struct Part : public ICommand
{
	std::vector<std::string>	channels;

	Part(Server &p_serv);

	void	execute(Message &msg);
	void	split_channels(std::string &p_params);
};
