#pragma once

#include <string>

#include "define.hpp"
#include "Server.hpp"
#include "Message.hpp"

struct ICommand
{
	const std::string	name;
	Server		&serv;

	ICommand() {};
	ICommand(std::string p_name, Server &p_serv): name(p_name), serv(p_serv) {};
	virtual ~ICommand() {};

	virtual void	execute(Message &msg) = 0;
};
