#pragma once

//	OTHER
#define SERVERNAME "ircserv"
#define VERSION "2.31"
#define NICK_GOOD_CHARACTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-[]\\`_{}|"
#define KICK_DEFAULT_COMMENT "No comment has been provided"
#define QUIT_MANUAL "Manual disconnection through commands"
//	SERVER
//		REPLIES
#define RPL_USAGE "000 " SERVERNAME " usage : ./ircserv <port> <password>\r\n"
//		ERRORS
#define ERR_BADSERVERPARAM "001 " SERVERNAME " :Wrong number of parameters\r\n"
#define ERR_SOCKCREATEFAIL "002 " SERVERNAME " :Server socket creation failed\r\n"
#define ERR_SOCKBINDFAIL "003 " SERVERNAME " :Binding server socket to PORT failed\r\n"
#define ERR_SOCKLISTENFAIL "004 " SERVERNAME " :Server socket listen failed\r\n"
#define ERR_PORTNOTANUMBER "005 " SERVERNAME " :<port> must be a numerical value\r\n"
#define ERR_BADCOMMANDTYPE "006 " SERVERNAME " :Command type must inherit ICommand\r\n"
#define ERR_NONUNIQUECOMMAND "007 " SERVERNAME " :Commands must be unique\r\n"

//	CLIENT
//		REPLIES
#define RPL_WELCOME "001 <client> :Welcome to the Internet Relay Network <nick>!\r\n"
#define RPL_YOURHOST "002 <client> :Your host is " SERVERNAME ", running version " VERSION "\r\n"
#define RPL_CREATED "003 <client> :This server was created <datetime>\r\n"
#define RPL_MYINFO "004 <client> " SERVERNAME " " VERSION " NO modes\r\n"
#define RPL_ISUPPORT "005 <client> 0 :are supported by this server\r\n"
#define RPL_TOPIC "332 <client> <channel> :<topic>\r\n"
#define RPL_TOPICWHOTIME "333 <client> <channel> <nick> <setat>\r\n"
#define RPL_INVITING "341 <client> <nick> <channel>\r\n"
#define RPL_NAMREPLY "353 <client> <symbol> <channel> :<nick>{ <nick>}\r\n"
#define RPL_ENDOFNAMES "366 <client> <channel> :End of /NAMES list\r\n"
#define RPL_YOUREOPER "381 <client> :You are now an IRC operator\r\n"
//		ERRORS
#define ERR_NOSUCHNICK "401 <client> :<nickname> :No such nick/channel\r\n"
#define ERR_NOSUCHCHANNEL "403 <client> <channel> :No such channel\r\n"
#define ERR_NOTEXTTOSEND "412 <client> :No text to send\r\n"
#define ERR_NONICKNAMEGIVEN "431 <client> :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME "432 <client> <nick> :Erroneus nickname\r\n"
#define ERR_NICKNAMEINUSE "433 <client> <nick> :Nickname is already in use\r\n"
#define ERR_USERNOTINCHANNEL "441 <client> <nick> <channel> :They aren't on that channel\r\n"
#define ERR_NOTONCHANNEL "442 <client> <channel> :You're not on that channel\r\n"
#define ERR_USERONCHANNEL "443 <client> <nick> <channel> :is already on channel\r\n"
#define ERR_NEEDMOREPARAMS "461 <client> <command> :Not enough parameters\r\n"
#define ERR_ALREADYREGISTRED "462 <client> :You may not reregister\r\n"
#define ERR_PASSWDMISMATCH "464 <client> :Password incorrect\r\n"
#define ERR_INVITEONLYCHAN "473 <client> <channel> :Cannot join channel (+i)\r\n"
#define ERR_BADCHANNELKEY "475 <client> <channel> :Cannot join channel (+k)\r\n"
#define ERR_CHANOPRIVSNEEDED "482 <client> <channel> :You're not channel operator\r\n"
//		CUSTOM
// 			REPLIES
#define RPL_JOIN ":<client> JOIN <channel>\r\n"
#define RPL_PART ":<client> PART <channel> :<reason>\r\n"
#define RPL_INVITE ":<client> INVITE <nick> <channel>\r\n"
#define RPL_KICK ":<client> KICK <channel> <nick> :<comment>\r\n"
#define RPL_ERROR ":<server> ERROR :<comment>\r\n"
#define RPL_QUIT ":<client> QUIT <comment>\r\n"
#define RPL_CLIENTLEFT "908 <client> :Client <nick> just left " SERVERNAME ", bye bye\r\n"
// 			ERRRORS
#define ERR_CANNOTBECOMEOPER "920 <client> :Operator role already fullfiled\r\n"
#define ERR_NOSUCHOPER "921 <client> :<oper> :No such operator\r\n"
#define ERR_UNREGISTERED "922 <client> :Client must be registered to proceed\r\n"