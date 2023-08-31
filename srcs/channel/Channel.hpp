#pragma once

#include <string>
#include <map>

class Server;

class Channel
{
friend class Server;
private:
	enum ClientStatus
	{
		INVITED = 0b0001,
		MEMBER = 0b0010,
		CHANOP = 0b0100
	};

	std::string			_name;
	std::string			_topic;
	std::string			_key;
	bool				_is_invite_only;
	std::map<int, int>	_clients;

	Channel();
	Channel(int creator, std::string new_name);

	Channel	&operator=(const Channel &assign);

	bool	_is(int bitfield, int enumval) const;
	void	add_new_member(int new_member);

public:
	Channel(const Channel &cpy);
	~Channel();

};
