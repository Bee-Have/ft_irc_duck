#include "Mode.hpp"

Mode::Mode(Server& p_serv) : ICommand(p_serv) {}

void	Mode::execute(Message &msg)
{
	std::string	name;

	if (msg.cmd_param.find(' ') == std::string::npos)
		name = msg.cmd_param;
	else
	{
		name = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
		msg.cmd_param = msg.cmd_param.substr(msg.cmd_param.find(' ') + 1);
	}
	if (msg.cmd_param[0] != '+' && msg.cmd_param[0] != '-')
		return (msg.reply_format(ERR_MODEBADFORMAT, "", serv.socket_id));

	std::cout << "NAME [" << name << ']' << std::endl;
	if (name[0] == '#' || name[0] == '&')
		std::cout << "ITS A CHANNEL" << std::endl;
	else
	{
		std::cout << "ITS A CLIENT" << std::endl;
		std::cout << "PARAM [" << msg.cmd_param << ']' << std::endl;
		client_handling(msg, name);
	}
}

void	Mode::client_handling(Message& msg, std::string nick)
{
	char	sign;
	std::string	usermodes;
	std::string	parameters;

	if (msg.cmd_param.find(' ') != std::string::npos)
	{
		parameters = msg.cmd_param.substr(msg.cmd_param.find(' ') + 1);
		msg.cmd_param = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
	}
	if (serv.get_client_by_nickname(nick) == -1)
		return (msg.reply_format(ERR_NOSUCHNICK, nick, serv.socket_id));
	if (serv.get_client_by_nickname(nick) != msg.get_emitter())
		return (msg.reply_format(ERR_USERSDONTMATCH, "", serv.socket_id));
	for (std::string::iterator it = msg.cmd_param.begin();
		it != msg.cmd_param.end(); ++it)
	{
		if (*it == '+' || *it == '-')
			sign = *it;
		else
		{
			Message response(msg);
			if (*it == 'i')
			{
				if (sign == '+')
					serv.client_list.find(msg.get_emitter())->second._is_invisible = true;
				else
					serv.client_list.find(msg.get_emitter())->second._is_invisible = false;
				if (usermodes.find(*it) == std::string::npos)
				{
					usermodes.push_back(sign);
					usermodes.push_back(*it);
				}
				else
					usermodes[usermodes.find(*it) - 1] = sign;
				std::cout << "INVISIBLE [" << serv.client_list.find(msg.get_emitter())->second._is_invisible;
			}
			else if (*it == 'O')
			{
				if (sign == '+')
				{
					response.cmd = "OPER";
					response.cmd_param = parameters;
					serv.commands["OPER"]->execute(response);
					serv.msgs.push_back(response);
					if (response.text.find("381 ") != std::string::npos)
					{
						if (usermodes.find(*it) == std::string::npos)
						{
							usermodes.push_back(sign);
							usermodes.push_back(*it);
						}
						else
							usermodes[usermodes.find(*it) - 1] = sign;
					}
				}
				else
				{
					if (serv._oper_socket != msg.get_emitter())
					{
						response.reply_format(ERR_NOPRIVILEGES, "", serv.socket_id);
						serv.msgs.push_back(response);
					}
					else
					{
						serv._oper_socket = -1;
						if (usermodes.find(*it) == std::string::npos)
						{
							usermodes.push_back(sign);
							usermodes.push_back(*it);
						}
						else
							usermodes[usermodes.find(*it) - 1] = sign;
					}
				}
			}
			else
			{
				response.reply_format(ERR_UMODEUNKNOWNMODEFLAG, "", serv.socket_id);
				serv.msgs.push_back(response);
			}
		}
	}
	std::cout << '\n' << "USERMODE [" << usermodes << ']' << std::endl;
	msg.reply_format(RPL_UMODEIS, usermodes, serv.socket_id);
}
