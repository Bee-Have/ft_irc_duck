#include "Who.hpp"

Who::Who(Server& p_serv): ICommand(p_serv)
{}

void	Who::execute(Message& msg)
{
	msg.reply_format(RPL_ENDOFWHO, msg.cmd_param, serv.socket_id);
}
