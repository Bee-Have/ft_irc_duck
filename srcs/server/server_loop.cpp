#include "ircserv.hpp"
#include "Logger.hpp"

#include <csignal>

bool g_run = true;

void handle_exit(int p_signal)
{
	(void)p_signal;
	g_run = false;
}

/**
 * @brief general loop of the server.
 * Everything passes through here as this is the main logic of the server
 * 
 * @param serv serv is setup in main and should not pose any problems
 */
void	server_loop(Server &serv)
{
	signal(SIGINT, handle_exit);
	while (g_run == true)
	{
		fd_set read_fds = serv.get_read_fds();
		fd_set write_fds = serv.get_write_fds();

		// select : detect anything on all sockets (server + clients) : new connections, messages, ect...
		if (select(serv.get_max_fd() + 1, &read_fds, &write_fds, NULL, NULL) < 0)
		{
			if (errno != EINTR) ///< EINTR is not an error, it just means that a signal was caught
				Logger(error_type, error_lvl) << "ERRNO:" << errno << ':' << strerror(errno);
			return ; ///< I am not sure that every select error should be fatal
		}
		// new client connection
		if (FD_ISSET(serv.socket_id, &read_fds))
			serv.add_client();

		// send messages if there are any
		send_messages(serv, write_fds);

		// read messages if there are any
		receive_messages(serv, read_fds);
	}
}