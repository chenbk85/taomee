#!/bin/sh
server_name='oa_alarm_monitor'
ps -eo user,pid,stat,pcpu,pmem,cmd | grep "\<$server_name\>\|^USER" | grep -v grep
