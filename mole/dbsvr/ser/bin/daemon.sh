#!/bin/sh

regex="\.\/ser";

echo "ser shell exec ..."

if [ "$1" = "stop" ] ; then
	ps -ef | grep $regex | awk '{print "kill -9 " $2}' | sh
elif [ "$1" = "restart" ]; then
	ps -ef |grep $regex | awk '{print "kill -9 " $2}' | sh
	./ser ../etc/bench.conf ./libser.so
#./r53ser
elif [ "$1" = "start" ]; then
	./ser ../etc/bench.conf ./libser.so
#./r53ser	
elif [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
	echo "daemon.sh start|stop|restart"
fi

