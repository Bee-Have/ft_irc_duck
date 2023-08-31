#include "Pass.hpp"

Pass::Pass()
{}

Pass::Pass(std::string p_name, const Server &p_serv): ICommand(p_name, p_serv)
{}

void	Pass::execute(Message &msg)
{
	if (serv.client_list.find(msg.get_emitter())->second._is_registered == true)
		return (msg.error_format("", ERR_ALREADYREGISTRED));
	if (serv.get_pass().compare(msg.cmd_param) != 0)
		return (msg.error_format("", ERR_PASSWDMISMATCH));
	serv.client_list.find(msg.get_emitter())->second._is_registered = true;
	msg.text.clear();
}
