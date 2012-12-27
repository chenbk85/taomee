#!/bin/sh
echo "ser shell exec ..."
if [ "$1" = "stop" ] ; then
#	ps -ef |grep ./ser | awk '{print "kill -15 " $2}'|sh
	ps -ef |grep "\<gfser_plus\>" | awk '{print "kill -9 " $2}'|sh
elif [ "$1" = "restart" ]; then
	killall -HUP ./gfser_plus
elif [ "$1" = "start" ]; then
#	 valgrind --leak-check=full ./gfser_test ../etc/bench_test.conf ./libtest.so -s 4096
	./gfser_plus ../etc/bench_test.conf ./libgfser.so 
elif [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
	echo "daemon.sh start|stop|restart"
fi
ls ./gfser_plus ../etc/bench_test.conf ./libgfser.so
