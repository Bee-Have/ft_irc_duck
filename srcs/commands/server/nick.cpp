#include "ircserv.hpp"
#include "errors.hpp"

static bool	is_nickname_printable(std::string nickname)
{
	for (size_t i = 0; i < nickname.size(); ++i)
	{
		if (std::isprint(nickname[i]) == 0)
			return (false);
	}
	return (true);
}

void	server::nick(message &msg)
{
	std::string	nickname;

	if (msg.command.params.empty() == true)
	{
		error_message(msg, msg.command.name, ERR_NONICKNAMEGIVEN);
		return ;
	}
	nickname = msg.command.params.substr(0, msg.command.params.find(' '));
	if (nickname.size() > 9 || is_nickname_printable(nickname) == false)
	{
		error_message(msg, nickname, ERR_ERRONEUSNICKNAME);
		return ;
	}
	for (std::map<int, client>::iterator it = client_list.begin();
		it != client_list.end(); ++it)
	{
		if (nickname.compare(it->second.nickname) == 0)
		{
			error_message(msg, nickname, ERR_NICKNAMEINUSE);
			return ;
		}
	}
	client_list.find(msg.get_emmiter())->second.nickname = nickname;
	msg.text.clear();
	std::cout << "worked:" << client_list.find(msg.get_emmiter())->second.nickname << std::endl;
}
