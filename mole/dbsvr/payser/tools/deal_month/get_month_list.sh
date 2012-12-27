#!/bin/bash

user="root"
password="ta0mee"
host="localhost"
tmp_file="tmp.sql"
if [ $# -ne 1  ]  ; then
	echo "request: $0 paytype "
	exit
fi

paytype=$1
outdata_file="month_list`date '+%Y%m'`_$paytype" 
no_deal_flag=2

create_sql() {
cat <<EOF >$tmp_file
	select transid, userid , damee from t_deal_month_history  
	where  dealflag=$no_deal_flag and paytype=$paytype ;
EOF
}

create_sql $tbx 
cat $tmp_file | mysql -u $user --password="$password" -h $host "INCREMENT_DB"| sed -n -e '2,$p' >$outdata_file
