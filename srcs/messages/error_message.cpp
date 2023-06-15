#include "ircserv.hpp"

void	error_message(message &msg, std::string prefix, std::string error)
{
	msg.target.clear();
	msg.target.insert(msg.get_emmiter());
	msg.text = prefix;
	msg.text.append(error);
}
