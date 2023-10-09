#include "ircserv.hpp"

static void	found_new_line(Server &serv, Client emitter, std::string text, int pos_msg)
{
	Message	new_msg(emitter);

	while (text.find("\n") != std::string::npos)
	{
		if (pos_msg != -1)
		{
			serv.msgs.at(pos_msg).text.append(text);

			check_for_cmds(serv, serv.msgs.at(pos_msg));
			if (serv.msgs.at(pos_msg).text.empty() == true || serv.msgs.at(pos_msg).target.empty() == true)
				serv.msgs.erase(serv.msgs.begin() + pos_msg);
		}
		else
		{
			new_msg.text.assign(text.substr(0, text.find("\n") + 1));
			check_for_cmds(serv, new_msg);
			if (new_msg.text.empty() == false && new_msg.target.empty() == false)
				serv.msgs.push_back(new_msg);
		}
		text = text.substr(text.find("\n") + 1, text.size());
	}
}

/**
 * @brief looks for incomplete messages from a specific client. incomplete means there is no tailing "\r\n"
 * 
 * @param serv the server, which contains all the messages
 * @param emitter the specific client to check incomplete messages from
 * @return either the position of an incomplete message in Server::msgs
 * or -1 if no incomplete message is found
 */
static int	find_incomplete_msg(Server &serv, Client emitter)
{
	for (std::vector<Message>::iterator it = serv.msgs.begin(); it != serv.msgs.end(); ++it)
	{
		if (it->get_emitter() == emitter.get_socket()
			&& it->text.find("\r\n") == std::string::npos)
		{
			return (it - serv.msgs.begin());
		}
	}
	return (-1);
}

static void	found_text(Server &serv, Client emitter, std::string text)
{
	Message	new_msg(emitter);
	int		pos_msg = find_incomplete_msg(serv, emitter);

	std::cout << "NEW MSG:" << text << '|' << std::endl;
	if (text.find("\n") == std::string::npos)
	{
		if (pos_msg != -1)
			serv.msgs.at(pos_msg).text.append(text);
		else
		{
			new_msg.text.append(text);
			serv.msgs.push_back(new_msg);
		}
	}
	else
		found_new_line(serv, emitter, text, pos_msg);
}

/**
 * @brief register any new messages from each of the clients in client_list
 * 
 * @param serv the server, which contains all the clients and messages
 * @param read_fds the fds changed by "select()". Only the fds we can read on will be in read_fds
 */
void	receive_messages(Server &serv, fd_set read_fds)
{
	char		buffer[1024] = {0};
	int			valread = 0;

	for (std::map<int, Client>::iterator it = serv.client_list.begin();
		it != serv.client_list.end(); ++it)
	{
		if (FD_ISSET(it->first, &read_fds) == 0)
			continue ;
		valread = read(it->first, buffer, 512);
		if (valread <= 0)
		{
			Message	new_msg(it->second);
			Quit*	quit_cmd = dynamic_cast<Quit *>(serv.commands["QUIT"]);

			if (quit_cmd != NULL)
				quit_cmd->manual_quit = false;
			serv.commands["QUIT"]->execute(new_msg);
			if (quit_cmd != NULL || serv.client_list.empty() == true)
				break ;
		}
		else
			found_text(serv, it->second, buffer);
	}
}
