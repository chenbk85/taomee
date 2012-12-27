#!/bin/sh

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="APPEAL_DB"

create_table_sql() {
cat <<EOF >$tmp_file
 alter table APPEAL_DB.t_appeal 
 	add answer 		char(64) after userid,
 	add question 	char(64) after  userid,
	add Ol_last 	INT UNSIGNED after userid ,
	add birthday 	INT UNSIGNED after userid ,
 	add reg_email 	char(64) after userid,
 	add reg_time 	INT UNSIGNED after userid ,	
 	add vip_way 	INT UNSIGNED after userid ,	
 	add vip 		INT 	UNSIGNED after userid ;	

EOF
}

create_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
