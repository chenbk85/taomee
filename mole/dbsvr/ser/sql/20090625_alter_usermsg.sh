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
	alter TABLE t_user_writing
		add reportdate  datetime  after userid;

	alter TABLE t_user_writing_questionnaire  
		add reportdate  datetime  after userid;
EOF
}

create_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
