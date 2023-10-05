#include "Pass.hpp"

Pass::Pass(Server &p_serv): ICommand(p_serv)
{}

void	Pass::execute(Message &msg)
{
	if (serv.client_list.find(msg.get_emitter())->second._is_registered == true
		|| serv.client_list.find(msg.get_emitter())->second._is_authenticated == true)
		return (setup_error(msg, ERR_ALREADYREGISTRED));
	if (serv.pass.compare(msg.cmd_param) != 0)
		return (setup_error(msg, ERR_PASSWDMISMATCH));
	serv.client_list.find(msg.get_emitter())->second._is_authenticated = true;
	msg.text.clear();
}

void	Pass::setup_error(Message &msg, std::string error)
{
	int	client = msg.get_emitter();

	msg._emitter = serv.socket_id;
	msg.reply_format(error, "", serv.socket_id);
	msg.target.clear();
	msg.target.insert(client);
}
