#!/bin/sh
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

delete_jy_fire_table(){
cat << EOF >$tmp_file
#	drop table t_jy_fire_$1
	drop table t_gen_vip_$1
EOF
}


db_index=0
table_index=0

while [ $db_index -lt 10 ]; do
	db=`printf "%01d" $db_index`
	echo $db
	while [ $table_index -lt 10 ];do
		table=`printf "%01d" $table_index`
		delete_jy_fire_table $table
		cat $tmp_file | mysql -u $user --password="$password" -h $host "ROOMINFO_$db"
		table_index=$(( $table_index+1 ))
	done
	table_index=0
	db_index=$(($db_index + 1))
done
