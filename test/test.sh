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
	$SERVER_PATH/ircserv $PORT $SERVER_PASSWORD > $LOG_FOLDER/server.log 2>&1 &
	while [ ! -s $LOG_FOLDER/server.log ]; do
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
	killall -s SIGINT ircserv
	if [ $? -ne 0 ]; then
		echo "Server failed to stop"
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
	while [ $CLIENT_NUMBER -lt $1 ]; do
		nc $IP_ADDRESS $PORT < <(connectionInfo) -i 1 > $LOG_FOLDER/$CLIENT_NAME.log 2>&1 &
		if [ $? -ne 0 ]; then
			echo "Connection failed"
			stopServer 1
		fi
		echo "$CLIENT_NAME connected to $IP_ADDRESS:$PORT"
		CLIENT_NUMBER=$((CLIENT_NUMBER+1))
		CLIENT_NAME="client-$CLIENT_NUMBER"
	done
}

connectTester ()
{
	CLIENT_NAME="tester"
	echo "tester connecting to $IP_ADDRESS:$PORT"
	nc -NC $IP_ADDRESS $PORT < <(./test/tester_input.sh)
	if [ $? -ne 0 ]; then
		echo "Connection failed"
		stopServer 1
	fi
}

run ()
{
	createLogFolder
	startServer
	connectFake 3
	connectTester
	stopServer 0
}

run