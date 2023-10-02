#include "Kick.hpp"

Kick::Kick(Server& p_serv): ICommand(p_serv)
{}

void	Kick::execute(Message& msg)
{
	std::string					chan_name;
	std::vector<std::string>	targets;
	Channel	*channel;

	comment.clear();
	if (msg.cmd_param.empty() == true || msg.cmd_param.find(' ') == std::string::npos)
		return (msg.reply_format(ERR_NEEDMOREPARAMS, msg.cmd, serv.socket_id));

	chan_name = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
	msg.cmd_param = msg.cmd_param.substr(msg.cmd_param.find(' ') + 1, msg.cmd_param.size());
	if (serv._channel_list.find(chan_name) == serv._channel_list.end())
		return (msg.reply_format(ERR_NOSUCHCHANNEL, chan_name, serv.socket_id));
	channel = &serv._channel_list.find(chan_name)->second;
	if (is_issuer_membership_valid(msg, *channel) == false)
		return ;
	if (msg.cmd_param.find(':') != std::string::npos)
	{
		comment = msg.cmd_param.substr(msg.cmd_param.find(':') + 1, msg.cmd_param.size());
		msg.cmd_param = msg.cmd_param.substr(0, msg.cmd_param.find(':'));
	}

	std::cout << "CHANNEL [" << chan_name << ']' << std::endl;
	targets = return_kick_target(msg);
	for (std::vector<std::string>::iterator it = targets.begin();
		it != targets.end(); ++it)
	{
		std::cout << "[" << *it << ']';
	}
	std::cout << std::endl;

	for (std::vector<std::string>::iterator it = targets.begin();
		it != targets.end(); ++it)
	{
		int	current_target = serv.get_client_by_nickname(*it);
		if (channel->_clients.find(current_target) == )
	}
}

std::vector<std::string>	Kick::return_kick_target(Message& msg)
{
	std::vector<std::string>	target_list;

	if (msg.cmd_param.find(',') == std::string::npos)
	{
		target_list.push_back(msg.cmd_param);
		return (target_list);
	}

	while (msg.cmd_param.find(", ") != std::string::npos)
	{
		target_list.push_back(msg.cmd_param.substr(0, msg.cmd_param.find(", ")));
		msg.cmd_param = msg.cmd_param.substr(msg.cmd_param.find(", ") + 2, msg.cmd_param.size());
	}
	target_list.push_back(msg.cmd_param);
	return (target_list);
}

bool	Kick::is_issuer_membership_valid(Message &msg, Channel channel)
{
	if (channel._clients.find(msg.get_emitter()) == channel._clients.end())
	{
		msg.reply_format(ERR_NOTONCHANNEL, channel._name, serv.socket_id);
		return (false);
	}
	if (channel._is(channel._clients.find(msg.get_emitter())->second, channel.CHANOP) == false)
	{
		msg.reply_format(ERR_CHANOPRIVSNEEDED, channel._name, serv.socket_id);
		return (false);
	}
	return (true);
}

