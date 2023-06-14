#include "ircserv.hpp"

static void	parsing_cmds(message &msg)
{
	if (msg.text.find("\n") != std::string::npos)
	{
		msg.text.erase(msg.text.find("\n"));
		if (msg.text.find("\r") != std::string::npos)
			msg.text.erase(msg.text.find("\r"));
	}

	if (msg.text[0] == ':')
	{
		msg.command.prefix = msg.text.substr(0, msg.text.find(' '));
		msg.text = msg.text.substr(msg.text.find(' '), msg.text.size());
	}
	if (msg.text.find(' ') != std::string::npos)
	{
		msg.command.name = msg.text.substr(0, msg.text.find(' '));
		msg.command.params = msg.text.substr(msg.text.find(' ') + 1, msg.text.size());
	}
	else
		msg.command.name = msg.text;
	msg.text.clear();
}

void	check_for_cmds(server &serv, message &msg)
{
	std::string	cmds[3] = {"NICK", "PASS", "USER"};

	// TODO : here serv is useless but it will be used for the function pointer
	(void)serv;
	for (int i = 0; i != 3; ++i)
	{
		if (msg.text.find(cmds[i]) != std::string::npos)
		{
			parsing_cmds(msg);
			std::cout << "CMD FOUND :" << msg.command.name << "->" << msg.command.params << std::endl;
			// TODO : here call function pointer for the appropriate command
			break ;
		}
	}
}