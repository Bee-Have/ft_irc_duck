#include "Mode.hpp"

Mode::Mode(Server& p_serv) : ICommand(p_serv)
{
	_reset_modes();
}

void Mode::_reset_modes()
{
	_all_usermodes['i'] = UNCHANGED;
	_all_usermodes['O'] = UNCHANGED;
}

void	Mode::execute(Message& msg)
{
	_usermodes.clear();
	_reset_modes();

	std::string	parameters;
	std::string	name;

	if (msg.cmd_param.find(' ') == std::string::npos)
		name = msg.cmd_param;
	else
	{
		name = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
		msg.cmd_param = msg.cmd_param.substr(msg.cmd_param.find(' ') + 1);
	}
	if (msg.cmd_param[0] != '+' && msg.cmd_param[0] != '-')
		return (msg.reply_format(ERR_MODEBADFORMAT, "", serv.socket_id));

	if (msg.cmd_param.find(' ') != std::string::npos)
	{
		parameters = msg.cmd_param.substr(msg.cmd_param.find(' ') + 1);
		msg.cmd_param = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
	}

	std::cout << "NAME [" << name << ']' << std::endl;
	if (name[0] == '#' || name[0] == '&')
		std::cout << "ITS A CHANNEL" << std::endl;
	else
	{
		std::cout << "ITS A CLIENT" << std::endl;
		std::cout << "PARAM [" << msg.cmd_param << ']' << std::endl;
		_client_handling(msg, name);
	}
	_apply_usermodes(msg, parameters);
	_format_usermodes();
	if (_usermodes.empty() == false)
		msg.reply_format(RPL_UMODEIS, _usermodes, serv.socket_id);
}

void	Mode::_apply_usermodes(Message& msg, std::string parameters)
{
	_client_i(msg);
	_client_O(msg, parameters);
}

void	Mode::_client_handling(Message& msg, std::string nick)
{
	char	sign;

	if (serv.get_client_by_nickname(nick) == -1)
		return (msg.reply_format(ERR_NOSUCHNICK, nick, serv.socket_id));
	if (serv.get_client_by_nickname(nick) != msg.get_emitter())
		return (msg.reply_format(ERR_USERSDONTMATCH, "", serv.socket_id));
	for (std::string::iterator it = msg.cmd_param.begin();
		it != msg.cmd_param.end(); ++it)
	{
		if (*it == '+' || *it == '-')
		{
			sign = *it;
			continue;
		}
		if (*it == 'i' || *it == 'O')
			_all_usermodes[*it] = (sign == '+') ? ADD : REMOVE;
		else
		{
			Message response(msg);
			response.reply_format(ERR_UMODEUNKNOWNMODEFLAG, "", serv.socket_id);
			serv.msgs.push_back(response);
		}
	}
}

void	Mode::_client_i(Message& msg)
{
	int& i = _all_usermodes['i'];

	if (i == UNCHANGED)
		return ;
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
		return ;
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

void	Mode::_format_usermodes()
{
	for (std::map<char, int>::iterator it = _all_usermodes.begin();
		it != _all_usermodes.end(); ++it)
	{
		if (it->second == ADD)
			_usermodes.append("+");
		else if (it->second == REMOVE)
			_usermodes.append("-");
		if (it->second != UNCHANGED)
			_usermodes.push_back(it->first);
	}
}
