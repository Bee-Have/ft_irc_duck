#include "Server.hpp"
#include "Message.hpp"

/**
 * This should never be used. Server MUST be created with a PORT and PASSWORD
 */
Server::Server(void)
{}

/**
 * @brief Construct a new Server::server object.
 * It also opens and bind the server socket
 * as well as start to listen on said socket.
 * It will also setup the function pointer for all the commands
 * 
 * @param new_port the new port of the server
 * @param new_pass the password of the server
 */
Server::Server(int new_port, char *new_pass): _port(new_port), _pass(new_pass), _oper_name("Cthulhu"), _oper_pass("R'lyeh"), _oper_socket(-1)
{
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0)
	{
		std::cerr << ERR_SOCKCREATEFAIL;
		return ;
	}
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(_port);
	_server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(_socket, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0)
	{
		std::cerr << ERR_SOCKBINDFAIL;
		return ;
	}
	if (listen(_socket, MAX_CLIENT) < 0)
	{
		std::cerr << ERR_SOCKLISTENFAIL;
		return ;
	}

	commands["PASS"] = &Server::pass;
	commands["NICK"] = &Server::nick;
	commands["USER"] = &Server::user;
	commands["OPER"] = &Server::oper;
	commands["PRIVMSG"] = &Server::privmsg;
	commands["JOIN"] = &Server::join;
	commands["PING"] = &Server::ping;
}

Server::Server(const Server &cpy)
{
	(void)cpy;
}

/**
 * @brief Destroy the Server::server object
 * @note It also closes all client sockets as well as the server socket
 */
Server::~Server(void)
{
	close(_socket);
	client_list.clear();
}

Server	&Server::operator=(const Server &assign)
{
	if (this != &assign)
	{
		_port = assign._port;
		_pass.assign(assign._pass);
		_socket = assign._socket;
		_server_addr = assign._server_addr;
		_client_addr = assign._client_addr;
		msgs.clear();
		msgs.insert(msgs.begin(), assign.msgs.begin(), assign.msgs.end());
		client_list.clear();
		client_list.insert(assign.client_list.begin(), assign.client_list.end());
	}
	return (*this);
}

int	Server::get_socket(void) const
{
	return (_socket);
}

/**
 * @brief Adds a new client to the Client_list

 * @note A new socket will be given to the client using "accept()".
 * The socket will then be checked using "getsockopt()"
 */
void	Server::add_client(void)
{
	socklen_t		client_addr_len = sizeof(_client_addr);
	Client	new_client(accept(_socket, (struct sockaddr *)&_client_addr, &client_addr_len));

	if (new_client._socket < 0)
	{
		std::cerr << errno << ' ' << SERVERNAME << " :" << strerror(errno) << "\r\n";
		return ;
	}

	int	socket_error;
	socklen_t optlen = sizeof(socket_error);
	if (getsockopt(new_client._socket, SOL_SOCKET, SO_ERROR, &socket_error, &optlen) >= 0)
	{
		if (socket_error != 0)
		{
			std::cout << "INVALID SOCKET :(" << std::endl;
			close(new_client._socket);
			return ;
		}
	}
	else
		std::cerr << "Error : " << errno << " : " << strerror(errno) << std::endl;

	std::cout << "NEW CLIENT :D" << std::endl;
	client_list.insert(std::make_pair(new_client._socket, new_client));
}

/**
 * @brief Delete specific client from client list.
 * 
 * @note This implies :
 * 1.deleting client fd from messages to be sent.
 * 2.closing client socket before deleting it's object
 * !(if a message send by the client exists, it will only be deleted if they are no target to send the message to)
 * 
 * @param fd the client defined by it's socket
 */
void	Server::del_client(int fd)
{
	// delete client from messages
	for (std::vector<Message>::iterator it = msgs.begin(); it != msgs.end(); ++it)
	{
		if (it->get_emitter() == fd && (it->target.empty() == true
			|| (it->target.size() == 1 && it->target.find(fd) != it->target.end())))
		{
			it = msgs.erase(it);
			if (msgs.empty() == true)
				break ;
		}
		else if (it->target.find(fd) != it->target.end())
			it->target.erase(it->target.find(fd));
	}
	// delete client from channels
	for (std::map<std::string, Channel>::iterator it = _channel_list.begin();
		it != _channel_list.end();)
	{
		if (it->second._clients.empty() == false
			&& it->second._clients.find(fd) != it->second._clients.end())
		{
			it->second._clients.erase(it->second._clients.find(fd));
			if (it->second._clients.empty() == true)
			{
				_channel_list.erase(it);
				it = _channel_list.begin();
			}
			else
			{
				// ! This behaviour might become problematic with the implementation of QUIT
				// TODO : check QUIT message & alter this behaviour
				Message	new_msg(_socket);
				new_msg.error_format(RPL_CLIENTLEFT, client_list.find(fd)->second.nickname, _socket);
				new_msg.target.clear();
				for (std::map<int, int>::iterator it_chan_client = it->second._clients.begin();
					it_chan_client != it->second._clients.end(); ++it_chan_client)
				{
					new_msg.target.insert(new_msg.target.end(), it_chan_client->first);
				}
				++it;
			}
		}
		else
			++it;
	}
	close(fd);
	client_list.erase(client_list.find(fd));
	std::cout << "BYE BYE CLIENT" << std::endl;
}

/**
 * @brief Finds the maximum value fd existing in Client_list and returns it for "select()"
 * 
 * @return the maximum fd found in Client_list
 */
int	Server::get_max_fd(void) const
{
	int	max_fd = _socket;

	for (std::map<int, Client>::const_iterator it = client_list.begin(); it != client_list.end(); ++it)
	{
		if (it->first > max_fd)
			max_fd = it->first;
	}
	return (max_fd);
}

/**
 * @brief Adds server fd and all client fd we wish to read on for "select()"
 * @note (this will always return all fds in Client_list)
 * 
 * @return fd_set of the client fd and server socket
 */
fd_set	Server::get_read_fds(void) const
{
	fd_set	read_fds;

	FD_ZERO(&read_fds);
	FD_SET(_socket, &read_fds);
	for (std::map<int, Client>::const_iterator it = client_list.begin()
		; it != client_list.end(); ++it)
	{
		FD_SET(it->first, &read_fds);
	}
	return (read_fds);
}

/**
 * @brief Adds all fds from messages to be sent for "select()"
 * @note this is based on the fd found in message::target
 * 
 * @return fd_set of the clients fd we want to write on
 */
fd_set	Server::get_write_fds(void) const
{
	fd_set	write_fds;

	FD_ZERO(&write_fds);

	for (std::vector<Message>::const_iterator	it_msg = msgs.begin();
		it_msg != msgs.end(); ++it_msg)
	{
		for (std::set<int>::iterator it_fd = it_msg->target.begin();
			it_fd != it_msg->target.end(); ++it_fd)
		{
			if (FD_ISSET(*it_fd, &write_fds) == 0)
				FD_SET(*it_fd, &write_fds);
		}
	}
	return (write_fds);
}

/**
 * @brief Checks wethere param nickname exists in Client_list
 * 
 * @param nickname the nickname to check
 * @return int -1 if no client is found.
 * else it will return the fd/socket of the client
 */
int	Server::_get_client_by_nickname(std::string nickname)
{
	for (std::map<int, Client>::iterator it = client_list.begin();
		it != client_list.end(); ++it)
	{
		if (nickname.compare(it->second.nickname) == 0)
			return (it->first);
	}
	return (-1);
}

std::string	Server::oper_command_check(std::string oper, std::string pass)
{
	if (_oper_socket != -1)
		return (ERR_CANNOTBECOMEOPER);
	if (_oper_name != oper)
		return (ERR_NOSUCHOPER);
	if (_oper_pass != pass)
		return (ERR_PASSWDMISMATCH);
	_oper_socket = msg.get_emitter();
	return (RPL_YOUREOPER);
}

/**
 * @brief sends a message to a specific user.
 * 
 * @param msg the message to send
 */
void	Server::privmsg(Message &msg)
{
	std::pair<int, std::string>	target;
	std::string	text;

	if (msg.cmd_param.find(':') == std::string::npos)
		return (error_message(msg, "", ERR_NOTEXTTOSEND));
	if (msg.cmd_param[0] == ':')
		return (error_message(msg, "", ERR_NONICKNAMEGIVEN));
	target.second = msg.cmd_param.substr(0, msg.cmd_param.find(':') - 1);
	target.first = _get_client_by_nickname(target.second);
	text = msg.cmd_param.substr(msg.cmd_param.find(':'), msg.cmd_param.size());
	if (target.first == -1)
		return (error_message(msg, target.second, ERR_NOSUCHNICK));
	msg.target.clear();
	msg.target.insert(target.first);
	msg.text = ":";
	msg.text.append(client_list.find(msg.get_emitter())->second.nickname);
	msg.text.append(" PRIVMSG ");
	msg.text.append(text);
	msg.text.append("\r\n");
}

/**
 * @brief split JOIN parameters to better use them
 * @note this function will split the parameters into vector<string>,
 * it will be used up to two times, once for the keys (if there are keys),
 * and once for the channel names
 * 
 * @param str the string to split into multiple parameters
 * @return std::vector<std::string> of either channels or keys
 */
static std::vector<std::string>	split_join_cmd(std::string &str)
{
	std::vector<std::string>	result;
	size_t						comma = str.find(',');

	if (comma == std::string::npos)
	{
		result.push_back(str);
		return (result);
	}
	while (comma != std::string::npos)
	{
		result.push_back(str.substr(0, comma));
		str.erase(0, comma + 1);
		comma = str.find(',');
	}
	if (str.empty() == false)
		result.push_back(str);
	return (result);
}

/**
 * @brief This function will setup the error for too many spaces in JOIN parameters.
 * 
 * @param msg the message to setup as ERR_NOSUCHANNEL (see define.hpp)
 */
void	Server::join_space_error(Message &msg)
{
	std::string	channel_name;
	int			begin;
	int			end;

	if (msg.cmd_param.find_first_of(",") < msg.cmd_param.find_first_of(" "))
	{
		begin = msg.cmd_param.find(",") + 1;
		if (msg.cmd_param.find(",", begin) < msg.cmd_param.find(" ", msg.cmd_param.find_first_of(" ") + 1))
			end = msg.cmd_param.find(",", begin) - begin;
		else
			end = msg.cmd_param.find(" ", msg.cmd_param.find_first_of(" ") + 1) - begin;
		channel_name = msg.cmd_param.substr(begin, end);
	}
	error_message(msg, channel_name, ERR_NOSUCHCHANNEL);
}

/**
 * @brief simple boolean function to check wether the supplied channel name 
 * is allowed or not.
 * 
 * @param chan_name the channel name to check
 * @return true = the channel name is allowed
 * @return false = the channel name is not allowed
 */
static bool	is_channel_name_allowed(std::string chan_name)
{
	if (chan_name[0] != '#' && chan_name[0] != '&')
		return (false);
	if (chan_name.find_first_not_of(NICK_GOOD_CHARACTERS, 1) != std::string::npos)
		return (false);
	return (true);
}

/**
 * @brief this function will setup all the necessary reply messages once a 
 * client has successfully joined a channel
 * @note it will also send a message to all the other channel members to warn 
 * them a new member has arrived
 * 
 * @param msg the message to alter
 * @param chan the name of the channel that was joined
 */
void	Server::new_chan_member_sucess(Message msg, std::string chan)
{
	Channel						channel_cpy(_channel_list.find(chan)->second);
	std::vector<std::string>	replies(1, RPL_JOIN);
	std::vector<std::string>	replace(1, chan);
	Message						reply(msg.get_emitter());
	Message						new_member_warning(msg.get_emitter());

	reply_message(new_member_warning, RPL_JOIN, chan);
	new_member_warning.target.clear();

	if (_channel_list.find(chan)->second._topic.empty() == false)
		replies.push_back(_channel_list.find(chan)->second._topic);
	replies.push_back(RPL_NAMREPLY);
	// TODO : check when modes is implemented that there isnt a if here to be added
	replace.push_back("=");
	replace.push_back(chan);
	reply_replace_curly_brackets(*replies.rbegin(), channel_cpy._clients.size());
	for (std::map<int, int>::iterator it = channel_cpy._clients.begin();
		it != channel_cpy._clients.end(); ++it)
	{
		std::string	nick(client_list.find(it->first)->second.nickname);
		if (channel_cpy._is(it->second, channel_cpy.CHANOP) == true)
			nick.insert(0, "@");
		replace.push_back(nick);
		if (it->first != msg.get_emitter())
			new_member_warning.target.insert(it->first);
	}
	replies.push_back(RPL_ENDOFNAMES);
	replace.push_back(chan);
	reply_message(reply, replies, replace);
	msgs.push_back(reply);
	if (channel_cpy._clients.size() > 1)
		msgs.push_back(new_member_warning);
}

void	Server::join_create_channel(Message msg, std::string chan_name)
{
	Channel							new_chan(msg.get_emitter(), chan_name);
	std::pair<std::string, Channel>	new_pair(chan_name, new_chan);

	_channel_list.insert(new_pair);
	new_chan_member_sucess(msg, chan_name);
}

void	Server::join_check_existing_chan(Message msg, Channel *channel, std::vector<std::string> keys)
{
	Message	error(msg.get_emitter());

	if (channel->_is(channel->_clients.find(msg.get_emitter())->second, channel->MEMBER) == true)
		return ;
	if (channel->_key.empty() == false
		&& (keys.empty() == true || channel->_key != *keys.begin()))
	{
		error_message(error, channel->_name, ERR_BADCHANNELKEY);
		msgs.push_back(error);
	}
	if (channel->_is(channel->_clients.find(msg.get_emitter())->second, channel->INVITED) == true)
		_channel_list.find(channel->_name)->second.add_new_member(msg.get_emitter());
	else
	{
		if (channel->_is_invite_only == true)
		{
			error_message(error, channel->_name, ERR_INVITEONLYCHAN);
			msgs.push_back(error);
		}
		else
			_channel_list.find(channel->_name)->second.add_new_member(msg.get_emitter());
	}
	if (error.text.empty() == true)
		new_chan_member_sucess(msg, channel->_name);
}

void	Server::join_channel(Message msg, std::vector<std::string> chans, std::vector<std::string> keys)
{
	for (std::vector<std::string>::iterator it_chan = chans.begin();
		it_chan != chans.end(); ++it_chan)
	{
		if (it_chan->empty() == true || is_channel_name_allowed(*it_chan) == false)
		{
			Message	error(msg.get_emitter());
			error_message(error, *it_chan, ERR_NOSUCHCHANNEL);
			std::cout << "ERR BAD CHAN [" << error.text << ']' << std::endl;
			msgs.push_back(error);
			if (keys.empty() == false)
				keys.erase(keys.begin());
			continue;
		}
		if (_channel_list.find(*it_chan) == _channel_list.end())
			join_create_channel(msg, *it_chan);
		else
		{
			Channel	*current_chan = &_channel_list.find(*it_chan)->second;
			join_check_existing_chan(msg, current_chan, keys);
		}
		if (keys.empty() == false)
			keys.erase(keys.begin());
	}
}

void	Server::join(Message &msg)
{
	std::string					tmp;
	std::vector<std::string>	channels;
	std::vector<std::string>	keys;

	if (msg.cmd_param.find(" ") != msg.cmd_param.find_last_of(" "))
		return (join_space_error(msg));
	if (msg.cmd_param.find(' ') != std::string::npos)
	{
		std::cout << "FOUND KEY" << std::endl;
		tmp = msg.cmd_param.substr(msg.cmd_param.find(' ') + 1, msg.cmd_param.size());
		keys = split_join_cmd(tmp);
		msg.cmd_param.erase(msg.cmd_param.find(' '), msg.cmd_param.size());
	}
	channels = split_join_cmd(msg.cmd_param);
	if (msg.cmd_param.empty() == true)
	{
		channels.back().append(",");
		return (error_message(msg, channels.back(), ERR_NOSUCHCHANNEL));
	}
	// for (std::vector<std::string>::iterator it = channels.begin()
	// 	; it != channels.end() ; ++it)
	// {
	// 	std::cout << "CHAN [" << *it << "]\n";
	// }
	join_channel(msg, channels, keys);
}

/**
 * @brief upon receiving PING, the server answers "PONG" with msg.cmd.param
 * 
 * @param msg the message containing the command
 * @note if there is no msg.cmd.param "error_message()" will be called
 */
void	Server::ping(Message &msg)
{
	msg.target.clear();
	msg.target.insert(msg.get_emitter());
	msg.text.assign("PONG ");
	msg.text.append(SERVERNAME);
	msg.text.append(" ");
	msg.text.append(msg.cmd_param);
	msg.text.append("\r\n");
}
