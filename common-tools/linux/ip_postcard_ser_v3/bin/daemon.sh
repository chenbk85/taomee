#!/bin/sh
echo "ser shell exec ..."
if [ "$1" = "stop" ] ; then
    ps -ef |grep "\<ippostcardser\>" | awk '{print "kill -9 " $2}'|sh
elif [ "$1" = "start" ]; then
    rm -f ../log/*
    ./ippostcardser ../etc/bench.conf
elif [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    echo "daemon.sh start|stop|restart"
fi
