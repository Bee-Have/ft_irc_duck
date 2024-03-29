#include "User.hpp"

User::User(Server& p_server) : ICommand(p_server)
{}

int	User::get_correct_user_params(const std::string& params)
{
	bool second_param_given = false;
	bool third_param_given = false;
	size_t find_result;

	find_result = params.find_first_of(" \t"); ///< End of _username
	if (find_result == std::string::npos)
		return (-1);
	_username.assign(params.substr(0, find_result));

	find_result = params.find_first_not_of(" \t", find_result); ///< Start of second parameter
	if (find_result == std::string::npos)
		return (-1);
	second_param_given = true;
	find_result = params.find_first_of(" \t", find_result); ///< End of second parameter
	if (find_result == std::string::npos)
		return (-1);

	find_result = params.find_first_not_of(" \t", find_result); ///< Start of third parameter
	if (find_result == std::string::npos)
		return (-1);
	third_param_given = true;
	_host.assign(params.substr(find_result, params.find_first_of(": \t", find_result) - find_result));
	find_result = params.find_first_of(": \t", find_result); ///< End of third parameter
	if (find_result == std::string::npos)
		return (-1);

	find_result = params.find_first_not_of(" \t", find_result); ///< Start of _realname
	if (find_result == std::string::npos || params[find_result] != ':')
		return (-1);
	_realname.assign(params.substr(find_result + 1));

	if (_username.empty() == true || second_param_given == false ||
		third_param_given == false || _realname.empty() == true)
		return (-1);
	return (0);
}

void	User::execute(Message& msg)
{
	Client* command_emitter = &serv.client_list.find(msg.get_emitter())->second;

	if (serv.client_list.find(msg.get_emitter())->second.get_is_authenticated() == false) ///< PASS command has not been called
		return (setup_error_no_nickname(msg, ERR_NOTREGISTERED, ""));
	if (command_emitter->_realname.empty() == false) ///< USER command has already been called
		return (setup_error_no_nickname(msg, ERR_ALREADYREGISTRED, ""));
	if (get_correct_user_params(msg.cmd_param) == -1)
		return (setup_error_no_nickname(msg, ERR_NEEDMOREPARAMS, msg.cmd));

	command_emitter->_username = _username;
	command_emitter->_realname = _realname;
	command_emitter->host = _host;

	serv.register_client_if_able(msg.get_emitter()); ///< Register the client and send msgs if USER|NICK
}

void	User::setup_error_no_nickname(Message& msg, std::string error, std::string replace)
{
	int	client = msg.get_emitter();

	msg._emitter = serv.socket_id;
	msg.host = SERVERNAME;
	msg.reply_format(error, replace, serv.socket_id);
	msg.target.clear();
	msg.target.insert(client);
}
