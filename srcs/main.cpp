#include "server.hpp"

int	main(int ac, char **av)
{
	std::string	tmp;

	if (ac < 3 || ac > 3)
	{
		std::cerr << "Error: wrong number of arguments\n";
		std::cerr << "Usage: ./ircserv port password\n";
		return (1);
	}
	tmp.append(av[1]);
	if (tmp.find_first_not_of("0123456789") != std::string::npos)
	{
		std::cerr << "Error: port must be a number\n";
		std::cerr << "Usage: ./ircserv port password\n";
		return (1);
	}

	server	serv(av[1], av[2]);
	return (0);
}