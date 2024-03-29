#include "ircserv.hpp"
#include "Logger.hpp"

/**
 * @brief splits the newly received message into : command and params.
 * For this purpose the message class has a struct with command and params inside.
 * @note this command only occurs if a command is found in a message.
 * Since prefix can only be found in server message, this function will change.
 *
 * @param msg the message with the raw text inside to parse and split into the command
 */
static void	parsing_cmds(Server& serv, Message& msg)
{
	if (msg.text.empty() == true || msg.text.find_first_not_of(" \n\r\t") == std::string::npos)
		return;

	msg.text.erase(0, msg.text.find_first_not_of(" \t"));

	const size_t newline_pos = msg.text.find("\n");
	const size_t carriage_pos = msg.text.find("\r");
	const size_t space_pos = msg.text.find(' ');

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
			return;
		}
	}
	else if (space_pos != std::string::npos)
	{
		msg.cmd = msg.text.substr(0, space_pos);
		size_t param_start_pos = msg.text.find_first_not_of(" \t", space_pos);
		if (param_start_pos != std::string::npos)
			msg.cmd_param = msg.text.substr(param_start_pos);
		else
			msg.cmd_param = msg.text.substr(space_pos);

		if (msg.cmd != "USER" && msg.cmd != "PRIVMSG")
			msg.cmd_param.erase(msg.cmd_param.find_last_not_of(" \t") + 1);
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
void	check_for_cmds(Server& serv, Message& msg)
{
	parsing_cmds(serv, msg);
	if (msg.target.empty() == false)
		return;
	if (serv.commands.find(msg.cmd) == serv.commands.end())
		return;
	if (serv.client_list.find(msg.get_emitter())->second.get_is_registered() == false
		&& msg.cmd != "PASS" && msg.cmd != "NICK" && msg.cmd != "USER")
		msg.reply_format(ERR_NOTREGISTERED, "", serv.socket_id);
	else
	{
		Logger(basic_type, minor_lvl) << "Command called by [" <<
			msg.get_emitter() << ":" << msg.emitter_nick << "]: [" <<
			msg.cmd << "] With parameters [" << msg.cmd_param << "]";
		serv.commands[msg.cmd]->execute(msg);
	}
}
