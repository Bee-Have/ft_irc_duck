#pragma once

#include "ICommand.hpp"

struct Invite : public ICommand
{
	Invite(Server &p_serv);

	void	execute(Message &msg);

	private :
	Channel *channel;
	int		client;

	bool	is_param_format_good(Message &msg);
	bool	do_param_exist_and_set_if_so(Message &msg);
	bool	are_param_membership_valid(Message &msg);
};

