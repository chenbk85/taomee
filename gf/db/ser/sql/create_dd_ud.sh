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


create_user_pet_table_sql() {
cat <<EOF >$tmp_file
   CREATE TABLE IF NOT EXISTS t_dd_ud_$1(
		userid      INT UNSIGNED NOT NULL,
		type		tinyint(1) unsigned not null,
		ip			int unsigned not null,
		confirm		tinyint(1) unsigned not null,
		PRIMARY KEY  (userid,type)
   ) ENGINE=innodb, CHARSET=utf8;
EOF

}
create_database_sql(){
cat <<EOF >$tmp_file
	CREATE DATABASE IF NOT EXISTS DD_UD_$1;
EOF
}
drop_database_sql(){
cat <<EOF >$tmp_file
	DROP DATABASE DD_UD_$1;
EOF
}
	

db_index=0
end_index=10

if [ "$1" == ""  ] ; then
        echo need 0,1..8
        exit;
fi

if [ "$1" == "0" ]; then
    let "db_index=0"
    let "end_index=10"
else
    let "db_index=($1-1)*12+($1/2)"
    let "end_index=$1*12+(($1+1)/2)"
fi
 echo $db_index $end_index

while [ $db_index -lt $end_index ] ; do
	echo $db_index
##	dbx=`printf "%02d" $db_index`
	dbx=`printf "%d" $db_index`
	table_index=0

#	drop_database_sql $dbx #
#	cat $tmp_file | mysql -u $user --password="$password" #

	create_database_sql $dbx ##
	cat $tmp_file | mysql -u $user --password="$password" ##
	while [ $table_index -lt 10 ] ; do
##		tbx=`printf "%02d" $table_index`
		tbx=`printf "%d" $table_index`
		create_user_pet_table_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "DD_UD_$dbx" 
		table_index=`expr $table_index + 1`
	done
	db_index=`expr $db_index + 1`
done

