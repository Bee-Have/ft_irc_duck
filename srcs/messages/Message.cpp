#include "Message.hpp"
#include "Client.hpp"

/**
 * @brief Construct a new Message::Message object
 * @note this construction should never be called by anyone.
 * Message should always be instanciated with am _emitter
 */
Message::Message(void)
{}

Message::Message(const Message &cpy):
	_emitter(cpy._emitter), _username(cpy._username),
	emitter_nick(cpy.emitter_nick), host(cpy.host),
	target(cpy.target), text(cpy.text), cmd(cpy.cmd), cmd_param(cpy.cmd_param)
{
	if (cpy._username.empty() == true)
		_username = "*";
}

Message::Message(const Client &emitter):
	_emitter(emitter.get_socket()), _username(emitter.get_username()),
	emitter_nick(emitter.nickname),
	host(emitter.host)
{
	if (emitter.get_username() == "")
		_username = "*";
}

Message::~Message(void)
{
	target.clear();
}

Message	&Message::operator=(const Message &assign)
{
	if (this != &assign)
	{
		_emitter = assign._emitter;
		_username = assign._username;
		emitter_nick.assign(assign.emitter_nick);
		host.assign(assign.host);
		target.clear();
		target.insert(assign.target.begin(), assign.target.end());
		text.assign(assign.text);
		cmd.assign(assign.cmd);
		cmd_param.assign(assign.cmd_param);
	}
	return (*this);
}

int	Message::get_emitter(void) const
{
	return (_emitter);
}

/**
 * @brief replace "<>" by the given value
 * 
 * @param msg the message to alter
 * @param replace the text to replace the "<>" with
 */
void	Message::replace_rpl_err_text(std::string replace)
{
	int	begin = 0;
	int	end = 0;

	begin = text.find('<');
	end = (text.find('>') + 1) - begin;
	text.replace(begin, end, replace);
}

void	Message::replace_prefix(std::string text, int socket)
{
	bool	is_reply_custom = false;
	if (text[9] == '!' && text[16] == '@')
		is_reply_custom = true;
	if (_emitter == socket)
		replace_rpl_err_text(SERVERNAME);
	else
		replace_rpl_err_text(emitter_nick);
	if (is_reply_custom == true)
	{
		replace_rpl_err_text(_username);
		replace_rpl_err_text(host);
	}
}

/**
 * @brief setup a "Message"(class) text with the reply given. It will also 
 * replace the "<>" with the value given
 * @note the first "<>" of the RPL will ALWAYS be the client nickname sending the RPL
 * 
 * @param msg the message to setup
 * @param replies the replies to be concatenated
 * @param replace the values to replace the "<>"
 */
void	Message::reply_format(std::string reply, std::string replace, int socket)
{
	// ! do we really need this ? it feels stale
	target.clear();
	target.insert(_emitter);

	text.clear();
	text = reply;
	replace_prefix(text, socket);
	if (text.find('<') != std::string::npos)
		replace_rpl_err_text(replace);
}

/**
 * @brief setup a "Message"(class) text with the replies given. It will also 
 * replace all the necessary "<>" with the apropriate values given
 * @note the first "<>" of each RPL will ALWAYS be the client nickname sending the RPL
 * 
 * @param msg the message to setup
 * @param replies the replies to be concatenated
 * @param replace the values to replace the "<>"
 */
void	Message::reply_format(std::vector<std::string> &replies, std::vector<std::string> &replace)
{
	std::vector<std::string>::iterator it_replace = replace.begin();

	target.clear();
	target.insert(_emitter);

	text.clear();
	for (std::vector<std::string>::iterator it = replies.begin(); it != replies.end(); ++it)
	{
		text.append(*it);
		replace_prefix(*it, -1);
		while (text.find('<') != std::string::npos)
		{
			replace_rpl_err_text(*it_replace);
			++it_replace;
		}
	}
}

/**
 * @brief replace curly brackets "{}" in RPL or ERR to the appropriate values
 * @note in define.hpp you will sometime see "{}". These means there is a 
 * variadic number of elements to be added between them.
 * This function does just that.
 * 
 * @param reply the string to alter with the cruly brackets "{}"
 * @param replace_count the number of times you have to insert the appropriate 
 * value.
 */
void	Message::reply_replace_curly_brackets(std::string &reply, int replace_count)
{
	std::string	replace;
	int		start = reply.find('{');

	replace = reply.substr(start + 1, reply.find('}') - start - 1);
	reply.erase(start + 1, replace.size());
	for (int i = 0; i < replace_count - 1; ++i)
	{
		reply.insert(start + 1, replace);
	}
	reply.erase(start, 1);
	reply.erase(reply.find('}'), 1);
}
