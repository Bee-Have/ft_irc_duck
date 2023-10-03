#include "Join.hpp"

Join::Join(Server &p_serv): ICommand(p_serv)
{}

void	Join::execute(Message &msg)
{
	std::string		tmp;

	keys.clear();
	channels.clear();

	if (msg.cmd_param.compare("0") == 0)
		return (special_argument(msg));
	if (msg.cmd_param.find_first_of(" ") != msg.cmd_param.find_last_of(" "))
		return (join_space_error(msg));
	if (msg.cmd_param.find(' ') != std::string::npos)
	{
		std::cout << "FOUND KEY" << std::endl;
		tmp = msg.cmd_param.substr(msg.cmd_param.find(' ') + 1, msg.cmd_param.size());
		keys = split_join_cmd(tmp);
		msg.cmd_param.erase(msg.cmd_param.find(' '), msg.cmd_param.size());
	}
	channels = split_join_cmd(msg.cmd_param);
	if (msg.cmd_param.empty() == true)
		channels.push_back("");
	join_channel(msg);
}

void	Join::special_argument(Message &msg)
{
	msg.cmd_param.clear();
	for (std::map<std::string, Channel>::iterator it = serv._channel_list.begin();
		it != serv._channel_list.end(); ++it)
	{
		if (it->second._clients.find(msg.get_emitter()) != it->second._clients.end())
		{
			if (msg.cmd_param.empty() == true)
				msg.cmd_param = it->first;
			else
				msg.cmd_param.append("," + it->first);
		}
	}
	if (msg.cmd_param.empty() == false)
		serv.commands["PART"]->execute(msg);
}

void	Join::join_space_error(Message &msg)
{
	std::string	channel_name;
	int			begin;
	int			end;

	if (msg.cmd_param.find_first_of(",") < msg.cmd_param.find_first_of(" "))
	{
		begin = msg.cmd_param.find(",") + 1;
		if (msg.cmd_param.find(",", begin) < msg.cmd_param.find(" ", msg.cmd_param.find_first_of(" ") + 1))
			end = msg.cmd_param.find(",", begin) - begin;
		else
			end = msg.cmd_param.find(" ", msg.cmd_param.find_first_of(" ") + 1) - begin;
		channel_name = msg.cmd_param.substr(begin, end);
	}
	msg.reply_format(ERR_NOSUCHCHANNEL, channel_name, serv.socket_id);
}

std::vector<std::string>	Join::split_join_cmd(std::string &str)
{
	std::vector<std::string>	result;
	size_t						comma = str.find(',');

	if (comma == std::string::npos)
	{
		result.push_back(str);
		return (result);
	}
	while (comma != std::string::npos)
	{
		result.push_back(str.substr(0, comma));
		str.erase(0, comma + 1);
		comma = str.find(',');
	}
	if (str.empty() == false)
		result.push_back(str);
	return (result);
}

void	Join::join_channel(Message msg)
{
	for (std::vector<std::string>::iterator it_chan = channels.begin();
		it_chan != channels.end(); ++it_chan)
	{
		if (it_chan->empty() == true || is_channel_name_allowed(*it_chan) == false)
		{
			Message	error(serv.client_list.find(msg.get_emitter())->second);
			error.reply_format(ERR_NOSUCHCHANNEL, *it_chan, serv.socket_id);
			std::cout << "ERR BAD CHAN [" << error.text << ']' << std::endl;
			serv.msgs.push_back(error);
			if (keys.empty() == false)
				keys.erase(keys.begin());
			continue;
		}
		if (serv._channel_list.find(*it_chan) == serv._channel_list.end())
			join_create_channel(msg, *it_chan);
		else
		{
			Channel	*current_chan = &serv._channel_list.find(*it_chan)->second;
			join_check_existing_chan(msg, current_chan);
		}
		if (keys.empty() == false)
			keys.erase(keys.begin());
	}
}

bool	Join::is_channel_name_allowed(std::string chan_name)
{
	if (chan_name[0] != '#' && chan_name[0] != '&')
		return (false);
	if (chan_name.find_first_not_of(NICK_GOOD_CHARACTERS, 1) != std::string::npos)
		return (false);
	return (true);
}

void	Join::join_create_channel(Message msg, std::string chan_name)
{
	Channel							new_chan(msg.get_emitter(), chan_name);
	std::pair<std::string, Channel>	new_pair(chan_name, new_chan);

	serv._channel_list.insert(new_pair);
	new_chan_member_sucess(msg, chan_name);
}

void	Join::join_check_existing_chan(Message msg, Channel *channel)
{
	Message	error(serv.client_list.find(msg.get_emitter())->second);

	if (channel->is(channel->_clients.find(msg.get_emitter())->second, channel->MEMBER) == true)
		return ;
	if (channel->_key.empty() == false
		&& (keys.empty() == true || channel->_key != *keys.begin()))
	{
		error.reply_format(ERR_BADCHANNELKEY, channel->_name, serv.socket_id);
		serv.msgs.push_back(error);
	}
	if (channel->is(channel->_clients.find(msg.get_emitter())->second, channel->INVITED) == true)
		channel->_clients.find(msg.get_emitter())->second = channel->MEMBER;
	else
	{
		if (channel->_is_invite_only == true)
		{
			error.reply_format(ERR_INVITEONLYCHAN, channel->_name, serv.socket_id);
			serv.msgs.push_back(error);
		}
		else
			channel->_clients[msg.get_emitter()] = channel->MEMBER;
	}
	if (error.text.empty() == true)
		new_chan_member_sucess(msg, channel->_name);
}

void	Join::new_chan_member_sucess(Message msg, std::string chan)
{
	Channel						channel_cpy(serv._channel_list.find(chan)->second);
	std::vector<std::string>	replies(1, RPL_JOIN);
	std::vector<std::string>	replace(1, chan);
	Message						reply(serv.client_list.find(msg.get_emitter())->second);
	Message						new_member_warning(serv.client_list.find(msg.get_emitter())->second);

	new_member_warning.reply_format(RPL_JOIN, chan, serv.socket_id);
	new_member_warning.target.clear();

	if (serv._channel_list.find(chan)->second._topic.empty() == false)
	{
		replies.push_back(RPL_TOPIC);
		replies.push_back(RPL_TOPICWHOTIME);
		replace.push_back(chan);
		replace.push_back(serv._channel_list.find(chan)->second._topic);
		replace.push_back(chan);
		replace.push_back(serv._channel_list.find(chan)->second._topic_author);
		replace.push_back(serv._channel_list.find(chan)->second._topic_date);
	}
	replies.push_back(RPL_NAMREPLY);
	// TODO : check when modes is implemented that there isnt a if here to be added
	replace.push_back("=");
	replace.push_back(chan);
	msg.reply_replace_curly_brackets(*replies.rbegin(), channel_cpy._clients.size());
	for (std::map<int, int>::iterator it = channel_cpy._clients.begin();
		it != channel_cpy._clients.end(); ++it)
	{
		if (channel_cpy.is(it->second, channel_cpy.MEMBER) == false)
			continue ;
		std::string	nick(serv.client_list.find(it->first)->second.nickname);
		if (channel_cpy.is(it->second, channel_cpy.CHANOP) == true)
			nick.insert(0, "@");
		replace.push_back(nick);
		if (it->first != msg.get_emitter())
			new_member_warning.target.insert(it->first);
	}
	replies.push_back(RPL_ENDOFNAMES);
	replace.push_back(chan);
	reply.reply_format(replies, replace);
	serv.msgs.push_back(reply);
	if (channel_cpy._clients.size() > 1)
		serv.msgs.push_back(new_member_warning);
}
