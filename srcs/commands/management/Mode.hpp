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

	void	_reset_modes();
	void	_client_handling(Message& msg, std::string nick);
	void	_client_i(Message& msg, char sign);
	void	_client_O(Message& msg, char sign, std::string param);
	void	_add_usermodes(Message& msg);
};
