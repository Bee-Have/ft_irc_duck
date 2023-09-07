#include "Part.hpp"

Part::Part(Server &p_serv): ICommand(p_serv)
{}

// TODO : why do we need to check if msg.cmd_param.empty() == true ?
void	Part::execute(Message &msg)
{
	std::string	reason;

	if (msg.cmd_param.find(" ") != std::string::npos)
	{
		reason = msg.cmd_param.substr(msg.cmd_param.find(" ") + 1, msg.cmd_param.size());
		msg.cmd_param = msg.cmd_param.substr(0, msg.cmd_param.find(" "));
	}
	std::cout << "reason [" << reason << "]\n";
	std::cout << "original [" << msg.cmd_param << "]\n";
	split_channels(msg.cmd_param);
	for (std::vector<std::string>::iterator print = channels.begin() ;
		print != channels.end() ; ++print)
	{
		std::cout << "[" << *print << "]";
	}
	std::cout << std::endl;
	if (msg.cmd_param.empty() == true)
	{
		channels.back().append(",");
		return (msg.reply_format(ERR_NOSUCHCHANNEL, channels.back(), serv.get_socket()));
	}
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
	Message	error(msg->get_emitter());

	for (std::vector<std::string>::iterator it = channels.begin();
		it != channels.end(); ++it)
	{
		if (serv._channel_list.find(*it) == serv._channel_list.end())
		{
			error.reply_format(ERR_NOSUCHCHANNEL, *it, serv.get_socket());
			continue ;
		}
		Channel	*current = &serv._channel_list.find(*it)->second;
		if (current->_clients.find(msg->get_emitter()) == current->_clients.end()
			|| current->_is(current->_clients.find(msg->get_emitter())->second, current->MEMBER) == false)
		{
			error.reply_format(ERR_NOTONCHANNEL, *it, serv.get_socket());
			continue ;
		}
		success_behaviour(msg, current);
	}
}

void	Part::success_behaviour(Message *msg, Channel *current)
{
	Message	warning_client_leaving(msg->get_emitter());
	std::pair<int, int>	*client_cpy = current->_clients.find(msg->get_emitter());

	client_cpy->second = client_cpy->second ^ current->MEMBER;
	if (client_cpy->second == 0)
		current->_clients.erase(current->.find(msg->get_emitter()));
	if (current->_is(client_cpy->second, current->CHANOP) == true)
	{
		client_cpy->second = client_cpy->second ^ current->CHANOP;
		if (are_there_other_chanops(current) == false)
			assign_next_chanop(current);
		if (current->_is(client_cpy->second, current->INVITED) == false)
			current->_clients.erase(current->.find(msg->get_emitter()));
	}
	warning_client_leaving.reply_format(RPL_PART, current->_name, serv.get_socket());
	warning_client_leaving.target.clear();
	for (std::map<int, int>::iterator it = current->_clients.begin() ;
		it != current->_clients.end() ; ++it)
	{
		if (current->_is(it->second, current->MEMBER) == true)
			warning_client_leaving.target.push_back(it->first);
	}
}

void	Part::assign_next_chanop(Channel *current)
{
	for (std::map<int, int>::iterator it = current->_clients.begin() ;
		it != current->_clients.end() ; ++it)
	{
		if (current->_is(it->second, current->MEMBER) == true)
		{
			it->second = it->second | current->CHANOP;
			return ;
		}
	}
}

bool	Part::are_there_other_chanops(Channel *current)
{
	for (std::map<int, int>::iterator it = current->_clients.begin();
		it != current->_clients.end(); ++it)
	{
		if (current->_is(it->second, current->CHANOP) == true)
			return (true);
	}
	return (false);
}
