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
	drop table t_dd_msg_$1;
   CREATE TABLE IF NOT EXISTS t_dd_msg_$1(
		userid      INT UNSIGNED NOT NULL,
		msgid		INT UNSIGNED NOT NULL,
		flag 		tinyint(1) unsigned not null,
		type 		tinyint(1) unsigned not null,
		senderid    INT UNSIGNED NOT NULL,
		sendtime    INT UNSIGNED NOT NULL,
		sendnick    char(16),
		msg 		varchar(516) not null,
		PRIMARY KEY  (userid,msgid)
   ) ENGINE=innodb, CHARSET=utf8;
EOF

}

db_index=0
end_index=100

if [ "$1" == ""  ] ; then
        echo need 0,1..8
        exit;
fi

if [ "$1" == "0" ]; then
    let "db_index=0"
    let "end_index=100"
else
    let "db_index=($1-1)*12+($1/2)"
    let "end_index=$1*12+(($1+1)/2)"
fi
 echo $db_index $end_index

while [ $db_index -lt $end_index ] ; do
	echo $db_index
	dbx=`printf "%02d" $db_index`
	table_index=0
	while [ $table_index -lt 100 ] ; do
		tbx=`printf "%02d" $table_index`
		create_user_pet_table_sql $tbx
		cat $tmp_file | mysql -u $user --password="$password" -h $host "DD_$dbx" 
		table_index=`expr $table_index + 1`
	done
	db_index=`expr $db_index + 1`
done

