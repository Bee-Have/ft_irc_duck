#include "Quit.hpp"

Quit::Quit(Server& p_serv): ICommand(p_serv)
{}

void	Quit::execute(Message& msg)
{
	std::string	comment("Quit: ");

	if (msg.cmd_param.empty() == false)
		comment.append(msg.cmd_param);
	

	std::cout << "COMMENT [" << msg.cmd_param << ']' << std::endl;
	
	for (std::map<std::string, Channel>::iterator it = serv._channel_list.begin();
		it != serv._channel_list.end(); ++it)
	{
		
	}
}
