#!/bin/sh
echo "ser shell exec ..."
if [ "$1" = "stop" ] ; then
#	ps -ef |grep ./ser | awk '{print "kill -15 " $2}'|sh
	ps -ef |grep "\<gfCacheSer\>" | awk '{print "kill -9 " $2}'|sh
elif [ "$1" = "restart" ]; then
	killall -HUP ./gfCacheSer
elif [ "$1" = "start" ]; then
#	 valgrind --leak-check=full ./gfCacheSer ../etc/bench.conf ./libtest.so -s 4096
	./gfCacheSer ../conf/bench.conf ./libcache_serv.so 
elif [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
	echo "daemon.sh start|stop|restart"
fi
ls ./gfCacheSer ../conf/bench.conf ./libcache_serv.so
