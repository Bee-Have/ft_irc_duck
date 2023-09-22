#pragma once

#include "ICommand.hpp"

struct Invite : public ICommand
{
	Invite(Server &p_serv);

	void	execute(Message &msg);
};

