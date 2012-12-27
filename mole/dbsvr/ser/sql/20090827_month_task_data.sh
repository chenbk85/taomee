#!/bin/sh

insert into t_month_task_00  select * from   t_month_task where userid mod 100 =0;
while read a b c
do 
	table=$(($b%100))
	echo "insert into SYSARG_DB.t_month_task_$table values($a, $b, $c);" >> month_task.sql
done < f

mysql -uroot -pta0mee < month_task.sql
