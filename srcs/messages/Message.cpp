#include "message.hpp"
// #include "server.hpp"

/**
 * @brief Construct a new message::message object
 * @note this construction should never be called by anyone.
 * Message should always be instanciated with am _emmiter
 */
message::message(void)
{}

message::message(const message &cpy): _emmiter(cpy._emmiter), target(cpy.target), text(cpy.text), cmd(cpy.cmd), cmd_param(cpy.cmd_param)
{}

message::message(const server::client &emmiter): _emmiter(emmiter.get_socket())
{}

message::message(int emmiter): _emmiter(emmiter)
{}

message::~message(void)
{
	target.clear();
}

message	&message::operator=(const message &assign)
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

int	message::get_emmiter(void) const
{
	return (_emmiter);
}
