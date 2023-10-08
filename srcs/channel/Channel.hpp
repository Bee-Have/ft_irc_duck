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
friend struct Mode;
friend struct Privmsg;

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

	// Mode related variables
	std::string			_key;
	bool				_is_invite_only;
	bool				_is_topic_restricted;
	int					_member_limit;
	std::map<int, int>	_clients;

	Channel();
	Channel(int creator, std::string new_name);

	Channel	&operator=(const Channel &assign);


public:
	Channel(const Channel &cpy);
	~Channel();

	bool	is(int bitfield, int enumval) const;
	bool	are_there_other_chanops();
	void	del_client(int client, Server& serv);
	void	assign_next_chanop(Server& serv);
};
