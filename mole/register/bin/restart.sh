#!/bin/sh

main_num=`ps -ef | grep -w 'Register' | grep -v 'grep' | grep -w MAIN | wc -l`
conn_num=`ps -ef | grep -w 'Register' | grep -v 'grep' | grep -w CONN | wc -l`
work_num=`ps -ef | grep -w 'Register' | grep -v 'grep' | grep -w WORKER | wc -l`
main=/home/andy/register
bin_dir=$main/bin
etc_dir=$main/etc

if [ $conn_num -eq 0 -o $work_num -eq 0 ]
then
	echo "Restarting Register - " `date`
	killall -TERM Register > /dev/null 2>&1
	#reboot the application
	$bin_dir/Register $etc_dir/bench.conf $bin_dir/libregister.so  -s 4096 > /dev/null 2>&1
	echo "Register Restarted!"
fi
