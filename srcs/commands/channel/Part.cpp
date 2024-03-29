#include "Part.hpp"

Part::Part(Server &p_serv): ICommand(p_serv)
{}

void	Part::execute(Message &msg)
{
	reason.clear();

	if (msg.cmd_param.find(" ") != std::string::npos)
	{
		reason = msg.cmd_param.substr(msg.cmd_param.find(" ") + 1, msg.cmd_param.size());
		msg.cmd_param = msg.cmd_param.substr(0, msg.cmd_param.find(" "));
	}
	split_channels(msg.cmd_param);
	if (msg.cmd_param.empty() == true)
		channels.push_back("");
	loop_check(&msg);
}

void	Part::split_channels(std::string &p_params)
{
	size_t	comma = p_params.find(',');

	channels.clear();
	if (comma == std::string::npos)
	{
		channels.push_back(p_params);
		return ;
	}
	while (comma != std::string::npos)
	{
		channels.push_back(p_params.substr(0, comma));
		p_params.erase(0, comma + 1);
		comma = p_params.find(',');
	}
	if (p_params.empty() == false)
		channels.push_back(p_params);
}

void	Part::loop_check(Message *msg)
{
	Message	error(serv.client_list.find(msg->get_emitter())->second);

	for (std::vector<std::string>::iterator it = channels.begin();
		it != channels.end(); ++it)
	{
		if (serv._channel_list.find(*it) == serv._channel_list.end())
		{
			error.reply_format(ERR_NOSUCHCHANNEL, *it, serv.socket_id);
			serv.msgs.push_back(error);
			continue ;
		}
		Channel	*current = &serv._channel_list.find(*it)->second;
		if (current->_clients.find(msg->get_emitter()) == current->_clients.end()
			|| current->is(current->_clients.find(msg->get_emitter())->second, current->MEMBER) == false)
		{
			error.reply_format(ERR_NOTONCHANNEL, *it, serv.socket_id);
			serv.msgs.push_back(error);
			continue ;
		}
		success_behaviour(msg, current);
	}
}

bool	Part::delete_chan_if_empty(Channel *current)
{
	for (std::map<int, int>::iterator it = current->_clients.begin() ;
		it != current->_clients.end() ; ++it)
	{
		if (current->is(it->second, current->MEMBER) == true)
			return (false);
	}
	serv._channel_list.erase(serv._channel_list.find(current->_name));
	return (true);
}

void	Part::success_behaviour(Message *msg, Channel *current)
{
	Message	warning_client_leaving(serv.client_list.find(msg->get_emitter())->second);
	std::vector<std::string> reply;
	std::vector<std::string> replace;
	
	reply.push_back(RPL_PART);
	replace.push_back(current->_name);
	if (reason.empty() == true)
		replace.push_back(KICK_DEFAULT_COMMENT);
	else
		replace.push_back(reason);

	current->del_client(msg->get_emitter(), serv);
	delete_chan_if_empty(current);
	warning_client_leaving.reply_format(reply, replace);
	for (std::map<int, int>::iterator it = current->_clients.begin() ;
		it != current->_clients.end() ; ++it)
	{
		if (current->is(it->second, current->MEMBER) == true)
			warning_client_leaving.target.insert(it->first);
	}
	serv.msgs.push_back(warning_client_leaving);
}
