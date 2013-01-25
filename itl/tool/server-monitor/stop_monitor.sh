#!/bin/sh

######################################################################################
#
# 针对应用需要修改:
# 1、server_name 为需要管理的服务器可执行文件的名称
#
######################################################################################

server_name="oa_alarm_monitor"
log_name=$LOGNAME
log_id=`cat /etc/passwd | grep $log_name | awk -F ':' '{print $3}'`

red_clr="\033[31m"
grn_clr="\033[32m"
end_clr="\033[0m"

result=`ps -ef | grep -E "^$log_name|^$log_id" | grep $server_name | grep -v grep`
if [  "$result" != ""  ]; then
    echo -e "---------$red_clr $server_name is running, stop the server $end_clr---------"
    ps -ef | grep -E "^$log_name|^$log_id" | grep $server_name | grep -v grep | awk '{print "echo kill " $2 " " $8 "; kill -9 " $2;}' | sh
fi
if [  "$result" == ""  ]; then
    echo -e "---------$grn_clr $server_name is not running $end_clr---------"
fi
