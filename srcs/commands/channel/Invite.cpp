#include "Invite.hpp"

Invite::Invite(Server &p_serv): ICommand(p_serv)
{}

// TODO : check when mode is implemented that the behaviour does not change
// specifically with invite only channels, who can invite and who cant
void	Invite::execute(Message &msg)
{
	Message		msg_invite(msg);
	std::vector<std::string>	success_reply(1, RPL_INVITING);
	std::vector<std::string>	success_replace;

	if (is_param_format_good(msg) == false)
		return ;
	if (do_param_exist_and_set_if_so(msg) == false)
		return ;

	success_replace.push_back(serv.client_list.find(client)->second.nickname);
	success_replace.push_back(channel->_name);
	msg.reply_format(success_reply, success_replace);

	if (are_param_membership_valid(msg) == false)
		return ;

	channel->_clients[client] |= channel->INVITED;
	success_reply.clear();
	success_reply.push_back(RPL_INVITE);

	msg_invite.reply_format(success_reply, success_replace);
	msg_invite.target.clear();
	msg_invite.target.insert(client);
	serv.msgs.push_back(msg_invite);
}

bool	Invite::is_param_format_good(Message &msg)
{
	if (msg.cmd_param.empty() == true)
	{
		msg.reply_format(ERR_NEEDMOREPARAMS, "INVITE", serv.socket_id);
		return (false);
	}
	if (msg.cmd_param.find(' ') == std::string::npos)
	{
		msg.reply_format(ERR_NEEDMOREPARAMS, "INVITE", serv.socket_id);
		return (false);
	}
	return (true);
}

bool	Invite::do_param_exist_and_set_if_so(Message &msg)
{
	int	first_space = msg.cmd_param.find(' ');
	int	last_space = msg.cmd_param.find_last_of(' ');
	std::string	nickname(msg.cmd_param.substr(0, first_space));
	std::string	chan_name(msg.cmd_param.substr(last_space + 1, msg.cmd_param.size()));

	msg.cmd_param = msg.cmd_param.substr(first_space, last_space - first_space);
	if (msg.cmd_param.find_first_not_of(" ") != std::string::npos)
	{
		msg.reply_format(ERR_TOOMANYPARAM, "INVITE", serv.socket_id);
		return (false);
	}
	if (serv.get_channel_by_name(chan_name) == NULL)
	{
		msg.reply_format(ERR_NOSUCHCHANNEL, chan_name, serv.socket_id);
		return (false);
	}
	if (serv.get_client_by_nickname(nickname) == -1)
	{
		msg.reply_format(ERR_NOSUCHNICK, nickname, serv.socket_id);
		return (false);
	}
	channel = serv.get_channel_by_name(chan_name);
	client = serv.get_client_by_nickname(nickname);
	return (true);
}

bool	Invite::are_param_membership_valid(Message &msg)
{
	if (channel->_clients.find(client) != channel->_clients.end())
	{
		if (channel->is(channel->_clients.find(client)->second, channel->MEMBER) == true)
		{
			std::vector<std::string>	reply(1, ERR_USERONCHANNEL);
			std::vector<std::string>	replace;
			replace.push_back(serv.client_list.find(client)->second.nickname);
			replace.push_back(channel->_name);
			msg.reply_format(reply, replace);
		}
		return (false);
	}
	if (channel->_clients.find(msg.get_emitter()) == channel->_clients.end())
	{
		msg.reply_format(ERR_NOTONCHANNEL, channel->_name, serv.socket_id);
		return (false);
	}
	return (true);
}
