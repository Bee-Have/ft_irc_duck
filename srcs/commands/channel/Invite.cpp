#include "Invite.hpp"

Invite::Invite(Server &p_serv): ICommand(p_serv)
{}

void	Invite::execute(Message &msg)
{
	std::string	nickname;
	std::string	channel;

	if (msg.cmd_param.empty() == true)
		return (msg.reply_format(ERR_NEEDMOREPARAMS, "INVITE", serv.socket_id));
	if (msg.cmd_param.find(' ') == std::string::npos)
		return (msg.reply_format(ERR_NEEDMOREPARAMS, "INVITE", serv.socket_id));

	nickname = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
	channel = msg.cmd_param.substr(msg.cmd_param.find(' ') + 1, msg.cmd_param.size());

	std::cout << "NICKNAME [" << nickname << "]\n";
	std::cout << "CHANNEL [" << channel << "]\n";

	if (serv._channel_list.find(channel) == serv._channel_list.end())
		return (msg.reply_format(ERR_NOSUCHCHANNEL));
}
