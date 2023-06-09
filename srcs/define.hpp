#pragma once

//	OTHER
#define SERVERNAME "ircserv"
#define VERSION "2.31"
#define NICK_GOOD_CHARACTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-[]\\`_{}|"

//	SERVER
//		REPLIES
#define RPL_USAGE "000 " SERVERNAME " usage : ./ircserv <port> <password>\r\n"
//		ERRORS
#define ERR_BADSERVERPARAM "001 " SERVERNAME " :Wrong number of parameters\r\n"
#define ERR_SOCKCREATEFAIL "002 " SERVERNAME " :Server socket creation failed\r\n"
#define ERR_SOCKBINDFAIL "003 " SERVERNAME " :Binding server socket to PORT failed\r\n"
#define ERR_SOCKLISTENFAIL "004 " SERVERNAME " :Server socket listen failed\r\n"
#define ERR_PORTNOTANUMBER "005 " SERVERNAME " :<port> must be a numerical value\r\n"

//	CLIENT
//		REPLIES
#define RPL_WELCOME "001 <client> :Welcome to the Internet Relay Network <nick>!\r\n"
#define RPL_YOURHOST "002 <client> :Your host is " SERVERNAME ", running version " VERSION "\r\n"
#define RPL_CREATED "003 <client> :This server was created <datetime>\r\n"
#define RPL_MYINFO "004 <client> " SERVERNAME " " VERSION " NO modes\r\n"
#define RPL_ISUPPORT "005 <client> 0 :are supported by this server\r\n"
#define RPL_YOUREOPER "381 <client> :You are now an IRC operator\r\n"
//		ERRORS
#define ERR_NOSUCHNICK "401 <client> :<nickname> :No such nick/channel\r\n"
#define ERR_NOTEXTTOSEND "412 <client> :No text to send\r\n"
#define ERR_NONICKNAMEGIVEN "431 <client> :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME "432 <client> <nick> :Erroneus nickname\r\n"
#define ERR_NICKNAMEINUSE "433 <client> <nick> :Nickname is already in use\r\n"
#define ERR_NEEDMOREPARAMS "461 <client> <command> :Not enough parameters\r\n"
#define ERR_ALREADYREGISTRED "462 <client> :You may not reregister\r\n"
#define ERR_PASSWDMISMATCH "464 <client> :Password incorrect\r\n"
//		CUSTOM
#define ERR_CANNOTBECOMEOPER "908 <client> :Operator role already fullfiled\r\n"
#define ERR_NOSUCHOPER "909 <client> :<oper> :No such operator\r\n"
#define ERR_UNREGISTERED "910 <client> :Client must be registered to proceed\r\n"