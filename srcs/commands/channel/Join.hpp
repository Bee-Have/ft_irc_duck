#pragma once

#include <vector>
#include <utility>

#include "ICommand.hpp"
#include "Channel.hpp"

struct Join : public ICommand
{
	std::vector<std::string>	channels;
	std::vector<std::string>	keys;

	Join(Server &p_serv);

	void	execute(Message &msg);
	
	private :
	/**
	 * @brief This function will setup the error for too many spaces in JOIN parameters.
	 * 
	 * @param msg the message to setup as ERR_NOSUCHANNEL (see define.hpp)
	 */
	void	join_space_error(Message &msg);
	/**
	 * @brief split JOIN parameters to better use them
	 * @note this function will split the parameters into vector<string>,
	 * it will be used up to two times, once for the keys (if there are keys),
	 * and once for the channel names
	 * 
	 * @param str the string to split into multiple parameters
	 * @return std::vector<std::string> of either channels or keys
	 */
	std::vector<std::string>	split_join_cmd(std::string &str);
	void	join_channel(Message msg);
	/**
	 * @brief simple boolean function to check wether the supplied channel name 
	 * is allowed or not.
	 * 
	 * @param chan_name the channel name to check
	 * @return true = the channel name is allowed
	 * @return false = the channel name is not allowed
	 */
	bool	is_channel_name_allowed(std::string chan_name);
	void	join_create_channel(Message msg, std::string chan_name);
	/**
	 * @brief this function will setup all the necessary reply messages once a 
	 * client has successfully joined a channel
	 * @note it will also send a message to all the other channel members to warn 
	 * them a new member has arrived
	 * 
	 * @param msg the message to alter
	 * @param chan the name of the channel that was joined
	 */
	void	join_check_existing_chan(Message msg, Channel *channel);
	void	new_chan_member_sucess(Message msg, std::string chan);
	void	special_argument(Message &msg);
};

