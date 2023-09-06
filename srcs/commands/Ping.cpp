#include "Ping.hpp"

Ping::Ping(Server &p_serv): ICommand(p_serv)
{}

void	Ping::execute(Message &msg)
{
	msg.target.clear();
	msg.target.insert(msg.get_emitter());
	msg.text.assign("PONG ");
	msg.text.append(SERVERNAME);
	msg.text.append(" ");
	msg.text.append(msg.cmd_param);
	msg.text.append("\r\n");
}
