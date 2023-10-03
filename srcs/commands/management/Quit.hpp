#pragma once

#include "ICommand.hpp"

struct Quit : public ICommand
{
	Quit(Server& p_serv);

	void	execute(Message& msg);

	private :
	void	leave_all_channels(Message& msg, std::string comment);
};
