#pragma once

#include "ICommand.hpp"

struct Mode : public ICommand
{
	Mode(Server &p_serv);
	void	execute(Message	&msg);
	
	private:
	// Contains "+/-"
	std::vector<std::string>	_sign;
	// Contains "o"
	std::vector<std::string>	_umode;
	// Contains "i/t/k/l"
	std::vector<std::string>	_cmode;
};
