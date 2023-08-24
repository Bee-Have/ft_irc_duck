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
		if (it->get_emmiter() == fd && (it->target.empty() == true
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
				reply_message(new_msg, RPL_CLIENTLEFT, client_list.find(fd)->second._nickname);
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
		if (nickname.compare(it->second._nickname) == 0)
			return (it->first);
	}
	return (-1);
}

static void	replace_rpl_err_text(Message &msg, std::string replace)
{
	int	begin = 0;
	int	end = 0;

	begin = msg.text.find('<');
	end = (msg.text.find('>') + 1) - begin;
	msg.text.replace(begin, end, replace);
}

/**
 * private fuction
 * @brief Setup an error message with the parameters
 * 
 * @param msg the message to turn into an error
 * @param prefix if there is something to replace in the prefix of the error
 * @param error the error declared in : "define.hpp"
 */
void	Server::error_message(Message &msg, std::string prefix, std::string error)
{
	msg.target.clear();
	msg.target.insert(msg.get_emmiter());

	msg.text = error;
	replace_rpl_err_text(msg, client_list.find(msg.get_emmiter())->second._nickname);
	if (prefix.empty() == false)
		replace_rpl_err_text(msg, prefix);
}

void	Server::reply_message(Message &msg, std::string reply, std::string replace)
{
	msg.target.clear();
	msg.target.insert(msg.get_emmiter());

	msg.text.clear();
	msg.text.append(reply);
	replace_rpl_err_text(msg, client_list.find(msg.get_emmiter())->second._nickname);
	if (msg.text.find('<') != std::string::npos)
		replace_rpl_err_text(msg, replace);
}

void	Server::reply_message(Message &msg, std::vector<std::string> &replies, std::vector<std::string> &replace)
{
	std::vector<std::string>::iterator it_replace = replace.begin();

	msg.target.clear();
	msg.target.insert(msg.get_emmiter());

	msg.text.clear();
	for (std::vector<std::string>::iterator it = replies.begin(); it != replies.end(); ++it)
	{
		msg.text.append(*it);
		replace_rpl_err_text(msg, client_list.find(msg.get_emmiter())->second._nickname);
		while (msg.text.find('<') != std::string::npos)
		{
			replace_rpl_err_text(msg, *it_replace);
			++it_replace;
		}
	}
}

static void	reply_replace_curly_brackets(std::string &reply, int replace_count)
{
	std::string	replace;
	int		start = reply.find('{');

	std::cout << "replace count : " << replace_count << "\n";

	replace = reply.substr(start + 1, reply.find('}') - start - 1);

	std::cout << "0 : [" << replace << "]\n";

	reply.erase(start + 1, replace.size());

	std::cout << "1 : " << reply << '\n';

	for (int i = 0; i < replace_count - 1; ++i)
	{
		std::cout << "2 : " << reply << '\n';
		// ! this line is the one that breaks everything
		reply.insert(start + 1, replace);
		std::cout << "3 : " << reply << '\n';
	}
	std::cout << "4 : " << reply << std::endl;
	reply.erase(start, 1);
	reply.erase(reply.find('}'), 1);
	std::cout << "5 : " << reply << std::endl;
}

/**
 * @brief Attempts to register a client into our server
 * 
 * @param msg the message containing the command.
 * this command will call "error_message()" if msg.cmd.param does not fit server password
 */
void	Server::pass(Message &msg)
{
	if (client_list.find(msg.get_emmiter())->second._is_registered == true)
		return (error_message(msg, "", ERR_ALREADYREGISTRED));
	if (_pass.compare(msg.cmd_param) != 0)
		return (error_message(msg, "", ERR_PASSWDMISMATCH));
	client_list.find(msg.get_emmiter())->second._is_registered = true;
	msg.text.clear();
}

/**
 * @brief checks wether nickname is allowed.
 * @note check irssi RFC for specification of nickname policy.
 * This function is only called and used by "nick()"
 * 
 * @param nickname the nickname to check
 * @return true if the nickname is allowed
 * @return false if the nickname isn't allowed
 */
static bool	is_nickname_allowed(std::string nickname)
{
	if (nickname.size() > 9)
		return (false);
	if (std::isdigit(nickname[0]) != 0 || nickname[0] == '-')
		return (false);
	if (nickname.find_first_not_of(NICK_GOOD_CHARACTERS) != std::string::npos)
		return (false);
	return (true);
}

/**
 * @brief changes the nickname of a user ("msg.get_emmiter()")
 * 
 * @param msg the message containing the command
 * @note if the nickname in msg.cmd.param is not allowed or missing or already in use, "error_message()" will be called
 */
void	Server::nick(Message &msg)
{
	std::string	nickname;

	if (msg.cmd_param.empty() == true)
		return (error_message(msg, "", ERR_NONICKNAMEGIVEN));
	nickname = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
	if (is_nickname_allowed(nickname) == false)
		return (error_message(msg, nickname, ERR_ERRONEUSNICKNAME));
	if (_get_client_by_nickname(nickname) != -1)
		return (error_message(msg, nickname, ERR_NICKNAMEINUSE));
	client_list.find(msg.get_emmiter())->second._nickname = nickname;
	msg.text.clear();
	// std::cout << "worked:" << client_list.find(msg.get_emmiter())->second._nickname << std::endl;
}

/**
 * @brief assigns a username and realname to a specific client ("msg.get_emmiter()")
 * 
 * @param msg the message containing the command
 * @note if client is unregistered of no username or realname is given, call "error_message()".
 * Likewise if everything is here replies will be added to msg.text as an answer
 */
void	Server::user(Message &msg)
{
	Client				&tmp(client_list.find(msg.get_emmiter())->second);
	std::vector<std::string>	replies;
	std::vector<std::string>	rpl_replace;
	std::time_t					time = std::time(0);
	std::tm						*now = std::localtime(&time);
	std::stringstream			ss;
	std::string					date;

	ss << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday;
	ss >> date;
	if (tmp._nickname.empty() == true)
		return (error_message(msg, "", ERR_NONICKNAMEGIVEN));
	if (msg.cmd_param.find(':') == std::string::npos
		|| msg.cmd_param.find(' ') == std::string::npos)
		return (error_message(msg, msg.cmd, ERR_NEEDMOREPARAMS));
	if (tmp._realname.empty() == false)
		return (error_message(msg, "", ERR_ALREADYREGISTRED));

	tmp._username = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
	tmp._realname = msg.cmd_param.substr(msg.cmd_param.find(':') + 1, msg.cmd_param.size());

	replies.push_back(RPL_WELCOME);
	replies.push_back(RPL_YOURHOST);
	replies.push_back(RPL_CREATED);
	replies.push_back(RPL_MYINFO);
	replies.push_back(RPL_ISUPPORT);

	rpl_replace.push_back(tmp._nickname);
	rpl_replace.push_back(date);

	reply_message(msg, replies, rpl_replace);
}

void	Server::oper(Message &msg)
{
	std::string	oper;
	std::string	pass;

	if (_oper_socket != -1)
		return (error_message(msg, "", ERR_CANNOTBECOMEOPER));
	if (msg.cmd_param.find(' ') == std::string::npos)
		return (error_message(msg, "", ERR_PASSWDMISMATCH));
	oper = msg.cmd_param.substr(0, msg.cmd_param.find(' '));
	if (_oper_name != oper)
		return (error_message(msg, oper, ERR_NOSUCHOPER));
	pass = msg.cmd_param.substr(msg.cmd_param.find(' ') + 1, msg.cmd_param.size());
	if (_oper_pass != pass)
		return (error_message(msg, "", ERR_PASSWDMISMATCH));
	
	_oper_socket = msg.get_emmiter();
	reply_message(msg, RPL_YOUREOPER, "");
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
	msg.text.append(client_list.find(msg.get_emmiter())->second._nickname);
	msg.text.append(" PRIVMSG ");
	msg.text.append(text);
	msg.text.append("\r\n");
}

// TODO : resize result from the begining depending on number of ','
// This will avoid doing too many memories allocations
static std::vector<std::string>	split_join_cmd(std::string &str)
{
	std::vector<std::string>	result;

	if (str.find(',') == std::string::npos)
	{
		result.push_back(str);
		return (result);
	}
	while (str.find(',') != std::string::npos)
	{
		result.push_back(str.substr(0, str.find(',')));
		str.erase(str.find(',') + 1, str.size());
	}
	if (str.empty() == false)
		result.push_back(str);
	return (result);
}

void	Server::join_space_error_behavior(Message &msg)
{
	std::string	channel_name;
	int			begin;
	int			end;

	if (msg.cmd_param.find_first_of(",") < msg.cmd_param.find_first_of(" "))
	{
		begin = msg.cmd_param.find("," + 1);
		if (msg.cmd_param.find(",", begin) < msg.cmd_param.find(" ", msg.cmd_param.find_first_of(" ") + 1))
			end = msg.cmd_param.find(",", begin) - begin;
		else
			end = msg.cmd_param.find(" ", msg.cmd_param.find_first_of(" ") + 1) - begin;
		channel_name = msg.cmd_param.substr(begin, end);
	}
	error_message(msg, channel_name, ERR_NOSUCHCHANNEL);
}

static bool	is_channel_name_allowed(std::string chan_name)
{
	if (chan_name[0] != '#' && chan_name[0] != '&')
		return (false);
	if (chan_name.find_first_not_of(NICK_GOOD_CHARACTERS, 1) != std::string::npos)
		return (false);
	return (true);
}

void	Server::new_chan_member_sucess(Message &msg, std::string chan)
{
	Channel						channel_cpy(_channel_list.find(chan)->second);
	std::vector<std::string>	replies(1, RPL_JOIN);
	std::vector<std::string>	replace(1, chan);

	// replies.resize(4);
	// replace.resize(channel_cpy._clients.size() + 3);
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
		std::string	nick(client_list.find(it->first)->second._nickname);
		if (channel_cpy._is(it->second, channel_cpy.CHANOP) == true)
			nick.insert(0, "@");
		replace.push_back(nick);
	}
	replies.push_back(RPL_ENDOFNAMES);
	replace.push_back(chan);
	for (std::vector<std::string>::iterator it = replace.begin(); it < replace.end(); ++it)
	{
		std::cout << '[' << *it << "]-";
	}
	std::cout << std::endl;
	reply_message(msg, replies, replace);
}

// TODO : split this function = it is way to big for simple understanding
// TODO : implement the behaviour that happens upon sucessfully entering a channel
// TODO : implement argument "0" and its behaviour once PART is implemented
void	Server::join(Message &msg)
{
	std::string					tmp;
	std::vector<std::string>	channels;
	std::vector<std::string>	keys;

	std::cout << "join cmd start\n";
	if (msg.cmd_param.find_first_of(" ") != msg.cmd_param.find_last_of(" "))
		return (join_space_error_behavior(msg));
	if (msg.cmd_param.find(' ') != std::string::npos)
	{
		std::cout << "found key(s)" << std::endl;
		tmp = msg.cmd_param.substr(msg.cmd_param.find(' ') + 1, msg.cmd_param.size());
		keys = split_join_cmd(tmp);
		msg.cmd_param.erase(0, msg.cmd_param.find(' ') + 1);
	}
	channels = split_join_cmd(msg.cmd_param);
	if (msg.cmd_param.empty() == true)
	{
		channels.back().append(",");
		return (error_message(msg, channels.back(), ERR_NOSUCHCHANNEL));
	}
	if (keys.empty() == false)
	{
		std::cout << "should not pass here\n";
		std::vector<std::string>::iterator it_chan(channels.begin());
		for (std::vector<std::string>::iterator it_key = keys.begin();
			it_key != keys.end(); ++it_key)
		{
			if (it_chan == channels.end())
				break ;
			if (it_chan->empty() == true || is_channel_name_allowed(*it_chan) == false)
				return (error_message(msg, *it_chan, ERR_NOSUCHCHANNEL));
			if (_channel_list.find(*it_chan) == _channel_list.end())
			{
				Channel new_chan = Channel(msg.get_emmiter(), *it_chan);
				std::pair<std::string, Channel>	new_pair(*it_chan, new_chan);
				_channel_list.insert(new_pair);
			}
			else
			{
				Channel	*current_chan = &_channel_list.find(*it_chan)->second;
				if (current_chan->_key != *it_key && current_chan->_key.empty() == false)
					return (error_message(msg, *it_chan, ERR_BADCHANNELKEY));
				else if (current_chan->_is_invite_only == true)
				{
					if (current_chan->_is(current_chan->_clients.find(msg.get_emmiter())->second, current_chan->INVITED) == true)
						current_chan->add_new_member(msg.get_emmiter());
					else
					{
						if (current_chan->_is_invite_only == true)
							return (error_message(msg, *it_chan, ERR_INVITEONLYCHAN));
						else
							current_chan->add_new_member(msg.get_emmiter());
					}
				}
			}
			new_chan_member_sucess(msg, *it_chan);
			it_chan = channels.erase(channels.begin());
		}
	}
	while (channels.empty() == false)
	{
		std::cout << *channels.begin() << std::endl;
		if (channels.begin()->empty() == true
			|| is_channel_name_allowed(*channels.begin()) == false)
				return (error_message(msg, *channels.begin(), ERR_NOSUCHCHANNEL));
		if (_channel_list.find(*channels.begin()) == _channel_list.end())
		{
			Channel new_chan = Channel(msg.get_emmiter(), *channels.begin());
			std::pair<std::string, Channel>	new_pair(*channels.begin(), new_chan);
			_channel_list.insert(new_pair);
		}
		else
		{
			Channel	*current_chan = &_channel_list.find(*channels.begin())->second;
			if (current_chan->_is(current_chan->_clients.find(msg.get_emmiter())->second, current_chan->INVITED) == true)
				_channel_list.find(*channels.begin())->second.add_new_member(msg.get_emmiter());
			else
			{
				if (current_chan->_is_invite_only == true)
					return (error_message(msg, *channels.begin(), ERR_INVITEONLYCHAN));
				else
					_channel_list.find(*channels.begin())->second.add_new_member(msg.get_emmiter());
			}
		}
		new_chan_member_sucess(msg, *channels.begin());
		channels.erase(channels.begin());
	}
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
	msg.target.insert(msg.get_emmiter());
	msg.text.assign("PONG ");
	msg.text.append(SERVERNAME);
	msg.text.append(" ");
	msg.text.append(msg.cmd_param);
	msg.text.append("\r\n");
}
