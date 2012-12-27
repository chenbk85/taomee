#!/bin/bash
#
#庄园用户的个人数据
#
#

db_index=0
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_user_table_sql() {
cat <<EOF >$tmp_file
	create table temp as select userid,type,id,optid,opttime,nick from t_dd_opt_history_$1;
	drop table t_dd_opt_history_$1;
	create table t_dd_opt_history_$1(
 	userid int(10) unsigned NOT NULL ,
 	type int(10) unsigned NOT NULL ,
 	id int(10) unsigned NOT NULL ,
 	optid int(10) unsigned NOT NULL ,
 	opttime int(10) unsigned NOT NULL ,
 	nick binary(16),
    PRIMARY KEY  (userid,type,id)
   ) ENGINE=innodb, CHARSET=utf8;
	insert into t_dd_opt_history_$1 select * from temp;
	drop table temp;
EOF
}



db_index=0
end_index=100

while [ $db_index -lt $end_index ] ; do
	echo $db_index
	dbx=`printf "%02d" $db_index`
	table_index=0
	while [ $table_index -lt 100 ] ; do
		echo "table " $tbx
		tbx=`printf "%02d" $table_index`
		create_user_table_sql $tbx
		cat $tmp_file | mysql -u $user --password="$password" -h $host "DD_$dbx" 
		table_index=`expr $table_index + 1`
	done
	db_index=`expr $db_index + 1`
done

