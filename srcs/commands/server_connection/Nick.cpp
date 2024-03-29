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

	if (serv.client_list.find(msg.get_emitter())->second.get_is_authenticated() == false)
		return (setup_error(msg, ERR_NOTREGISTERED, ""));
	if (msg.cmd_param.empty() == true)
		return (setup_error(msg, ERR_NONICKNAMEGIVEN, ""));
	nickname = msg.cmd_param;
	if (is_nickname_allowed(nickname) == false)
		return (setup_error(msg, ERR_ERRONEUSNICKNAME, nickname));
	if (is_nick_unique(nickname) == false)
		return (setup_error(msg, ERR_NICKNAMEINUSE, nickname));
	serv.client_list.find(msg.get_emitter())->second.nickname.assign(nickname);
	msg.text.clear();
	if (serv.client_list.find(msg.get_emitter())->second.get_is_registered() == false)
		serv.register_client_if_able(msg.get_emitter()); ///< Register the client and send msgs if USER|NICK
	add_nick_to_messages(msg, nickname);
}

void	Nick::setup_error(Message &msg, std::string error, std::string replace)
{
	int	client = msg.get_emitter();

	msg._emitter = serv.socket_id;
	msg.host = SERVERNAME;
	msg.reply_format(error, replace, serv.socket_id);
	msg.target.clear();
	msg.target.insert(client);
}

void	Nick::add_nick_to_messages(Message& msg, std::string nickname)
{
	for (std::vector<Message>::iterator it = serv.msgs.begin();
		it != serv.msgs.end(); ++it)
	{
		if (it->_emitter == msg._emitter)
			it->emitter_nick = nickname;
	}
}

static std::string	str_tolower(std::string str)
{
	std::string	ret;

	for (std::string::iterator it = str.begin(); it != str.end(); ++it)
	{
		if (std::isalpha(*it) == 0)
			ret.push_back(*it);
		else
			ret.push_back(std::tolower(*it));
	}
	return (ret);
}

bool	Nick::is_nick_unique(std::string nickname)
{
	std::string	tmp_nick = str_tolower(nickname);

	for (std::map<int, Client>::iterator it = serv.client_list.begin();
		it != serv.client_list.end(); ++it)
	{
		std::string	compare_nick(str_tolower(it->second.nickname));
		if (tmp_nick == compare_nick)
			return (false);
	}
	return (true);
}
