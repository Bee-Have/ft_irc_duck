#include "ircserv.hpp"

/**
 * @brief looks for incomplete messages from a specific client. incomplete means there is no tailing "\r\n"
 * 
 * @param serv the server, which contains all the messages
 * @param emmiter the specific client to check incomplete messages from
 * @return either the position of an incomplete message in server::msgs
 * or -1 if no incomplete message is found
 */
static int	find_incomplete_msg(server &serv, server::client emmiter)
{
	for (std::vector<message>::iterator it = serv.msgs.begin(); it != serv.msgs.end(); ++it)
	{
		if (it->get_emmiter() == emmiter.get_socket()
			&& it->text.find("\r\n") == std::string::npos)
		{
			return (it - serv.msgs.begin());
		}
	}
	return (-1);
}

/**
 * @brief register any new messages from each of the clients in server::client_list
 * 
 * @param serv the server, which contains all the clients and messages
 * @param read_fds the fds changed by "select()". Only the fds we can read on will be in read_fds
 */
void	receive_messages(server &serv, fd_set read_fds)
{
	char		buffer[1024] = {0};
	int			valread = 0;
	std::string	tmp;

	for (std::map<int, server::client>::iterator it = serv.client_list.begin();
		it != serv.client_list.end(); ++it)
	{
		if (FD_ISSET(it->first, &read_fds) != 0)
		{
			valread = read(it->first, buffer, 512);
			if (valread <= 0)
			{
				serv.del_client(it->first);
				if (serv.client_list.empty() == true)
					break ;
				it = serv.client_list.begin();
			}
			else
			{
				message	new_msg(it->second);
				int		pos_msg = find_incomplete_msg(serv, it->second);

				tmp.assign(buffer);
				std::cout << "NEW MSG:" << tmp << '|' << std::endl;
				if (tmp.find("\n") == std::string::npos)
				{
					if (pos_msg != -1)
						serv.msgs.at(pos_msg).text.append(tmp);
					else
					{
						new_msg.text.append(tmp);
						serv.msgs.push_back(new_msg);
					}
				}
				else
				{
					while (tmp.find("\n") != std::string::npos)
					{
						if (pos_msg != -1)
						{
							serv.msgs.at(pos_msg).text.append(tmp);

							check_for_cmds(serv, serv.msgs.at(pos_msg));
							if (serv.msgs.at(pos_msg).text.empty() == true || new_msg.target.empty() == true)
								serv.msgs.erase(serv.msgs.begin() + pos_msg);
						}
						else
						{
							new_msg.text.assign(tmp.substr(0, tmp.find("\n") + 1));
							// std::cout << "string::msg:" << new_msg.text << '|' << std::endl;
							check_for_cmds(serv, new_msg);
							if (new_msg.text.empty() == false && new_msg.target.empty() == false)
							{
								// std::cout << "SAVING MSG TO SEND" << std::endl;
								serv.msgs.push_back(new_msg);
							}
						}
						tmp = tmp.substr(tmp.find("\n") + 1, tmp.size());
						// std::cout << "string::tmp:" << tmp << '|' << std::endl;
					}
				}
			}
		}
	}
}