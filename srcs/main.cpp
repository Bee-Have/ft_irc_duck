#include "ircserv.hpp"
#include "sstream"

// void	setup_commands(Server &serv)
// {
// 	commands["PASS"] = &Server::pass;
// 	commands["NICK"] = &Server::nick;
// 	commands["USER"] = &Server::user;
// 	commands["OPER"] = &Server::oper;
// 	commands["PRIVMSG"] = &Server::privmsg;
// 	commands["JOIN"] = &Server::join;
// 	commands["PING"] = &Server::ping;
// }

int	main(int ac, char **av)
{
	std::stringstream	ss;
	std::string			tmp;
	int					port;

	if (ac != 3)
	{
		std::cerr << ERR_BADSERVERPARAM << RPL_USAGE;
		return (1);
	}
	tmp.append(av[1]);
	if (tmp.find_first_not_of("0123456789") != std::string::npos)
	{
		std::cerr << ERR_PORTNOTANUMBER << RPL_USAGE;
		return (1);
	}

	ss << tmp;
	ss >> port;

	Server	serv(port, av[2]);
	server_loop(serv);
	return (0);
}