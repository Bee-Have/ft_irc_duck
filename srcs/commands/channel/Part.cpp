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

void	part_loop_check(Message *msg)
{

	for (std::vector<std::string>::iterator it = channels.begin();
		it != channels.end(); ++it)
	{
		if (serv._channel_list.find(*it) == serv._channel_list.end())
		{
			Message	error(msg.get_emitter());
			error.reply_format(ERR_NOSUCH_CHANNEL)
		}
	}
}
