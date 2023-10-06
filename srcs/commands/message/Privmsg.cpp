#include "Privmsg.hpp"

Privmsg::Privmsg(Server &p_serv): ICommand(p_serv)
{}

void	Privmsg::execute(Message &msg)
{
	std::pair<int, std::string>	target;
	std::string target_name;
	std::string	text;
	const size_t column_pos = msg.cmd_param.find(':');

	if (column_pos == std::string::npos)
		return (msg.reply_format(ERR_NOTEXTTOSEND, "", serv.socket_id));
	target_name = msg.cmd_param.substr(0, column_pos);
	target_name.erase(target_name.find_last_not_of(" \t") + 1);
	if (target_name.empty())
		return (msg.reply_format(ERR_NONICKNAMEGIVEN, "", serv.socket_id));

	target.first = serv.get_client_by_nickname(target_name);
	target.second = target_name;
	text = msg.cmd_param.substr(column_pos);
	if (target.first == -1)
		return (msg.reply_format(ERR_NOSUCHNICK, target.second, serv.socket_id));

	msg.target.clear();
	msg.target.insert(target.first);

	msg.text = ":";
	msg.text.append(msg.emitter_nick)
			.append(" PRIVMSG ")
			.append(text)
			.append("\r\n");
}
