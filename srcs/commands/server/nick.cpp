#include "server.hpp"
#include "errors.hpp"

static void	setup_error(message &msg, std::string prefix, std::string error)
{
	msg.target.clear();
	msg.target.insert(msg.get_emmiter());
	msg.text = prefix;
	msg.text.append(error);
	std::cout << "error:" << msg.text;
}

static bool	is_nickname_printable(std::string nickname)
{
	for (size_t i = 0; i < nickname.size(); ++i)
	{
		if (std::isprint(nickname[i]) != 0)
			return (false);
	}
	return (true);
}

void	server::nick(message &msg)
{
	std::string	nickname;

	if (msg.command.params.empty() == true)
	{
		setup_error(msg, msg.command.name, ERR_NONICKNAMEGIVEN);
		return ;
	}
	nickname = msg.command.params.substr(0, msg.command.params.find(' '));
	if (nickname.size() > 9 || is_nickname_printable(nickname) == false)
	{
		setup_error(msg, nickname, ERR_ERRONEUSNICKNAME);
		return ;
	}
	for (std::map<int, client>::iterator it = client_list.begin();
		it != client_list.end(); ++it)
	{
		if (nickname.compare(it->second.nickname) == 0)
		{
			setup_error(msg, nickname, ERR_NICKNAMEINUSE);
			return ;
		}
	}
	client_list.find(msg.get_emmiter())->second.nickname = nickname;
	msg.text.clear();
	std::cout << "worked:" << client_list.find(msg.get_emmiter())->second.nickname << std::endl;
}
