#!/bin/sh
echo "ser shell exec ..."
if [ "$1" = "stop" ] ; then
    ps -ef |grep "\<postcardser\>" | awk '{print "kill -9 " $2}'|sh
elif [ "$1" = "restart" ]; then
    killall -HUP ./postcardser
elif [ "$1" = "start" ]; then
    ./postcardser ../etc/bench.conf
elif [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    echo "daemon.sh start|stop|restart"
fi
