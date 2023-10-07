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

static void add_correct_targets(Server& serv, Message& msg, std::map<std::string, int>& targets)
{

	for (std::map<std::string, int>::iterator it = targets.begin(); it != targets.end(); ++it)
	{
		if (it->first[0] == '#' || it->first[0] == '&')
		{
			// HANDLE CHANNEL MESSAGE
		}
		else
		{
			if (it->first.empty() == true)
			{
				Message reply_msg(msg);
				reply_msg.reply_format(ERR_NONICKNAMEGIVEN, "", serv.socket_id);
				serv.msgs.push_back(reply_msg);
				continue;
			}
			it->second = serv.get_client_by_nickname(it->first);
			if (it->second == -1)
			{
				Message reply_msg(msg);
				reply_msg.reply_format(ERR_NOSUCHNICK, it->first, serv.socket_id);
				serv.msgs.push_back(reply_msg);
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
	text = msg.cmd_param.substr(column_pos);
	msg.cmd_param.erase(column_pos);
	return (0);
}

void	Privmsg::execute(Message& msg)
{
	std::map<std::string, int> targets;
	std::string	text;

	if (get_text(serv, msg, text) != 0)
		return;

	get_targets(msg, targets);

	msg.target.clear();

	add_correct_targets(serv, msg, targets);

	msg.text = ":";
	msg.text.append(msg.emitter_nick)
		.append(" PRIVMSG ")
		.append(text)
		.append("\r\n");
}
