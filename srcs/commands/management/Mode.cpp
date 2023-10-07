#include "Mode.hpp"

Mode::Mode(Server& p_serv) :
	ICommand(p_serv),
	_usermodes_chars("iO"),
	_chanmodes_chars("itkol")
{
	_reset_modes();
}

void Mode::_reset_modes()
{
	_all_usermodes['i'] = UNCHANGED;
	_all_usermodes['O'] = UNCHANGED;

	_all_chanmodes['i'] = UNCHANGED;
	_all_chanmodes['t'] = UNCHANGED;
	_all_chanmodes['k'] = UNCHANGED;
	_all_chanmodes['o'] = UNCHANGED;
	_all_chanmodes['l'] = UNCHANGED;
}

void	Mode::_current_mode(Message& msg, Channel* channel)
{
	if (channel != NULL)
	{
		if (channel->_is_invite_only == true)
			_all_chanmodes['i'] = ADD;
		if (channel->_is_topic_restricted == true)
			_all_chanmodes['t'] = ADD;
		if (channel->_key.empty() == false)
			_all_chanmodes['k'] = ADD;
		if (channel->are_there_other_chanops() == true)
			_all_chanmodes['o'] = ADD;
		if (channel->_member_limit != -1)
			_all_chanmodes['l'] = ADD;
	}
	else
	{
		if (serv.client_list.find(msg.get_emitter())->second._is_invisible == true)
			_all_usermodes['i'] = ADD;
		if (serv._oper_socket == msg.get_emitter())
			_all_usermodes['O'] = ADD;
	}
	_format_replymodes(channel != NULL);
	msg.reply_format(RPL_UMODEIS, _replymodes, serv.socket_id);
}

void	Mode::execute(Message& msg)
{
	_replymodes.clear();
	_reset_modes();

	std::string	parameters;
	std::string	name = msg.cmd_param.substr(0, msg.cmd_param.find_first_of(" \t\0\r\n"));
	const size_t space_pos = msg.cmd_param.find_first_of(" \t");
	bool is_channel = (name[0] == '#' || name[0] == '&');
	Channel* channel = serv.get_channel_by_name(name);
	int	client = serv.get_client_by_nickname(name);

	if (is_channel == false && client == -1)
		return (msg.reply_format(ERR_NOSUCHNICK, name, serv.socket_id));
	if (is_channel == true && channel == NULL)
		return (msg.reply_format(ERR_NOSUCHCHANNEL, name, serv.socket_id));
	if (is_channel == false && client != msg.get_emitter())
		return (msg.reply_format(ERR_USERSDONTMATCH, "", serv.socket_id));
	if (space_pos == std::string::npos)
		return (_current_mode(msg, channel));
	else
		msg.cmd_param = msg.cmd_param.substr(space_pos + 1);
	if (msg.cmd_param[0] != '+' && msg.cmd_param[0] != '-')
		return (msg.reply_format(ERR_MODEBADFORMAT, "", serv.socket_id));

	size_t param_start = msg.cmd_param.find_first_of(" \t");
	if (param_start != std::string::npos)
	{
		parameters = msg.cmd_param.substr(param_start + 1);
		msg.cmd_param = msg.cmd_param.substr(0, param_start);
	}

	_fill_mod_maps(msg, is_channel);
	_apply_mode_changes(msg, parameters, is_channel);
	_format_replymodes(is_channel);
	if (_replymodes.empty() == false)
		msg.reply_format(RPL_UMODEIS, _replymodes, serv.socket_id);
}

void	Mode::_fill_mod_maps(Message& msg, bool is_channel)
{
	char	sign;

	for (std::string::iterator it = msg.cmd_param.begin();
		it != msg.cmd_param.end(); ++it)
	{
		if (*it == '+' || *it == '-')
		{
			sign = *it;
			continue;
		}
		if (is_channel == true && _chanmodes_chars.find(*it) != std::string::npos)
			_all_chanmodes[*it] = (sign == '+') ? ADD : REMOVE;
		else if (is_channel == false && _usermodes_chars.find(*it) != std::string::npos)
			_all_usermodes[*it] = (sign == '+') ? ADD : REMOVE;
		else
		{
			Message response(msg);
			response.reply_format(ERR_UMODEUNKNOWNMODEFLAG, "", serv.socket_id);
			serv.msgs.push_back(response);
		}
	}
}

void	Mode::_apply_mode_changes(Message& msg, std::string parameters, bool is_channel)
{
	if (is_channel == true)
	{
		// _channel_i(msg);
		// _channel_t(msg);
		// _channel_k(msg, parameters);
		// _channel_o(msg, parameters);
		// _channel_l(msg, parameters);
	}
	else
	{
		_client_i(msg);
		_client_O(msg, parameters);
	}
}

void	Mode::_format_replymodes(bool is_channel)
{
	std::map<char, int>* modmap = (is_channel == true) ? &_all_chanmodes : &_all_usermodes;

	for (std::map<char, int>::iterator it = modmap->begin();
		it != modmap->end(); ++it)
	{
		if (it->second == ADD)
			_replymodes.append("+");
		else if (it->second == REMOVE)
			_replymodes.append("-");
		if (it->second != UNCHANGED)
			_replymodes.push_back(it->first);
	}
	{
		std::string tmp;
		for (std::string::iterator it = _replymodes.begin(); it != _replymodes.end(); ++it)
		{
			if (*it == '+')
			{
				if (tmp.find('+') == std::string::npos)
					tmp.push_back('+');
				tmp.push_back(*(it + 1));
			}
		}
		for (std::string::iterator it = _replymodes.begin(); it != _replymodes.end(); ++it)
		{
			if (*it == '-')
			{
				if (tmp.find('-') == std::string::npos)
					tmp.push_back('-');
				tmp.push_back(*(it + 1));
			}
		}
		_replymodes = tmp;
	}
}

void	Mode::_client_i(Message& msg)
{
	int& i = _all_usermodes['i'];

	if (i == UNCHANGED)
		return;
	else if (i == ADD)
	{
		if (serv.client_list.find(msg.get_emitter())->second._is_invisible == true)
			i = UNCHANGED;
		else
			serv.client_list.find(msg.get_emitter())->second._is_invisible = true;
	}
	else if (i == REMOVE)
	{
		if (serv.client_list.find(msg.get_emitter())->second._is_invisible == false)
			i = UNCHANGED;
		else
			serv.client_list.find(msg.get_emitter())->second._is_invisible = false;
	}
	std::cout << "INVISIBLE [" << serv.client_list.find(msg.get_emitter())->second._is_invisible << ']';
}

void	Mode::_client_O(Message& msg, std::string param)
{
	Message response(msg);
	int& O = _all_usermodes['O'];

	if (O == UNCHANGED)
		return;
	else if (O == ADD)
	{
		response.cmd = "OPER";
		response.cmd_param = param;
		serv.commands["OPER"]->execute(response);
		serv.msgs.push_back(response);
		if (response.text.find("381 ") == std::string::npos)
			O = UNCHANGED;
	}
	else if (O == REMOVE)
	{
		if (serv._oper_socket != msg.get_emitter())
		{
			response.reply_format(ERR_NOPRIVILEGES, "", serv.socket_id);
			serv.msgs.push_back(response);
			O = UNCHANGED;
		}
		else
		{
			serv._oper_socket = -1;
		}
	}
}
