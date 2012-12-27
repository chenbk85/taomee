#!/bin/bash
#
# insert into t_deal_month_history 
user="root"
password="ta0mee"
host="localhost"
tmp_file="tmp.sql"
dealdate=`date '+%Y/%m/%d 00:00:00'	-d'+2 month'`
payvalue=10
monthcount=1
dealflag=2 #未处理

create_sql() {
cat <<EOF >$tmp_file
	insert into t_deal_month_history  
	select  "$dealdate", userid,paytype,1,$payvalue,$dealflag  from  t_user_month 
	where  used=1 and  duetime<"$dealdate" and disableflag=0;
EOF
}

create_sql $tbx 
cat $tmp_file | mysql -u $user --password="$password" -h $host "INCREMENT_DB" 
