#include "ircserv.hpp"

/*
	This function will check if there are any empty messages.
	empty here means it has noone to send its message to
*/
static void	check_msgs_to_delete(server &serv)
{
	for (std::vector<message>::iterator it = serv.msgs.begin();
		it != serv.msgs.end(); ++it)
	{
		if (it->text.find("\r\n") != std::string::npos && it->target.empty() == true)
		{
			it = serv.msgs.erase(it);
			if (serv.msgs.empty() == true)
				return ;
		}
	}
}

/*
	This function messages to the target (msgs->target) if :
		-the target fd is present in write_fds (allowed list of fds given by select())
		-the message contains an end of message : '\r\n'
	After sending a message the target fd will be deleted from the message targets.
	If there are no targets in the message, the message will be deleted
	If there are no messages to send does nothing
*/
void	send_messages(server &serv, fd_set &write_fds)
{
	check_msgs_to_delete(serv);
	if (serv.msgs.empty() == true)
		return ;
	std::cout << "gonna send msg" << std::endl;
	for (std::vector<message>::iterator it_msg = serv.msgs.begin();
		it_msg != serv.msgs.end(); ++it_msg)
	{
		std::cout << "msg:" << it_msg->text << "|";
		for (std::set<int>::iterator it_fd = it_msg->target.begin();
			it_fd != it_msg->target.end(); ++it_fd)
		{
			if (FD_ISSET(*it_fd, &write_fds) != 0
				&& it_msg->text.find("\r\n") != std::string::npos)
			{
				std::cout << "about to send" << std::endl;
				send(*it_fd, it_msg->text.c_str(), it_msg->text.size(), 0);
				FD_CLR(*it_fd, &write_fds);
				it_msg->target.erase(it_fd);
				if (it_msg->target.empty() == true)
				{
					it_msg = serv.msgs.erase(it_msg) - 1;
					break ;
				}
				it_fd = it_msg->target.begin();
			}
			else
				++it_fd;
		}
	}
}
