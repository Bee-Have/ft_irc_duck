#pragma once

// OTHER
#define SERVERNAME "ircserv"
#define VERSION "2.31"
#define NICK_GOOD_CHARACTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-[]\\`_{}|"

// ERRORS
#define ERR_NONICKNAMEGIVEN " :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME " :Erroneus nickname\r\n"
#define ERR_NICKNAMEINUSE " :Nickname is already in use\r\n"
#define ERR_NEEDMOREPARAMS " :Not enough parameters\r\n"
#define ERR_ALREADYREGISTRED ":You may not reregister\r\n"
// CUSTOM
#define ERR_BADPASS " :Password does not match\r\n"
#define ERR_UNREGISTERED "Client must be registered to proceed\r\n"
#define ERR_CMDUNKNOWN " :Command is unknown\r\n"

// REPLIES USER
#define RPL_WELCOME "001 <client> :Welcome to the Internet Relay Network <nick>!\r\n"
#define RPL_YOURHOST "002 <client> :Your host is " SERVERNAME ", running version " VERSION "\r\n"
#define RPL_CREATED "003 <client> :This server was created <datetime>\r\n"
#define RPL_MYINFO "004 <client> " SERVERNAME " " VERSION " NO modes\r\n"
#define RPL_ISUPPORT "005 <client> 0 :are supported by this server\r\n"