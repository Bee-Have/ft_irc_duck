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

	std::pair<char, std::string> _mode_params[3];

	void	_get_mode_params(Message& msg, bool is_channel);
	void	_map_mode_params(char mode);
	std::string	_get_param(char mode);

	void	_current_mode(Message& msg, Channel* channel);
	void	_reset_modes();

	void	_fill_mod_maps(Message& msg, bool is_channel);
	void	_apply_mode_changes(Message& msg, Channel* channel);
	void	_format_replymodes(bool is_channel);

	void	_client_i(Message& msg); ///< Invisible
	void	_client_O(Message& msg); ///< Server operator

	void	_channel_i(Message& msg, Channel* channel); ///< Invite Only
	void	_channel_t(Message& msg, Channel* channel); ///< Topic Restricted
	void	_channel_k(Message& msg, Channel* channel); ///< Key
	void	_channel_o(Message& msg, Channel* channel); ///< Operator
	void	_channel_l(Message& msg, Channel* channel); ///< Member limit

	void	_send_reply(Message& msg, Channel* channel, bool has_been_changed);
};
