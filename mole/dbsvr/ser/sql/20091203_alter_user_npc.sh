#!/bin/bash

# 2009-11-20: 更新user_xx.t_user_npx_x表的结构，添加一个新的字段——npc_3(花婶)

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"


alter_table_sql()
{
cat <<EOF >$tmp_file
	ALTER table t_user_npc_$1
	ADD  npc_6 INT DEFAULT 0 NOT NULL; 
		
EOF
}

db_index=0
end_index=100


while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbx
	table_index=0
		while [ $table_index -lt 10 ] ; do
			tbx=`printf "%d" $table_index`
			alter_table_sql $tbx 
			cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

