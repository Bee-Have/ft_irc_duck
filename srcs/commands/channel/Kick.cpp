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

	std::cout << "BEFORE [" << msg.cmd_param << "]\n";

	chan_name = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
	msg.cmd_param = msg.cmd_param.substr(msg.cmd_param.find(' ') + 1, msg.cmd_param.size());

	std::cout << "CUT OUT CHAN [" << msg.cmd_param << "]\n";

	if (serv._channel_list.find(chan_name) == serv._channel_list.end())
		return (msg.reply_format(ERR_NOSUCHCHANNEL, chan_name, serv.socket_id));
	channel = &serv._channel_list.find(chan_name)->second;
	if (is_issuer_membership_valid(msg, *channel) == false)
		return ;
	register_comment(msg);

	targets = return_kick_target(msg);

	kick_targets_if_member(msg, targets, channel);
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
		if (*msg.cmd_param.rbegin() == ' ')
			msg.cmd_param.erase(msg.cmd_param.end() - 1);
	}
	else
		comment = KICK_DEFAULT_COMMENT;
}

void	Kick::kick_targets_if_member(Message &msg, std::vector<std::string>targets, Channel *channel)
{
	for (std::vector<std::string>::iterator it = targets.begin();
		it != targets.end(); ++it)
	{
		Message	reply(serv.client_list.find(msg.get_emitter())->second);
		std::vector<std::string>	rpl;
		std::vector<std::string>	replace;
		int	current_target = serv.get_client_by_nickname(*it);

		if (channel->_clients.find(current_target) == channel->_clients.end()
			|| channel->_is(channel->_clients.find(current_target)->second, channel->MEMBER) == false)
		{
			rpl.push_back(ERR_USERNOTINCHANNEL);
			replace.push_back(*it);
			replace.push_back(channel->_name);
			reply.reply_format(rpl, replace);
		}
		else
		{
			rpl.push_back(RPL_KICK);
			replace.push_back(channel->_name);
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
		}
		serv.msgs.push_back(reply);
	}
}
