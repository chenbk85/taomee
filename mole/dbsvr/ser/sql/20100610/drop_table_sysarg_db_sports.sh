#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
alter_user_ex_table_sql() {
cat <<EOF >$tmp_file
	drop table t_sysarg_db_sports_gamescore,t_sysarg_db_sports_petscore,t_sysarg_db_sports_teaminfo,
		 t_sysarg_db_sports_tmpuser;
EOF
}


		alter_user_ex_table_sql 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "SYSARG_DB" 

