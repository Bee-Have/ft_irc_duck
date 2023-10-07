#pragma once

#include "ICommand.hpp"

struct Mode : public ICommand
{
	Mode(Server &p_serv);
	void	execute(Message	&msg);
	
	private:
	enum FlagStatus
	{
		UNCHANGED = 0,
		ADD = 1,
		REMOVE = 2
	};

	std::string	_usermodes;
	std::map<char, int> _all_usermodes;

	void 	_current_mode(Message& msg, std::string& name);
	void	_reset_modes();
	void	_client_handling(Message& msg);
	void	_client_i(Message& msg);
	void	_client_O(Message& msg, std::string param);
	void	_format_usermodes();
	void 	_apply_usermodes(Message& msg, std::string parameters);
};
