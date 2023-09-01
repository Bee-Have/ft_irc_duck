#include "Pass.hpp"

Pass::Pass(Server &p_serv): ICommand(p_serv)
{}

void	Pass::execute(Message &msg)
{
	if (serv.client_list.find(msg.get_emitter())->second._is_registered == true)
		return (msg.reply_format(ERR_ALREADYREGISTRED, "", serv.get_socket()));
	if (serv.get_pass().compare(msg.cmd_param) != 0)
		return (msg.reply_format(ERR_PASSWDMISMATCH, "", serv.get_socket()));
	serv.client_list.find(msg.get_emitter())->second._is_registered = true;
	msg.text.clear();
}
