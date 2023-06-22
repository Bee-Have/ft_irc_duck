#pragma once

// OTHER
#define HOST "ircserv"
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

// REPLIES
#define RPL_WELCOME "Welcome to the Internet Relay Network nick!user@host\r\n"