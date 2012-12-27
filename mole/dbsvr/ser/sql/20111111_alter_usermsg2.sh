#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
alter_usermsg_db_sql() {
cat <<EOF >$tmp_file
	ALTER TABLE t_usermsg  drop primary key, add primary key(logdate, userid, objuserid, medal);
EOF
}


alter_usermsg_db_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host USERMSG_DB
