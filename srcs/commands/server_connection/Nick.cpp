#include "Nick.hpp"

Nick::Nick(std::string p_name, const Server &p_serv): ICommand(p_name, p_serv)
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
		return (msg.error_message(msg, "", ERR_NONICKNAMEGIVEN));
	nickname = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
	if (is_nickname_allowed(nickname) == false)
		return (msg.error_message(msg, nickname, ERR_ERRONEUSNICKNAME));
	if (serv._get_client_by_nickname(nickname) != -1)
		return (msg.error_message(msg, nickname, ERR_NICKNAMEINUSE));
	serv.client_list.find(msg.get_emitter())->second.nickname = nickname;
	msg.text.clear();
}
