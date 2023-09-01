#pragma once

#include "ICommand.hpp"

/**
/**
 * @brief attempt to become a server operator using a specific password
 */
struct Oper : public ICommand
{
	Oper(Server &p_serv);

	void	execute(Message &msg);
};

