#include "Message.hpp"
// #include "Server.hpp"

/**
 * @brief Construct a new Message::Message object
 * @note this construction should never be called by anyone.
 * Message should always be instanciated with am _emmiter
 */
Message::Message(void)
{}

Message::Message(const Message &cpy): _emmiter(cpy._emmiter), target(cpy.target), text(cpy.text), cmd(cpy.cmd), cmd_param(cpy.cmd_param)
{}

Message::Message(const Client &emmiter): _emmiter(emmiter.get_socket())
{}

Message::Message(int emmiter): _emmiter(emmiter)
{}

Message::~Message(void)
{
	target.clear();
}

Message	&Message::operator=(const Message &assign)
{
	if (this != &assign)
	{
		_emmiter = assign._emmiter;
		target.clear();
		target.insert(assign.target.begin(), assign.target.end());
		text.assign(assign.text);
		cmd.assign(assign.cmd);
		cmd_param.assign(assign.cmd_param);
	}
	return (*this);
}

int	Message::get_emmiter(void) const
{
	return (_emmiter);
}
