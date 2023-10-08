#include "Channel.hpp"
#include "Server.hpp"
#include "Message.hpp"

Channel::Channel(void) : _is_invite_only(false)
{}

Channel::Channel(int creator, std::string new_name) :
	_name(new_name), _is_invite_only(false), _is_topic_restricted(true),
	_member_limit(-1)
{
	int	bitfield = MEMBER | CHANOP;
	_clients[creator] = bitfield;
}

Channel::Channel(const Channel &cpy) :
	_name(cpy._name), _topic(cpy._topic), _key(cpy._key),
	_is_invite_only(cpy._is_invite_only),
	_is_topic_restricted(cpy._is_topic_restricted),
	_member_limit(cpy._member_limit),
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
		_is_topic_restricted = assign._is_topic_restricted;
		_member_limit = assign._member_limit;
		_clients.clear();
		for (std::map<int, int>::const_iterator it = assign._clients.begin();
			it != assign._clients.end(); ++it)
		{
			_clients[it->first] = it->second;
		}
	}
	return (*this);
}

bool	Channel::is(int bitfield, int enumval) const
{
	if ((bitfield & enumval) != 0)
		return (true);
	return (false);
}

void	Channel::del_client(int client, Server& serv)
{
	if (_clients.find(client) == _clients.end())
		return ;
	
	int	*bitfield = &_clients.find(client)->second;

	if (is(*bitfield, MEMBER) == true)
	{
		*bitfield = *bitfield ^ MEMBER;
		if(is(*bitfield, CHANOP) == true)
		{
			*bitfield = *bitfield ^ CHANOP;
			if (are_there_other_chanops() == false)
				assign_next_chanop(serv);
		}
	}
	_clients.erase(client);
}

bool	Channel::are_there_other_chanops()
{
	for (std::map<int, int>::iterator it = _clients.begin();
		it != _clients.end(); ++it)
	{
		if (is(it->second, CHANOP) == true)
			return (true);
	}
	return (false);
}

void	Channel::assign_next_chanop(Server& serv)
{
	for (std::map<int, int>::iterator it = _clients.begin() ;
		it != _clients.end() ; ++it)
	{
		if (is(it->second, MEMBER) == true)
		{
			Message	warning(serv.client_list.find(it->first)->second);
			it->second = it->second | CHANOP;

			warning.reply_format(RPL_YOURECHANOP, _name, serv.socket_id);
			serv.msgs.push_back(warning);
			return ;
		}
	}
}
