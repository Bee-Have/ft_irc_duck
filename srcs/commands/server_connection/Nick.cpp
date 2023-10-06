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
		return (setup_error(msg, ERR_NONICKNAMEGIVEN, ""));
	nickname = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
	if (is_nickname_allowed(nickname) == false)
		return (setup_error(msg, ERR_ERRONEUSNICKNAME, nickname));
	if (serv.get_client_by_nickname(nickname) != -1)
		return (setup_error(msg, ERR_NICKNAMEINUSE, nickname));
	if (serv.client_list.find(msg.get_emitter())->second.get_is_authenticated() == false)
		return (setup_error(msg, ERR_UNREGISTERED, ""));
	serv.client_list.find(msg.get_emitter())->second.nickname = nickname;
	msg.text.clear();
}

void	Nick::setup_error(Message &msg, std::string error, std::string replace)
{
	int	client = msg.get_emitter();

	msg._emitter = serv.socket_id;
	msg.reply_format(error, replace, serv.socket_id);
	msg.target.clear();
	msg.target.insert(client);
}
