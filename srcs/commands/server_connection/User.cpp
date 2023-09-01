#include "User.hpp"

User::User(const Server &p_server): ICommand(p_server), replies(5), replace(2)
{
	replies[0] = RPL_WELCOME;
	replies[1] = RPL_YOURHOST;
	replies[2] = RPL_CREATED;
	replies[3] = RPL_MYINFO;
	replies[4] = RPL_ISUPPORT;
}

void	User::execute(Message &msg)
{
	std::time_t					time = std::time(0);
	std::tm						*now = std::localtime(&time);
	std::stringstream			ss;
	std::string					date;

	replace.clear();
	command_emitter = serv.client_list.find(msg.get_emitter()->second);

	ss << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday;
	ss >> date;

	if (msg.emitter_name.empty() == true)
		return (msg.reply_format(ERR_NONICKNAMEGIVEN, "", serv.get_socket()));
	if (msg.cmd_param.find(':') == std::string::npos
		|| msg.cmd_param.find(' ') == std::string::npos)
		return (msg.reply_format(ERR_NEEDMOREPARAMS, msg.cmd, serv.get_socket()));
	if (command_emitter._realname.empty() == false)
		return (msg.reply_format(ERR_ALREADYREGISTRED, "", serv.get_socket()));

	command_emitter._username = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
	command_emitter._realname = msg.cmd_param.substr(msg.cmd_param.find(':') + 1, msg.cmd_param.size());

	rpl_replace.push_back(tmp.nickname);
	rpl_replace.push_back(date);

	msg.reply_format(replies, rpl_replace);
}
