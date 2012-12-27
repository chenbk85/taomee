#!/bin/sh
if [ "$1" = "stop" ] ; then
	ps -ef |grep ./comser | awk '{print "kill -9 " $2}'|sh
elif [ "$1" = "restart" ]; then
	killall -HUP comser
elif [ "$1" = "start" ]; then
	./comser ../etc/bench.conf ./libcom.so 
elif [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
	echo "daemon.sh start|stop|restart"
fi
