#pragma once

#include <string>

#include "Message.hpp"

struct ICommand
{
	ICommand() {};
	ICommand(std::string p_name): name(p_name) {};
	virtual ~ICommand() {};

	const std::string	name;

	virtual void	execute(Message &msg) = 0;
};
