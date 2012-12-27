#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
alter_user_ex_table_sql() {
cat <<EOF >$tmp_file
	alter table t_sysarg_db_sports_gamescore add  nick CHAR(16);
	alter table t_sysarg_db_sports_petscore add  nick CHAR(16);
EOF
}





		alter_user_ex_table_sql 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "SYSARG_DB" 

