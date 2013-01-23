#!/bin/sh

server_name='demo'
server_path='../newbench'
server_conf='./conf/bench.conf'
server_so='./lib'$server_name'.so'

logname=$LOGNAME

logid=`cat /etc/passwd | grep $logname | awk -F ':' '{print $3}'`

if [ "$1" = "state" ] ; then
    ps -eo user,pid,stat,pcpu,pmem,cmd | grep "$server_path\|^USER" | grep -v grep
elif [ "$1" = "start" ] ; then
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./
    $server_path $server_conf
elif [ "$1" = "stop" ] ; then
    ps -ef | grep -E "^$log_name|^$log_id" | grep "$server_path" | grep -v grep | awk '{print "echo kill " $2 " " $8 "; kill " $2;}' | sh
else
    echo 'usage: server.sh state|start|stop'
fi
