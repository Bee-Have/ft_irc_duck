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
	register_comment(msg);
	targets = return_kick_target(msg);

	for (std::vector<std::string>::iterator it = targets.begin();
		it != targets.end(); ++it)
	{
		Message	reply(serv.client_list.find(msg.get_emitter())->second);
		int	current_target = serv.get_client_by_nickname(*it);
		if (channel->_clients.find(current_target) == channel->_clients.end()
			|| channel->_is(channel->_clients.find(current_target)->second, channel->MEMBER) == false)
		{
			std::vector<std::string>	error(1, ERR_USERNOTINCHANNEL);
			std::vector<std::string>	replace;
			replace.push_back(*it);
			replace.push_back(chan_name);
			reply.reply_format(error, replace);
			serv.msgs.push_back(reply);
		}
		else
		{
			std::vector<std::string>	rpl(1, RPL_KICK);
			std::vector<std::string>	replace;

			replace.push_back(chan_name);
			replace.push_back(*it);
			replace.push_back(comment);
			reply.reply_format(rpl, replace);
			for (std::map<int, int>::iterator it_target = channel->_clients.begin();
				it_target != channel->_clients.end(); ++it_target)
			{
				if (channel->_is(it_target->second, channel->MEMBER) == true)
					reply.target.insert(it_target->first);
			}
			channel->_clients.erase(current_target);
			serv.msgs.push_back(reply);
		}
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

void	Kick::register_comment(Message &msg)
{
	if (msg.cmd_param.find(':') != std::string::npos)
	{
		comment = msg.cmd_param.substr(msg.cmd_param.find(':') + 1, msg.cmd_param.size());
		msg.cmd_param = msg.cmd_param.substr(0, msg.cmd_param.find(':'));
	}
	else
		comment = KICK_DEFAULT_COMMENT;
}
