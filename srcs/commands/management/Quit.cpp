#include "Quit.hpp"

Quit::Quit(Server& p_serv): ICommand(p_serv), manual_quit(true)
{}

void	Quit::execute(Message& msg)
{
	int	sender = msg.get_emitter();
	std::string	comment("Quit: ");

	if (msg.cmd_param.empty() == false)
		comment.append(msg.cmd_param);

	std::cout << "COMMENT [" << msg.cmd_param << ']' << std::endl;

	if (serv.client_list.find(sender)->second.get_is_registered() == true)
	{
		leave_all_channels(msg, comment);
		del_client_from_msgs(sender);
	}
	msg._emitter = serv.socket_id;
	if (manual_quit == true)
		msg.reply_format(RPL_ERROR, QUIT_MANUAL, serv.socket_id);
	else
		msg.reply_format(RPL_ERROR, QUIT_FORCE, serv.socket_id);
	msg.target.clear();
	msg.target.insert(sender);
	serv.msgs.push_back(msg);
	serv.del_client(sender);
}

void	Quit::leave_all_channels(Message& msg, std::string comment)
{
	Message	channel_warning(serv.client_list.find(msg.get_emitter())->second);

	channel_warning.reply_format(RPL_QUIT, comment, serv.socket_id);
	channel_warning.target.clear();

	for (std::map<std::string, Channel>::iterator it = serv._channel_list.begin();
		it != serv._channel_list.end(); ++it)
	{
		if (it->second._clients.find(msg.get_emitter()) != it->second._clients.end()
			&& it->second.is(msg.get_emitter(), it->second.MEMBER) == true)
		{
			it->second.del_client(msg.get_emitter());
			for (std::map<int, int>::iterator it_target = it->second._clients.begin();
				it_target != it->second._clients.end(); ++it_target)
			{
				if (it->second.is(it_target->second, it->second.MEMBER) == true)
					channel_warning.target.insert(it_target->first);
			}
			serv.msgs.push_back(channel_warning);
		}
	}
}

void	Quit::del_client_from_msgs(int fd)
{
	for (std::vector<Message>::iterator it = serv.msgs.begin();
		it != serv.msgs.end(); ++it)
	{
		if (it->target.size() == 1 && it->target.find(fd) != it->target.end())
		{
			it = serv.msgs.erase(it);
			if (serv.msgs.empty() == true)
				break ;
		}
		else if (it->target.find(fd) != it->target.end())
			it->target.erase(it->target.find(fd));
	}
}
