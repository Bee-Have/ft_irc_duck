#include "Channel.hpp"

Channel::Channel(void) : _is_invite_only(false)
{}

Channel::Channel(int creator, std::string new_name) : _name(new_name), _is_invite_only(false)
{
	int	bitfield = MEMBER | CHANOP;
	_clients[creator] = bitfield;
}

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
			_clients[it->first] = it->second;
		}
	}
	return (*this);
}

bool	Channel::_is(int bitfield, int enumval) const
{
	if ((bitfield & enumval) != 0)
		return (true);
	return (false);
}

void	Channel::add_new_member(int new_member)
{
	if (_clients.find(new_member) == _clients.end())
		_clients[new_member] = 0;
	if (_is(_clients.find(new_member)->second, MEMBER) == true)
		return ;
	_clients[new_member] |= MEMBER;
}
