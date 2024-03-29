#pragma once

#include <cctype>
#include "ICommand.hpp"

/**
 * @brief changes the nickname of a user ("msg.get_emitter()")
 * 
 * @note if the nickname in msg.cmd.param is not allowed or missing or already in use, error will be returned
 */
struct Nick : public ICommand
{
	Nick(Server &p_serv);
	
	/**
	 * @brief checks wether nickname is allowed.
	 * @note check irssi RFC for specification of nickname policy.
	 * This function is only called and used by "nick()"
	 * 
	 * @param nickname the nickname to check
	 * @return true if the nickname is allowed
	 * @return false if the nickname isn't allowed
	 */
	bool	is_nickname_allowed(const std::string &nickname) const;
	void	execute(Message &msg);
	void	setup_error(Message &msg, std::string error, std::string replace);
	void	add_nick_to_messages(Message& msg, std::string nickname);
	bool	is_nick_unique(std::string nickname);
};
