#!/bin/bash
#Script-Name: meassrv
if [ $# -eq 0 ] 
	then
	echo "Missing parameter, please try again with ONE parameter"
	exit 0 

elif [ $# -gt 1 ] 
	then
	echo "To many arguments, please reduce to only ONE parameter"
	exit 1

elif [ $# -eq 1 ] 
	then

	case "$1" in
		start)
		cd /home/antonia/Schreibtisch
		echo "Server will be started. He'll live." 
		gcc -Wall -o dht11srv dht11srv.c -lwiringPi -lpthread

		nohup ./dht11srv.exe &

		sleep 5s
		exit 2
		;;

		restart)
		echo "The server will be restarted. I'll be back!"
		killall ./dht11srv.exe
		sleep 2s
		gcc -Wall -o dht11srv dht11srv.c -lwiringPi -lpthread
		nohup ./dht11srv.exe &
		exit 3
		;;

		stop)
		echo "The termination has started. Hasta la vista, baby!"
		killall ./dht11srv.exe
		exit 4
		;; 
		
		status)
		echo "Information about your status will be provided"
		netstat -u -o 
		;;
		
		help | -h | h | --h | -o | --o)
		echo "following parameter are possible:"
		echo "-help/-h/-o/h/--h for help"
		echo "start		the programm dht11srv gets started"
		echo "restart		the program dht11srv gets restarted" 
		echo "stop		the programm dht11srv gets terminated"
		echo "status		shows you the status of the server (running, terminated ...)"
		exit 5
		;;

		*)
		echo "wrong use! please choose between: start, restart, stop, status"
		echo "if you need help use one of the followng parameter help, -h, -o, h, --o, --h"
		exit 6
		;;
	
esac
fi
