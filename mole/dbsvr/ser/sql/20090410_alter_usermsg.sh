#!/bin/sh
#
#举报和投稿
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="USERMSG_DB"


create_table_sql() {
cat <<EOF >$tmp_file
	alter table t_report_deal_history 
        add nick  char(16)  NOT NULL ;
EOF
}

create_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
