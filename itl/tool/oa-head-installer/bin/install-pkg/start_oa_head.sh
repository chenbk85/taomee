#!/bin/bash

red_clr="\033[31m"
grn_clr="\033[32m"
end_clr="\033[0m"

#判断是否是root用户
if [ `echo $USER` != "root" ]
then
    printf "$red_clr%70sIt must be root to run this shell.$end_clr%s\n"
    exit -1
fi

#启动用户当前所在的绝对路径
cwd=`pwd`

#本shell所在的目录
path=`dirname $0`

cd $cwd && cd $path && cd ./bin 

#判断是否已经运行
if test -e ./daemon.pid; then
	pid=`cat ./daemon.pid`
	result=`ps -p $pid | wc -l`
	if test $result -gt 1; then
		printf "$red_clr%70s$end_clr\n" "OA_HEAD is running"
		exit -1
	fi
fi

export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH &&  
