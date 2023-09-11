#include "Privmsg.hpp"

Privmsg::Privmsg(Server &p_serv): ICommand(p_serv)
{}

// TODO : do we really need std::pair here ? We are not really using it...
void	Privmsg::execute(Message &msg)
{
	std::pair<int, std::string>	target;
	std::string	text;

	if (msg.cmd_param.find(':') == std::string::npos)
		return (msg.reply_format(ERR_NOTEXTTOSEND, "", serv.socket_id));
	if (msg.cmd_param[0] == ':')
		return (msg.reply_format(ERR_NONICKNAMEGIVEN, "", serv.socket_id));

	target.second = msg.cmd_param.substr(0, msg.cmd_param.find(':') - 1);
	target.first = serv.get_client_by_nickname(target.second);
	text = msg.cmd_param.substr(msg.cmd_param.find(':'), msg.cmd_param.size());
	if (target.first == -1)
		return (msg.reply_format(ERR_NOSUCHNICK, target.second, serv.socket_id));
	
	msg.target.clear();
	msg.target.insert(target.first);
	
	msg.text = ":";
	msg.text.append(msg.emitter_nick);
	msg.text.append(" PRIVMSG ");
	msg.text.append(text);
	msg.text.append("\r\n");
}
