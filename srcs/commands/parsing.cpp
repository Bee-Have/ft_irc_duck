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
		msg.cmd.prefix = msg.text.substr(0, msg.text.find(' '));
		msg.text = msg.text.substr(msg.text.find(' ') + 1, msg.text.size());
	}
	if (msg.text.find(' ') != std::string::npos)
	{
		msg.cmd.name = msg.text.substr(0, msg.text.find(' '));
		msg.cmd.params = msg.text.substr(msg.text.find(' ') + 1, msg.text.size());
	}
	else
		msg.cmd.name = msg.text;
	msg.text.clear();
}

void	check_for_cmds(server &serv, message &msg)
{
	std::string	cmds[3] = {"NICK", "PASS", "USER"};

	std::cout << "MSG:" << msg.text << '|' << std::endl;

	if (msg.text.find("CAP LS") == 0)
		client.assign("irssi");

	for (int i = 0; i != 3; ++i)
	{
		if (msg.text.find(cmds[i]) != std::string::npos)
		{
			parsing_cmds(msg);
			std::cout << "CMD FOUND :" << msg.cmd.name << std::endl;
			// TODO : here call function pointer for the appropriate command
			(serv.*serv.commands[i])(msg);
			break ;
		}
	}
}