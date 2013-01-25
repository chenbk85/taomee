#!/bin/sh
cd `dirname $0`
echo "ser shell exec ..."
if [ "$1" = "stop" ] ; then
	ps -ef |grep "\<pubser\>" | awk '{print "kill -9 " $2}'|sh
elif [ "$1" = "restart" ]; then
	ps -ef |grep "\<pubser\>" | awk '{print "kill -9 " $2}'|sh
	./pubser ./bench.lua ./libpubser.so
elif [ "$1" = "start" ]; then
	./pubser ./bench.lua ./libpubser.so 
elif [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
	echo "daemon.sh start|stop|restart"
fi
