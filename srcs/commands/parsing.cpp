#include "ircserv.hpp"

/**
 * @brief splits the newly received message into : command and params.
 * For this purpose the message class has a struct with command and params inside.
 * @note this command only occurs if a command is found in a message.
 * Since prefix can only be found in server message, this function will change.
 * 
 * @param msg the message with the raw text inside to parse and split into the command
 */
static void	parsing_cmds(Server &serv, Message &msg)
{
	msg.text.erase(0, msg.text.find_first_not_of(" \t"));

	const size_t newline_pos = msg.text.find("\n");
	const size_t carriage_pos = msg.text.find("\r");
	const size_t space_pos = msg.text.find(' ');

	std::cout << "PARSING" << std::endl;
	if (newline_pos != std::string::npos)
	{
		msg.text.erase(newline_pos);
		if (carriage_pos != std::string::npos)
			msg.text.erase(carriage_pos);
	}

	if (msg.text.empty() == false && space_pos == std::string::npos)
	{
		if (msg.text.find("QUIT") != std::string::npos)
			msg.cmd = "QUIT";
		else if (serv.commands.find(msg.text) != serv.commands.end())
		{
			msg.reply_format(ERR_NEEDMOREPARAMS, msg.text, serv.socket_id);
			return ;
		}
	}
	else
	{
		msg.cmd = msg.text.substr(0, space_pos);
		msg.cmd_param = msg.text.substr(msg.text.find_first_not_of(" \t", space_pos));
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
		&& msg.cmd != "PASS" && msg.cmd != "NICK" && msg.cmd != "USER")
		msg.reply_format(ERR_UNREGISTERED, "", serv.socket_id);
	else
	{
		std::cout << "TEST" << std::endl;
		serv.commands[msg.cmd]->execute(msg);
	}
}