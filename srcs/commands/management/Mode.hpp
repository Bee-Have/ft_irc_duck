#pragma once

#include "ICommand.hpp"

struct Mode : public ICommand
{
	Mode(Server& p_serv);
	void	execute(Message& msg);

private:
	enum FlagStatus
	{
		UNCHANGED = 0,
		ADD = 1,
		REMOVE = 2
	};

	const std::string	_usermodes_chars;
	const std::string	_chanmodes_chars;

	std::string	_replymodes;
	std::map<char, int> _all_usermodes;
	std::map<char, int> _all_chanmodes;

	void	_current_mode(Message& msg, std::string& name);
	void	_reset_modes();

	void	_fill_mod_maps(Message& msg, bool is_channel);
	void	_apply_mode_changes(Message& msg, std::string parameters, bool is_channel);
	void	_format_replymodes();

	void	_client_i(Message& msg);
	void	_client_O(Message& msg, std::string param);

	void	_channel_i(Message& msg);
	void	_channel_t(Message& msg);
	void	_channel_k(Message& msg, std::string param);
	void	_channel_o(Message& msg, std::string param);
	void	_channel_l(Message& msg, std::string param);
};
