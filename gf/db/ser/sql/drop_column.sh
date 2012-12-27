#!/bin/bash


user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
table_name_prefix="t_dudu_"
db_name_prefix="ddd_"
#实现
todo(){
cat <<EOF >$tmp_file
	alter table $table_name_prefix$1
	drop column nick
EOF
}

#可配参数
db_index=0
db_end_index=100
table_end_index=100


while [ $db_index -lt $db_end_index ];do
	t_db=`printf "%02d" $db_index`
	echo "db_"$t_db
	table_index=0
	while [ $table_index -lt $table_end_index ];do
		t_tb=`printf "%02d" $table_index`
		echo "table_"$t_tb
		todo $t_tb
		cat $tmp_file | mysql -u $user --password="$password" -h $host "$db_name_prefix$t_db"
		table_index=`expr $table_index + 1`
	done
	db_index=`expr $db_index + 1`
done
