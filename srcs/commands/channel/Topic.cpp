#include "Topic.hpp"

Topic::Topic(Server &p_server): ICommand(p_server)
{}

void	Topic::execute(Message &msg)
{
	std::string	new_topic;

	if (msg.cmd_param.empty() == true)
		return (msg.reply_format(ERR_NOSUCHCHANNEL, "", serv.socket_id));
	if (serv._channel_list.find(msg.cmd_param.substr(0, msg.cmd_param.find(' '))) == serv._channel_list.end())
		return (msg.reply_format(ERR_NOSUCHCHANNEL, msg.cmd_param.substr(0, msg.cmd_param.find(' ')), serv.socket_id));
	if (msg.cmd_param.find(':') == std::string::npos)
		return (return_topic(msg, &serv._channel_list.find(msg.cmd_param)->second));

	Channel		*channel = &serv._channel_list.find(msg.cmd_param.substr(0, msg.cmd_param.find(':') - 1))->second;

	std::cout << "CHAN [" << channel->_name << "]\n";
	new_topic = msg.cmd_param.substr(msg.cmd_param.find(':') + 1, msg.cmd_param.size());
	std::cout << "TOPIC [" << new_topic << "]\n";
	if (channel->_clients.find(msg.get_emitter()) == channel->_clients.end())
		return (msg.reply_format(ERR_NOTONCHANNEL, channel->_name, serv.socket_id));
	if (channel->_is(channel->_clients.find(msg.get_emitter())->second, channel->CHANOP) == false)
		return (msg.reply_format(ERR_CHANOPRIVSNEEDED, channel->_name, serv.socket_id));
	change_topic(msg, channel, new_topic);
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
