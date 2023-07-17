#include "Channel.hpp"

Channel::Channel(void) : _is_invite_only(false)
{}

Channel::Channel(const Channel &cpy) :
	_name(cpy._name),
	_topic(cpy._topic),
	_key(cpy._key),
	_is_invite_only(cpy._is_invite_only),
	_clients(cpy._clients)
{}

Channel::~Channel(void)
{}

Channel	&Channel::operator=(const Channel &assign)
{
	if (this != &assign)
	{
		_name.assign(assign._name);
		_topic.assign(assign._topic);
		_key.assign(assign._key);
		_is_invite_only = assign._is_invite_only;
		_clients.clear();
		for (std::map<int, int>::const_iterator it = assign._clients.begin();
			it != assign._clients.end(); ++it)
		{
			_clients.insert(*it);
		}
	}
	return (*this);
}
