#include "ircserv.hpp"

/**
 * @brief checks for any empty messages. By empty means msg.target is empty AND there is no tailing "\r\n"
 * 
 * @param serv the server, which has all the messages
 */
static void	check_msgs_to_delete(Server &serv)
{
	for (std::vector<Message>::iterator it = serv.msgs.begin();
		it != serv.msgs.end(); ++it)
	{
		std::cout  << "check[" << it->text << ']' << std::endl;
		if (it->text.find("\r\n") != std::string::npos
			&& it->text.find("\r\n") + 2 == it->text.size()
			&& it->target.empty() == true)
		{
			it = serv.msgs.erase(it);
			if (serv.msgs.empty() == true)
				return ;
		}
	}
}
/**
 * @brief merge messages if they have the same target.
 * @note current behavior only merges messages with identical targets. This behavior should be tweaked later
 * 
 * @param serv the server, wich has all the messages
 */
static void	merge_msgs(Server &serv)
{
	if (serv.msgs.size() <= 1)
		return ;
	for (std::vector<Message>::iterator it_a = serv.msgs.begin();
		it_a != serv.msgs.end(); ++it_a)
	{
		for (std::vector<Message>::iterator it_b = it_a + 1;
			it_b != serv.msgs.end(); ++it_b)
		{
			if (it_a->target == it_b->target)
			{
				it_a->text.append(it_b->text);
				it_a = serv.msgs.erase(it_b) - 2;
				break ;
			}
		}
	}
}

/**
 * @brief temprorary debug function to print all messages
 */
// static void	print_msgs(Server &serv)
// {
// 	std::cout << "START PRINT MSGS:";
// 	for (std::vector<Message>::iterator it = serv.msgs.begin(); it != serv.msgs.end(); ++it)
// 	{
// 		std::cout << "it:" << it->text;
// 	}
// }

/**
 * @brief This function messages to the target (Server::msgs->target) if :
 * 1.the target fd is present in write_fds (allowed list of fds given by select()).
 * 2.the message contains an end of message : '\r\n'
 * @note After sending a message the target fd will be deleted from the message targets.
 * If the message target are empty, delete the message.
 * If there are no messages to send does nothing
 * 
 * @param serv the server, which has all the messages
 * @param write_fds the fds after "select()" which are the fds we can "send()" messages to
 */
void	send_messages(Server &serv, fd_set &write_fds)
{
	check_msgs_to_delete(serv);
	if (serv.msgs.empty() == true)
		return ;
	merge_msgs(serv);
	std::cout << "gonna send msg" << std::endl;
	for (std::vector<Message>::iterator it_msg = serv.msgs.begin();
		it_msg != serv.msgs.end(); ++it_msg)
	{
		std::cout << "msg:" << it_msg->text << "|";
		for (std::set<int>::iterator it_fd = it_msg->target.begin();
			it_fd != it_msg->target.end(); ++it_fd)
		{
			std::cout << "TARGET" << std::endl;
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
		}
	}
}
