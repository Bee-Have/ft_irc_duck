#include "message.hpp"
// #include "server.hpp"

message::message(void)
{}

message::message(const message &cpy): _emmiter(cpy._emmiter), target(cpy.target), text(cpy.text)
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
		text.assign(assign.text);
		target.clear();
		target.insert(assign.target.begin(), assign.target.end());
	}
	return (*this);
}

int	message::get_emmiter(void) const
{
	return (_emmiter);
}
