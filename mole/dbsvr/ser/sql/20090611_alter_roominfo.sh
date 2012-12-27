#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_user_ex_table_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE t_class_qa_$1 ( 
		classid INT UNSIGNED NOT NULL,
		score  INT UNSIGNED NOT NULL DEFAULT 0,
		count  INT UNSIGNED NOT NULL DEFAULT 0,
		logdate INT UNSIGNED NOT NULL DEFAULT 0,
		member_list		VARBINARY(404),
		member_score_list	VARBINARY(804),
		PRIMARY KEY (classid)
		) ENGINE=innodb, CHARSET=utf8;

	CREATE TABLE IF NOT EXISTS t_gen_vip_$1 ( 
		userid INT UNSIGNED NOT NULL,
		time   INT UNSIGNED NOT NULL,
		count  INT UNSIGNED NOT NULL DEFAULT 0,
		PRIMARY KEY (userid)
		) ENGINE=innodb, CHARSET=utf8;

	drop table t_jy_fire_$1;

EOF
}

db_index=0
end_index=10

while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%01d" $db_index`
	echo $dbx
	table_index=0
	while [ $table_index -lt 10 ] ; do
		tbx=`printf "%d" $table_index`
		alter_user_ex_table_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "ROOMINFO_$dbx" 
		let "table_index+=1"
	done
	let "db_index+=1"
done

