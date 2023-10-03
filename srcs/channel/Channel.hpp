#pragma once

#include <string>
#include <map>

class Server;

class Channel
{
friend class Server;
friend struct Join;
friend struct Part;
friend struct Invite;
friend struct Topic;
friend struct Kick;
friend struct Quit;

// TODO : wouldn't it be better to have a function here to del a client from the channel ?
// that way its handled by the channel and not the commands.
// the channel can do all the necessary steps to change chanop privileges
private:
	enum ClientStatus
	{
		INVITED = 0b0001,
		MEMBER = 0b0010,
		CHANOP = 0b0100
	};

	std::string			_name;
	std::string			_topic;
	std::string			_topic_date;
	std::string			_topic_author;
	std::string			_key;
	bool				_is_invite_only;
	std::map<int, int>	_clients;

	Channel();
	Channel(int creator, std::string new_name);

	Channel	&operator=(const Channel &assign);


public:
	bool	is(int bitfield, int enumval) const;
	Channel(const Channel &cpy);
	~Channel();

};
