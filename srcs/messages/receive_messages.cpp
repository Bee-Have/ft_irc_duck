#include "server.hpp"

/*
	Looks for an existing incomplete (whitout a '\r\n') from the client 'emmiter' and returns it's position
	If no such message is found, it returns -1
*/
static int	find_incomplete_msg(server &serv, client emmiter)
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

void	receive_message(server &serv, fd_set read_fds)
{
	char		buffer[1024] = {0};
	int			valread = 0;
	std::string	tmp;

	for (std::map<int, client>::iterator it = serv.client_list.begin();
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
					while (tmp.find("\n") != std::string::npos)
					{
						if (pos_msg != -1)
						{
							serv.msgs.at(pos_msg).text.append(tmp);
							// TODO : check && handle commands function call
						}
						else
						{
							new_msg.text.append(0, tmp.find("\n") + 1);
							serv.msgs.push_back(new_msg);
							// TODO : check && handle commands function call
						}
						tmp.assign(tmp.substr(tmp.find("\n") + 1, tmp.size()));
					}
			}
		}
	}
}