#!/bin/sh
echo "ser shell exec ..."
if [ "$1" = "stop" ] ; then
#	ps -ef |grep ./ser | awk '{print "kill -15 " $2}'|sh
	ps -ef |grep "\<gf_del\>" | awk '{print "kill -9 " $2}'|sh
elif [ "$1" = "restart" ]; then
	killall -HUP ./gf_del
elif [ "$1" = "start" ]; then
#	 valgrind --leak-check=full ./gf_del ../etc/bench.conf ./libtest.so -s 4096
	./gf_del ../etc/bench.conf ./libgfser.so 
elif [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
	echo "daemon.sh start|stop|restart"
fi
ls ./gf_del ../etc/bench.conf ./libgfser.so
