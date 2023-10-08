#include "Topic.hpp"

Topic::Topic(Server &p_server): ICommand(p_server)
{}

void	Topic::execute(Message &msg)
{
	std::string	new_topic;
	std::string channel_name;

	if (msg.cmd_param.empty() == true)
		return (msg.reply_format(ERR_NOSUCHCHANNEL, "", serv.socket_id));
	channel_name = msg.cmd_param.substr(0, msg.cmd_param.find_first_of(" :\0"));
	if (serv.get_channel_by_name(channel_name) == NULL)
		return (msg.reply_format(ERR_NOSUCHCHANNEL, channel_name, serv.socket_id));
	if (msg.cmd_param.find(':') == std::string::npos)
		return (return_topic(msg, serv.get_channel_by_name(channel_name)));

	Channel		*channel = serv.get_channel_by_name(channel_name);

	new_topic = msg.cmd_param.substr(msg.cmd_param.find(':') + 1, msg.cmd_param.size());
	if (are_client_credentials_correct(msg, channel) == false)
		return ;
	change_topic(msg, channel, new_topic);
}

bool	Topic::are_client_credentials_correct(Message& msg, Channel *channel)
{
	std::map<int, int>::iterator	client_check(channel->_clients.find(msg.get_emitter()));

	if (client_check == channel->_clients.end())
	{
		msg.reply_format(ERR_NOTONCHANNEL, channel->_name, serv.socket_id);
		return (false);
	}
	if (channel->_is_topic_restricted == true
		&& channel->is(client_check->second, channel->CHANOP) == false)
	{
		msg.reply_format(ERR_CHANOPRIVSNEEDED, channel->_name, serv.socket_id);
		return (false);
	}
	return (true);
}

void	Topic::return_topic(Message &msg, Channel *channel)
{
	std::vector<std::string>	replies;
	std::vector<std::string>	replace;

	replies.push_back(RPL_TOPIC);
	replies.push_back(RPL_TOPICWHOTIME);

	replace.push_back(channel->_name);
	replace.push_back(channel->_topic);

	replace.push_back(channel->_name);
	replace.push_back(channel->_topic_author);
	replace.push_back(channel->_topic_date);
	msg.reply_format(replies, replace);
}

void	Topic::change_topic(Message &msg, Channel *channel, std::string new_topic)
{
	std::time_t					time = std::time(0);
	std::tm						*now = std::localtime(&time);
	std::stringstream			ss;

	ss << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday;
	ss >> channel->_topic_date;
	channel->_topic_author = serv.client_list.find(msg.get_emitter())->second.nickname;
	channel->_topic = new_topic;

	return_topic(msg, channel);
	msg.target.clear();

	for (std::map<int, int>::iterator it = channel->_clients.begin();
		it != channel->_clients.end(); ++it)
	{
		msg.target.insert(it->first);
	}
}
