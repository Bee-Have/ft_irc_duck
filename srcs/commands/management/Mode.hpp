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
	// Contains "+/-"
	std::vector<std::string>	_sign;
	// Contains "o"
	std::vector<std::string>	_umode;
	// Contains "i/t/k/l"
	std::vector<std::string>	_cmode;

	std::string	usermodes;
	// TODO : transform this into std::map
	int			i;
	int			O;

	void	client_handling(Message& msg, std::string nick);
	void	client_i(Message& msg, char sign);
	void	client_O(Message& msg, char sign, std::string param);
	void	add_usermodes(Message& msg);
};
