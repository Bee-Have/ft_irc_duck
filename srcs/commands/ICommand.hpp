#pragma once

#include <string>

#include "define.hpp"
#include "Server.hpp"
#include "Message.hpp"

struct ICommand
{
	Server		&serv;

	ICommand(Server &p_serv): serv(p_serv) {};
	virtual ~ICommand() {};

	virtual void	execute(Message &msg) = 0;
};
