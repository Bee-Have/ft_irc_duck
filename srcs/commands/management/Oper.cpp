#include "Oper.hpp"

Oper::Oper(Server &p_serv): ICommand(p_serv)
{}

void	Oper::execute(Message &msg)
{
	std::string	oper;
	std::string	pass;
	std::string	return_msg;
	size_t oper_end_pos = msg.cmd_param.find(' ');
	size_t pass_begin_pos;

	oper = msg.cmd_param.substr(0, oper_end_pos);

	if (oper_end_pos == std::string::npos)
		return (msg.reply_format(ERR_NEEDMOREPARAMS, "OPER", serv.socket_id));
	pass_begin_pos = msg.cmd_param.find_first_not_of(" \t", oper_end_pos);
	if (pass_begin_pos == std::string::npos)
		return (msg.reply_format(ERR_NEEDMOREPARAMS, "OPER", serv.socket_id));
	pass = msg.cmd_param.substr(pass_begin_pos);

	return_msg = serv.oper_command_check(msg.get_emitter(), oper, pass);

	if (return_msg == RPL_YOUREOPER)
		return (msg.reply_format(return_msg, "", serv.socket_id));
	else if (return_msg == ERR_NOSUCHOPER)
		return (msg.reply_format(return_msg, oper, serv.socket_id));
	msg.reply_format(return_msg, "", serv.socket_id);
}
