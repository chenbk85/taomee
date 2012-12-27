#!/bin/bash
#
#每天投稿编号初始化
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="USERMSG_DB"


create_table_sql() {
cat <<EOF >$tmp_file
   insert into t_user_writing 
   values (0,0,0,$1000000,0,'','' );
EOF
}
#得到今天日期
now=`date   +'%Y%m%d'   ` 
start_day="20080101" 
#得到秒数
now_s=`date   -d  "$now"    "+%s"`
start_s=`date   -d  "$start_day"    "+%s"`
#得到相关的天数
let  "day_count=(now_s-start_s)/86400" 
echo $day_count     
create_table_sql $day_count 
cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
