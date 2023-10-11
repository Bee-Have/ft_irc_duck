#pragma once

#include "ICommand.hpp"

struct Who : public ICommand
{
	Who(Server& p_serv);

	void	execute(Message& msg);
};
