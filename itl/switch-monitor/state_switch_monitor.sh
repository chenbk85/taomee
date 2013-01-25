#!/bin/bash

red_clr="\033[31m"
grn_clr="\033[32m"
end_clr="\033[0m"

#启动用户当前所在的绝对路径
cwd=`pwd`

#本shell所在的目录
path=`dirname $0`

cd $cwd && cd $path && cd ./bin 


if ! test -e ./daemon.pid; then
	printf "$red_clr%50s$end_clr\n" "SWITCH-MONITOR is not running"
	exit -1
fi

pid=`cat ./daemon.pid`
result=`ps -p $pid | wc -l`
if test $result -le 1; then
	printf "$red_clr%50s$end_clr\n" "SWITCH-MONITOR is not running"
	exit -1
fi

ps -fs $pid 

exit 0
