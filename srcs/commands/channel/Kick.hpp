#pragma once

#include "ICommand.hpp"

struct Kick : public ICommand
{
	std::string	comment;

	Kick(Server& p_serv);

	void	execute(Message& msg);

	private :
	std::vector<std::string>	return_kick_target(Message& msg);
	bool	is_issuer_membership_valid(Message &msg, Channel channel);
};
