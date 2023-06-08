#include "ircserv.hpp"

void	server_loop(server &serv)
{
	while (true)
	{
		fd_set read_fds = serv.get_read_fds();
		fd_set write_fds = serv.get_write_fds();

		std::cout << "about to select()" << std::endl;
		// select : detect anything on all sockets (server + clients) : new connections, messages, ect...
		// ! this current behavior is not good, the write_fds are useless
		if (select(serv.get_max_fd() + 1, &read_fds, &write_fds, NULL, NULL) < 0)
		{
			std::cerr << "ERRNO:" << errno << ':' << strerror(errno) << std::endl;
			return ;
		}
		// new client connection
		if (FD_ISSET(serv.get_socket(), &read_fds))
			if (serv.add_client() == 1)
				return ;

		// send messages if there are any
		send_messages(serv, write_fds);

		// read messages if there are any
		receive_messages(serv, read_fds);
	}
}