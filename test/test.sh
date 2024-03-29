#!/bin/bash

source ./test/script_variables.sh

createLogFolder ()
{
	if [ ! -d $LOG_FOLDER ]; then
		mkdir $LOG_FOLDER
	fi
}

startServer ()
{
	echo "Starting server"
	rm -f server.log
	$SERVER_PATH/ircserv $PORT $SERVER_PASSWORD &
	while [ ! -s server.log ]; do
		sleep 1
	done
	if ! ps | grep -q " $! "; then
		echo "Server failed to start"
		exit 1
	fi
	echo "Server started"
}

stopServer ()
{
	echo "Stopping server"
	killall nc
	killall tail
	pkill -f --signal SIGINT ircserv
	kill_result=$?
	if [ $kill_result -ne 0 ]; then
		echo "Server failed to stop with result $kill_result"
		exit 1
	fi
	echo "Server stopped"
	exit $1
}

connectionInfo ()
{
	echo -en "PASS $SERVER_PASSWORD\n"
	echo -en "NICK $CLIENT_NAME\n"
	echo -en "USER $CLIENT_NAME 0 * :$CLIENT_NAME\n"
	echo -en "JOIN #test\n"
}

connectFake ()
{
	while [ $CLIENT_ID -lt $1 ]; do
		connectionInfo > ./logs/.client-$CLIENT_ID-input.log
		tail -f ./logs/.client-$CLIENT_ID-input.log | nc -C $IP_ADDRESS $PORT > $LOG_FOLDER/$CLIENT_NAME.log 2>&1 &
		if [ $? -ne 0 ]; then
			echo "Connection failed"
			stopServer 1
		fi
		NC_PID[$CLIENT_ID]=$!
		echo "$CLIENT_NAME connected to $IP_ADDRESS:$PORT"
		CLIENT_ID=$((CLIENT_ID+1))
		CLIENT_NAME="client-$CLIENT_ID"
	done
}

attachSTDIN ()
{
	if [ -z "$1" ]; then
		echo "No client number specified"
		return
	fi
	if [ $1 -ge $CLIENT_NUMBER ] || [ $1 -lt 0 ]; then
		echo "Client number out of range"
		return
	fi
	local id=$1
	if [ "$id" -eq "-0" ]; then
		id="0"
	fi
	echo "Attaching stdin to (client-$id)"
	CURRENT_INPUT=./logs/.client-$id-input.log
}

run ()
{
	createLogFolder
	startServer
	connectFake $CLIENT_NUMBER
	attachSTDIN 0
	while [ "$USER_INPUT" != "t-quit" ]; do
		read USER_INPUT
		if [ "$USER_INPUT" == "t-attach" ]; then
			printf "Enter client number[0-%s]: " $((CLIENT_NUMBER-1))
			read USER_INPUT
			attachSTDIN $USER_INPUT
			continue
		fi
		echo -en "$USER_INPUT\n" >> $CURRENT_INPUT
	done
	stopServer 0
}

run