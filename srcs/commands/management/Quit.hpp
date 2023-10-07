#pragma once

#include "ICommand.hpp"

struct Quit : public ICommand
{
	bool	manual_quit;

	Quit(Server& p_serv);

	void	execute(Message& msg);

	private :
	void	leave_all_channels(int client, std::string comment);
	void	del_client_from_msgs(int fd);
};
