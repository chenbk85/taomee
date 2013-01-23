#!/bin/sh

# 需要管理的服务器可执行文件的名称，根据实际应用修改
server_name='stubserver'
server_path='../../newbench'
server_conf='./conf/bench.conf'
server_so='./bin/lib'$server_name'.so'

# 当前登录的用户名
logname=$LOGNAME

# 当前登录的用户 ID，如果用户名长度大于 7，则需要使用用户 ID 来搜索进程
logid=`cat /etc/passwd | grep $logname | awk -F ':' '{print $3}'`

if [ "$1" = "state" ] ; then
    ps -eo user,pid,stat,pcpu,pmem,cmd | grep "$server_path\|^USER" | grep -v grep
elif [ "$1" = "start" ] ; then
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../../lib/64/:./lib:../../pkg-filter/so/
    $server_path $server_conf
elif [ "$1" = "gdb" ] ; then
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../../lib/64/:./lib:../../pkg-filter/so/
    gdb $server_path
elif [ "$1" = "stop" ] ; then
    ps -ef | grep -E "^$log_name|^$log_id" | grep "$server_path" | grep -v grep | awk '{print "echo kill " $2 " " $8 "; kill " $2;}' | sh
else
    echo 'usage: server.sh state|start|stop'
fi
