#include "Privmsg.hpp"

Privmsg::Privmsg(Server& p_serv) : ICommand(p_serv)
{}

static void get_targets(Message& msg, std::map<std::string, int>& targets)
{
	size_t comma_pos;
	std::string target_name;

	comma_pos = msg.cmd_param.find(',');
	while (comma_pos != std::string::npos)
	{
		target_name = msg.cmd_param.substr(0, comma_pos);
		targets[target_name] = -1;
		msg.cmd_param.erase(0, comma_pos + 1);
		comma_pos = msg.cmd_param.find(',');
	}

	target_name = msg.cmd_param;
	target_name.erase(target_name.find_last_not_of(" \t") + 1);
	targets[target_name] = -1;
}

void Privmsg::add_correct_targets(Message& msg, std::map<std::string, int>& targets)
{
	for (std::map<std::string, int>::iterator it = targets.begin(); it != targets.end(); ++it)
	{
		Channel*	channel(serv.get_channel_by_name(it->first));
		if (channel != NULL)
		{
			if (channel->is(channel->_clients.find(msg.get_emitter())->second, channel->MEMBER) == false)
			{
				Message reply_msg(msg);
				reply_msg.reply_format(ERR_CANNOTSENDTOCHAN, channel->_name, serv.socket_id);
				serv.msgs.push_back(reply_msg);
				it->second = -2;
				continue;
			}
			for (std::map<int, int>::iterator client = channel->_clients.begin();
				client != channel->_clients.end(); ++client)
			{
				if (client->first != msg.get_emitter())
					msg.target.insert(client->first);
			}
		}
		else
		{
			if (it->first.empty() == true)
			{
				Message reply_msg(msg);
				reply_msg.reply_format(ERR_NONICKNAMEGIVEN, "", serv.socket_id);
				serv.msgs.push_back(reply_msg);
				it->second = -2;
				continue;
			}
			it->second = serv.get_client_by_nickname(it->first);
			if (it->second == -1)
			{
				Message reply_msg(msg);
				reply_msg.reply_format(ERR_NOSUCHNICK, it->first, serv.socket_id);
				serv.msgs.push_back(reply_msg);
				it->second = -2;
			}
			else
				msg.target.insert(it->second);
		}
	}
}

static int get_text(Server& serv, Message& msg, std::string& text)
{
	const size_t column_pos = msg.cmd_param.find(':');

	if (column_pos == std::string::npos)
	{
		msg.reply_format(ERR_NOTEXTTOSEND, "", serv.socket_id);
		return (1);
	}
	text = msg.cmd_param.substr(column_pos + 1);
	msg.cmd_param.erase(column_pos);
	return (0);
}

void	Privmsg::execute(Message& msg)
{
	std::map<std::string, int> targets;
	std::string	text;
	std::vector<std::string>	reply(1, RPL_PRIVMSG);
	std::vector<std::string>	replace;
	std::set<int>	msg_target_cpy;
	int	target_size(0);

	if (get_text(serv, msg, text) != 0)
		return;

	get_targets(msg, targets);

	add_correct_targets(msg, targets);
	if (msg.target.empty() == true)
		return ;
	msg_target_cpy.swap(msg.target);
	for (std::map<std::string, int>::iterator it = targets.begin();
		it != targets.end(); ++it)
	{
		if (it->second != -2)
		{
			replace.push_back(it->first);
			++target_size;
		}
	}
	msg.reply_replace_curly_brackets(*reply.begin(), target_size);
	replace.push_back(text);
	msg.reply_format(reply, replace);
	msg.target.clear();
	msg.target.swap(msg_target_cpy);
}
