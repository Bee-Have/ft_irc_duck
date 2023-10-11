#include "Mode.hpp"
#include <sstream>

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

	_mode_params[0] = std::make_pair('\0', "");
	_mode_params[1] = std::make_pair('\0', "");
	_mode_params[2] = std::make_pair('\0', "");
}

void Mode::_send_reply(Message& msg, Channel* channel, bool has_been_changed)
{
	if (has_been_changed == false)
	{
		if (channel != NULL)
		{
			std::vector<std::string>	reply(1, RPL_CHANNELMODEIS);
			std::vector<std::string>	replace;

			replace.push_back(channel->_name);
			replace.push_back(_replymodes);
			msg.reply_format(reply, replace);
		}
		else
			msg.reply_format(RPL_UMODEIS, _replymodes, serv.socket_id);
	}
	else
	{
		std::vector<std::string>	reply(1, RPL_CHANGEMODE);
		std::vector<std::string>	replace;

		if (channel != NULL)
			replace.push_back(channel->_name);
		else
			replace.push_back(serv.client_list.find(msg.get_emitter())->second.nickname);
		replace.push_back(_replymodes);
		msg.reply_format(reply, replace);
		if (channel != NULL)
		{
			for (std::map<int, int>::iterator it = channel->_clients.begin();
				it != channel->_clients.end(); ++it)
			{
				if (it->first != msg.get_emitter())
					msg.target.insert(it->first);
			}
		}
	}
}

void	Mode::_current_mode(Message& msg, Channel* channel)
{
	std::string channel_suffix;

	if (channel != NULL)
	{
		if (channel->_is_invite_only == true)
			_all_chanmodes['i'] = ADD;
		if (channel->_is_topic_restricted == true)
			_all_chanmodes['t'] = ADD;
		if (channel->_key.empty() == false)
		{
			_all_chanmodes['k'] = ADD;
			if ((channel->_clients.find(msg.get_emitter())->second & Channel::CHANOP) == Channel::CHANOP)
				_mode_params[0] = std::make_pair('k', channel->_key);
			else
				_mode_params[0] = std::make_pair('k', "*");
		}
		if (channel->_member_limit != -1)
		{
			_all_chanmodes['l'] = ADD;
			std::stringstream ss;
			ss << channel->_member_limit;
			_mode_params[1] = std::make_pair('l', ss.str());
		}
	}
	else
	{
		if (serv.client_list.find(msg.get_emitter())->second._is_invisible == true)
			_all_usermodes['i'] = ADD;
		if (serv._oper_socket == msg.get_emitter())
			_all_usermodes['O'] = ADD;
	}
	_format_replymodes(channel != NULL);
	if (_replymodes.empty() == true)
		_replymodes = '+';
	_send_reply(msg, channel, false);
}

void Mode::_get_mode_params(Message& msg, bool is_channel)
{
	size_t param_start = msg.cmd_param.find_first_of(" \t");
	if (param_start == std::string::npos)
		return;
	std::string tmp_mode_params = msg.cmd_param.substr(param_start + 1);
	msg.cmd_param = msg.cmd_param.substr(0, param_start);

	if (is_channel == false)
	{
		_mode_params[0] = std::make_pair('O', tmp_mode_params);
	}
	else
	{
		int	i = 0;
		size_t space_pos = tmp_mode_params.find(" ");

		while (space_pos != std::string::npos)
		{
			_mode_params[i].second = tmp_mode_params.substr(0, space_pos);
			tmp_mode_params = tmp_mode_params.substr(space_pos + 1);
			space_pos = tmp_mode_params.find(" ");
			++i;
			if (i == 3)
			{
				Message	warning(msg);
				warning.reply_format(ERR_TOOMANYPARAM, msg.cmd, serv.socket_id);
				serv.msgs.push_back(warning);
				break;
			}
		}
		if (tmp_mode_params.empty() == false && i < 3)
			_mode_params[i].second = tmp_mode_params;
	}
}

void Mode::_map_mode_params(char mode)
{
	for (size_t i = 0; i < 3; ++i)
	{
		if (_mode_params[i].first == '\0')
		{
			_mode_params[i].first = mode;
			return;
		}
		else if (_mode_params[i].first == mode)
		{
			return;
		}
	}
}

std::string Mode::_get_param(char mode)
{
	for (size_t i = 0; i < 3; ++i)
	{
		if (_mode_params[i].first == mode)
			return (_mode_params[i].second);
	}
	return ("");
}

void	Mode::execute(Message& msg)
{
	_replymodes.clear();
	_reset_modes();

	std::string	name = msg.cmd_param.substr(0, msg.cmd_param.find_first_of(" \t\0\r\n"));
	const size_t space_pos = msg.cmd_param.find_first_of(" \t");
	Channel* channel = serv.get_channel_by_name(name);
	bool is_channel = (channel != NULL);
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
	if (is_channel == true && msg.cmd_param == "b")
		return (msg.reply_format(RPL_ENDOFBANLIST, channel->_name, serv.socket_id));
	if (msg.cmd_param[0] != '+' && msg.cmd_param[0] != '-')
		return (msg.reply_format(ERR_MODEBADFORMAT, "", serv.socket_id));

	_get_mode_params(msg, is_channel);
	if (msg.target.empty() == false)
		return;

	_fill_mod_maps(msg, is_channel);
	_apply_mode_changes(msg, channel);
	_format_replymodes(is_channel);
	if (_replymodes.empty() == false)
		_send_reply(msg, channel, true);
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
		if ((sign == '+' && (*it == 'o' || *it == 'l' || *it == 'k')) ||
			(sign == '-' && (*it == 'o' || *it == 'k')))
			_map_mode_params(*it);
	}
}

void	Mode::_apply_mode_changes(Message& msg, Channel* channel)
{
	if (channel != NULL)
	{
		if (channel->is(channel->_clients.find(msg.get_emitter())->second, channel->CHANOP) == false)
		{
			_reset_modes();
			return (msg.reply_format(ERR_CHANOPRIVSNEEDED, channel->_name, serv.socket_id));
		}
		_channel_i(msg, channel);
		_channel_t(msg, channel);
		_channel_k(msg, channel);
		_channel_o(msg, channel);
		_channel_l(msg, channel);
	}
	else
	{
		_client_i(msg);
		_client_O(msg);
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
		std::string suffix;
		for (std::string::iterator it = _replymodes.begin(); it != _replymodes.end(); ++it)
		{
			if (*it == '+')
			{
				if (tmp.find('+') == std::string::npos)
					tmp.push_back('+');
				tmp.push_back(*(it + 1));
				if (*(it + 1) == 'o' || *(it + 1) == 'l' || *(it + 1) == 'k')
					suffix += " " + _get_param(*(it + 1));
			}
		}
		for (std::string::iterator it = _replymodes.begin(); it != _replymodes.end(); ++it)
		{
			if (*it == '-')
			{
				if (tmp.find('-') == std::string::npos)
					tmp.push_back('-');
				tmp.push_back(*(it + 1));
				if (*(it + 1) == 'o' || *(it + 1) == 'k')
					suffix += " " + _get_param(*(it + 1));
			}
		}
		_replymodes = tmp + suffix;
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
}

void	Mode::_client_O(Message& msg)
{
	Message response(msg);
	int& O = _all_usermodes['O'];

	if (O == UNCHANGED)
		return;
	else if (O == ADD)
	{
		response.cmd = "OPER";
		response.cmd_param = _mode_params[0].second;
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

void	Mode::_channel_i(Message& msg, Channel* channel)
{
	(void)msg;
	if (_all_chanmodes['i'] == UNCHANGED)
		return;
	else if (_all_chanmodes['i'] == ADD)
	{
		if (channel->_is_invite_only == true)
			_all_chanmodes['i'] = UNCHANGED;
		else
			channel->_is_invite_only = true;
	}
	else if (_all_chanmodes['i'] == REMOVE)
	{
		if (channel->_is_invite_only == false)
			_all_chanmodes['i'] = UNCHANGED;
		else
			channel->_is_invite_only = false;
	}
}

void	Mode::_channel_t(Message& msg, Channel* channel)
{
	(void)msg;
	if (_all_chanmodes['t'] == UNCHANGED)
		return;
	else if (_all_chanmodes['t'] == ADD)
	{
		if (channel->_is_topic_restricted == true)
			_all_chanmodes['t'] = UNCHANGED;
		else
			channel->_is_topic_restricted = true;
	}
	else if (_all_chanmodes['t'] == REMOVE)
	{
		if (channel->_is_topic_restricted == false)
			_all_chanmodes['t'] = UNCHANGED;
		else
			channel->_is_topic_restricted = false;
	}
}

void	Mode::_channel_k(Message& msg, Channel* channel)
{
	if (_all_chanmodes['k'] == UNCHANGED)
		return;
	std::string param = _get_param('k');
	if (param.empty() == true)
	{
		_all_chanmodes['k'] = UNCHANGED;
		Message	warning(msg);
		warning.reply_format(ERR_NEEDMOREPARAMS, msg.cmd, serv.socket_id);
		serv.msgs.push_back(warning);
	}
	if (_all_chanmodes['k'] == ADD)
	{
		if (channel->_key == param)
			_all_chanmodes['k'] = UNCHANGED;
		else
			channel->_key = param;
	}
	else if (_all_chanmodes['k'] == REMOVE)
	{
		if (channel->_key.empty() == true || channel->_key != param)
			_all_chanmodes['k'] = UNCHANGED;
		else
			channel->_key.clear();
	}
}

void	Mode::_channel_o(Message& msg, Channel* channel)
{
	if (_all_chanmodes['o'] == UNCHANGED)
		return;
	std::string param = _get_param('o');
	int	client = serv.get_client_by_nickname(param);
	if (param.empty() == true)
	{
		_all_chanmodes['o'] = UNCHANGED;
		Message	warning(msg);
		warning.reply_format(ERR_NEEDMOREPARAMS, msg.cmd, serv.socket_id);
		serv.msgs.push_back(warning);
	}
	else if (channel->_clients.find(client) == channel->_clients.end())
	{
		Message	warning(msg);
		std::vector<std::string>	reply(1, ERR_USERNOTINCHANNEL);
		std::vector<std::string>	replace(1, param);

		_all_chanmodes['o'] = UNCHANGED;
		replace.push_back(channel->_name);
		warning.reply_format(reply, replace);
		serv.msgs.push_back(warning);
	}

	if (_all_chanmodes['o'] == ADD)
	{
		if (channel->is(channel->_clients.find(client)->second, channel->CHANOP) == true)
			_all_chanmodes['o'] = UNCHANGED;
		else
		{
			Message	warning(serv.client_list.find(client)->second);
			channel->_clients.find(client)->second |= channel->CHANOP;
			warning.reply_format(RPL_YOURECHANOP, channel->_name, serv.socket_id);
			serv.msgs.push_back(warning);
		}
	}
	else if (_all_chanmodes['o'] == REMOVE)
	{
		if (channel->is(channel->_clients.find(client)->second, channel->CHANOP) == false)
			_all_chanmodes['o'] = UNCHANGED;
		else
			channel->_clients.find(client)->second ^= Channel::CHANOP;
	}
}

void	Mode::_channel_l(Message& msg, Channel* channel)
{
	if (_all_chanmodes['l'] == UNCHANGED)
		return;
	std::string param = _get_param('l');
	std::stringstream ss(param);
	int new_member_limit;

	ss >> new_member_limit;
	if (ss.fail() == true || new_member_limit < 0)
	{
		_all_chanmodes['l'] = UNCHANGED;
		Message	warning(msg);
		warning.reply_format(ERR_NEEDMOREPARAMS, msg.cmd, serv.socket_id);
		serv.msgs.push_back(warning);
		return;
	}
	if (_all_chanmodes['l'] == ADD)
	{
		if (channel->_member_limit == new_member_limit)
			_all_chanmodes['l'] = UNCHANGED;
		else
			channel->_member_limit = new_member_limit;
	}
	else if (_all_chanmodes['l'] == REMOVE)
	{
		if (channel->_member_limit == -1)
			_all_chanmodes['l'] = UNCHANGED;
		else
			channel->_member_limit = -1;
	}
}
