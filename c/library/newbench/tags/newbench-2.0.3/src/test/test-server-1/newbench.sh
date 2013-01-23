#!/bin/bash

red_clr="\033[31m"
grn_clr="\033[32m"
end_clr="\033[0m"

pid=
if [ -e ./bin/daemon.pid ]; then
    pid=`cat ./bin/daemon.pid`
fi

if [ "$1" = "start" ]; then
    cd ./bin/ && ./newbench ../conf/bench.conf
    sleep 1
elif [ "$1" = "state" ]; then
    if [ "$pid" = '' ]; then
        printf "$red_clr%50s$end_clr\n" "ERROR: server is not started"
    else
        ps -o user,pid,stat,pcpu,pmem,cmd -s $pid
    fi
elif [ "$1" = "stop" ]; then
    if [ "$pid" = '' ]; then
        printf "$red_clr%50s$end_clr\n" "ERROR: server is not started"
    else
        pkill -s $pid
        sleep 1
    fi
else
    printf "Usage: %s start|state|stop\n" $0
fi
