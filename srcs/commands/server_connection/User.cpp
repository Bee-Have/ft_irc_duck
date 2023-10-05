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
	std::time_t			time = std::time(0);
	std::tm				*now = std::localtime(&time);
	std::stringstream	ss;
	std::string			date;
	Client				*command_emitter = &serv.client_list.find(msg.get_emitter())->second;

	replace.clear();

	ss << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday;
	ss >> date;

	if (serv.client_list.find(msg.get_emitter())->second.get_is_authenticated() == false)
		return (setup_error_no_nickname(msg, ERR_UNREGISTERED, ""));
	if (serv.client_list.find(msg.get_emitter())->second.nickname.empty() == true)
		return (setup_error_no_nickname(msg, ERR_NONICKNAMEGIVEN, ""));
	if (msg.cmd_param.find(':') == std::string::npos
		|| msg.cmd_param.find(' ') == std::string::npos)
		return (setup_error_no_nickname(msg, ERR_NEEDMOREPARAMS, msg.cmd));
	if (command_emitter->_realname.empty() == false)
		return (setup_error_no_nickname(msg, ERR_ALREADYREGISTRED, ""));

	command_emitter->_username = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
	command_emitter->_realname = msg.cmd_param.substr(msg.cmd_param.find(':') + 1, msg.cmd_param.size());

	replace.push_back(command_emitter->nickname);
	replace.push_back(date);

	msg.reply_format(replies, replace);
}

void	User::setup_error_no_nickname(Message &msg, std::string error, std::string replace)
{
	int	client = msg.get_emitter();

	msg._emitter = serv.socket_id;
	msg.reply_format(error, replace, serv.socket_id);
	msg.target.clear();
	msg.target.insert(client);
}
