#include "Mode.hpp"

Mode::Mode(Server& p_serv) : ICommand(p_serv) {}

void	Mode::execute(Message &msg)
{
	(void)msg;
}