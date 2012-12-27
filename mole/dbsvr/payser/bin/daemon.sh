#!/bin/sh
if [ "$1" = "stop" ] ; then
#	ps -ef |grep ./payser | awk '{print "kill -15 " $2}'|sh
	ps -ef |grep ./payser | awk '{print "kill -9 " $2}'|sh
elif [ "$1" = "restart" ]; then
	killall -HUP ./payser
elif [ "$1" = "start" ]; then
#	 valgrind --leak-check=full ./payser ../etc/bench.conf ./libtest.so -s 4096
	./payser ../etc/bench.conf ./libtest.so -s 4096
elif [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
	echo "daemon.sh start|stop|restart"
fi
