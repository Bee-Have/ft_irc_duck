#include "ircserv.hpp"

/**
 * @brief splits the newly received message into : prefix, command and params.
 * For this purpose the message class has a struct with prefix, command and params inside.
 * @note this command only occurs if a command is found in a message.
 * Since prefix can only be found in server message, this function will change.
 * 
 * @param msg the message with the raw text inside to parse and split into the command
 */
static void	parsing_cmds(Server &serv, Message &msg)
{
	std::cout << "PARSING" << std::endl;
	if (msg.text.find("\n") != std::string::npos)
	{
		msg.text.erase(msg.text.find("\n"));
		if (msg.text.find("\r") != std::string::npos)
			msg.text.erase(msg.text.find("\r"));
	}

	if (msg.text.empty() == false && msg.text.find(' ') == std::string::npos)
	{
		if (serv.commands.find(msg.text) != serv.commands.end())
			msg.reply_format(ERR_NEEDMOREPARAMS, msg.text, serv.socket_id);
		return ;
	}
	else
	{
		msg.cmd = msg.text.substr(0, msg.text.find(' '));
		msg.cmd_param = msg.text.substr(msg.text.find(' ') + 1, msg.text.size());
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
void	check_for_cmds(Server &serv, Message &msg)
{
	std::cout << "MSG:" << msg.text << '|' << std::endl;
	parsing_cmds(serv, msg);
	if (msg.target.empty() == false)
		return ;
	if (serv.commands.find(msg.cmd) == serv.commands.end())
		return ;
	std::cout << "CMD FOUND :" << msg.cmd << std::endl;
	if (serv.client_list.find(msg.get_emitter())->second.get_is_registered() == false
		&& msg.cmd != "PASS")
		msg.reply_format(ERR_UNREGISTERED, "", serv.socket_id);
	else
		serv.commands[msg.cmd]->execute(msg);
}
