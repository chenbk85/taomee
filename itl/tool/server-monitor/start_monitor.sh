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
    echo -e "---------$red_clr $server_name is already running $end_clr---------"
    ps -ef | grep -E "^$log_name|^$log_id" | grep $server_name | grep -v grep
fi
if [  "$result" == ""  ]; then
    echo -e "---------$grn_clr $server_name is not running, start server $end_clr---------"
    cd ./bin/
    #参数说明: u数据库用户名， h数据库所在主机，p数据库登陆密码，P数据库连接端口，e:不监控网段ID列表
    ./oa_alarm_monitor -uroot -h10.1.1.27 -pta0mee -P3306 -e36
fi
