#!/bin/sh
if [ "$1" = "stop" ] ; then
	killall -TERM ServerBench
elif [ "$1" = "restart" ]; then
	killall -HUP ServerBench
elif [ "$1" = "start" ]; then
	../src/ServerBench ../conf/bench.conf ../flashpolicy/libflashpolicy.so -s 4096
elif [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
	echo "daemon.sh start|stop|restart"
fi
