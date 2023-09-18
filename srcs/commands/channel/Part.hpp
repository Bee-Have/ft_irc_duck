#pragma once

#include "ICommand.hpp"

struct Part : public ICommand
{
	std::vector<std::string>	channels;

	Part(Server &p_serv);

	void	execute(Message &msg);

	private :
	void	split_channels(std::string &p_params);
	void	loop_check(Message *msg);
	void	assign_next_chanop(Channel *current);
	void	success_behaviour(Message *msg, Channel *current);
	// checks
	bool	are_there_other_chanops(Channel *current);
	private :
	bool	delete_chan_if_empty(Channel *current);
};
