#!/bin/bash

echo -en "PASS $SERVER_PASSWORD\n"
echo -en "NICK $CLIENT_NAME\n"
echo -en "USER $CLIENT_NAME 0 * :$CLIENT_NAME\n"

while [ "$USER_INPUT" != "quit" ]; do
	read USER_INPUT
	echo -en "$USER_INPUT\n"
done
