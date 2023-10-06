#include "ircserv.hpp"
// #include "Server.hpp"
#include <sstream>

void	setup_commands(Server& serv)
{
	serv.register_command<Pass>("PASS");
	serv.register_command<Nick>("NICK");
	serv.register_command<User>("USER");
	serv.register_command<Privmsg>("PRIVMSG");
	serv.register_command<Oper>("OPER");
	serv.register_command<Join>("JOIN");
	serv.register_command<Part>("PART");
	serv.register_command<Invite>("INVITE");
	serv.register_command<Topic>("TOPIC");
	serv.register_command<Kick>("KICK");
	serv.register_command<Ping>("PING");
	serv.register_command<Mode>("MODE");
}

int	main(int ac, char** av)
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

	Server*	serv;
	try
	{
		serv = new Server(port, av[2]);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	setup_commands(*serv);
	server_loop(*serv);
	delete serv;

	return (0);
}