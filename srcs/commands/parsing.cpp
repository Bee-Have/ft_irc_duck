#include "ircserv.hpp"

// TODO : deleted prefix handling since it cannot be added by the client and 
// TODO : 42's IRC does not handle server to server communications
/**
 * @brief splits the newly received message into : prefix, command and params.
 * For this purpose the message class has a struct with prefix, command and params inside.
 * @note this command only occurs if a command is found in a message.
 * Since prefix can only be found in server message, this function will change.
 * 
 * @param msg the message with the raw text inside to parse and split into the command
 */
static void	parsing_cmds(server &serv, message &msg)
{
	std::cout << "PARSING" << std::endl;
	if (msg.text.find("\n") != std::string::npos)
	{
		msg.text.erase(msg.text.find("\n"));
		if (msg.text.find("\r") != std::string::npos)
			msg.text.erase(msg.text.find("\r"));
	}

	if (msg.text.find(' ') == std::string::npos)
	{
		serv.error_message(msg, msg.text, ERR_NEEDMOREPARAMS);
		return ;
	}
	else
	{
		msg.cmd.name = msg.text.substr(0, msg.text.find(' '));
		msg.cmd.params = msg.text.substr(msg.text.find(' ') + 1, msg.text.size());
	}
	msg.text.clear();
}

/**
 * @brief check if there are commands in the message we just received from client.
 * If a command is found it will be parsed and then send to the server's appropriated command
 * 
 * @param serv the server
 * @param msg  the message we just received
 */
void	check_for_cmds(server &serv, message &msg)
{

	std::cout << "MSG:" << msg.text << '|' << std::endl;
	parsing_cmds(serv, msg);
	if (msg.target.empty() == false)
		return ;
	if (serv.commands.find(msg.cmd.name) == serv.commands.end())
		return ;
	std::cout << "CMD FOUND :" << msg.cmd.name << std::endl;
	(serv.*serv.commands[msg.cmd.name])(msg);
}