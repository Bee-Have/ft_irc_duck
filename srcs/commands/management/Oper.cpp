#include "Oper.hpp"

Oper::Oper(Server &p_serv): ICommand(p_serv)
{}

void	Oper::execute(Message &msg)
{
	std::string	oper;
	std::string	pass;
	std::string	return_msg;

	if (msg.cmd_param.find(' ') == std::string::npos)
		return (msg.error_format(ERR_PASSWDMISMATCH));

	oper = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
	pass = msg.cmd_param.substr(msg.cmd_param.find(' ') + 1, msg.cmd_param.size());

	return_msg = serv.oper_command_check(msg.get_emitter(), oper, pass);

	if (return_msg == RPL_YOUREOPER)
		return (msg.reply_format(return_msg, ""));
	else if (return_msg == ERR_NOSUCHOPER)
		return (msg.error_format(return_msg, oper));
	msg.error_format(return_msg, "");
}
