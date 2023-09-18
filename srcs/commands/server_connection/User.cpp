#include "User.hpp"

User::User(Server &p_server): ICommand(p_server), replies(5), replace(2)
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
	Client						*command_emitter = &serv.client_list.find(msg.get_emitter())->second;

	replace.clear();

	ss << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday;
	ss >> date;

	if (serv.client_list.find(msg.get_emitter())->second.nickname.empty() == true)
		return (msg.reply_format(ERR_NONICKNAMEGIVEN, "", serv.socket_id));
	if (msg.cmd_param.find(':') == std::string::npos
		|| msg.cmd_param.find(' ') == std::string::npos)
		return (msg.reply_format(ERR_NEEDMOREPARAMS, msg.cmd, serv.socket_id));
	if (command_emitter->_realname.empty() == false)
		return (msg.reply_format(ERR_ALREADYREGISTRED, "", serv.socket_id));

	command_emitter->_username = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
	command_emitter->_realname = msg.cmd_param.substr(msg.cmd_param.find(':') + 1, msg.cmd_param.size());

	replace.push_back(command_emitter->nickname);
	replace.push_back(date);

	msg.reply_format(replies, replace);
}
