#!/bin/sh
#
#举报和投稿
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="PP_SUBMIT_DB"


create_table_sql() {
cat <<EOF >$tmp_file
		CREATE TABLE t_report_deal_history (
		  logdate datetime default NULL,
		  userid int(10) unsigned NOT NULL,
		  adminid int(10) unsigned NOT NULL,
		  deal_type int(10) unsigned NOT NULL,
		  reason int(10) unsigned NOT NULL,
		  nick char(16) NOT NULL,
		  KEY idx1 (logdate)
		) ENGINE=MyISAM DEFAULT CHARSET=utf8
EOF
}
create_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
