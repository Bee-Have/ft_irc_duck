#include "Nick.hpp"

Nick::Nick(Server &p_serv): ICommand(p_serv)
{}

bool	Nick::is_nickname_allowed(const std::string &nickname) const
{
	if (nickname.size() > 9)
		return (false);
	if (std::isdigit(nickname[0]) != 0 || nickname[0] == '-')
		return (false);
	if (nickname.find_first_not_of(NICK_GOOD_CHARACTERS) != std::string::npos)
		return (false);
	return (true);
}

void	Nick::execute(Message &msg)
{
	std::string	nickname;

	if (msg.cmd_param.empty() == true)
		return (msg.reply_format(ERR_NONICKNAMEGIVEN, "", serv.socket_id));
	nickname = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
	if (is_nickname_allowed(nickname) == false)
		return (msg.reply_format(ERR_ERRONEUSNICKNAME, nickname, serv.socket_id));
	if (serv.get_client_by_nickname(nickname) != -1)
		return (msg.reply_format(ERR_NICKNAMEINUSE, nickname, serv.socket_id));
	serv.client_list.find(msg.get_emitter())->second.nickname = nickname;
	msg.text.clear();
}
