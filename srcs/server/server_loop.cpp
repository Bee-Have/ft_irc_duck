#include "server.hpp"

void	server_loop(server &serv)
{
	while (true)
	{
		fd_set read_fds = serv.get_read_fds();
		// fd_set write_fds = serv.get_read_fds();
		fd_set write_fds = serv.get_write_fds();

		std::cout << "about to select()" << std::endl;
		// select : detect anything on all sockets (server + clients) : new connections, messages, ect...
		// ! this current behavior is not good, the write_fds are useless
		// if (select(serv.get_max_fd() + 1, &read_fds, NULL, NULL, NULL) < 0)
		if (select(serv.get_max_fd() + 1, &read_fds, &write_fds, NULL, NULL) < 0)
		{
			std::cerr << "ERRNO:" << errno << ':' << strerror(errno) << std::endl;
			return ;
		}
		// new client connection
		if (FD_ISSET(serv.get_socket(), &read_fds))
			if (serv.add_client() == 1)
				return ;
		
		// read messages if there are any
		char		buffer[1024] = {0};
		for (std::map<int, client>::iterator it = serv.client_list.begin();
			it != serv.client_list.end(); ++it)
		{
			if (FD_ISSET(it->first, &read_fds))
			{
				int	valread = read(it->first, buffer, 512);
				if (valread <= 0)
				{
					serv.del_client(it->first);
					if (serv.client_list.empty() == true)
						break ;
					it = serv.client_list.begin();
				}
			}
		}
	}
}