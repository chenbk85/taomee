#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

logtime=`date +%s -d'-2 days'`
create_other_table_sql() {
cat <<EOF >$tmp_file
 delete from USER_SERIAL_DB.t_user_serial where gentime<$logtime;
EOF
}
create_other_table_sql $tbx
cat $tmp_file | mysql -u $user --password="$password" -h $host

