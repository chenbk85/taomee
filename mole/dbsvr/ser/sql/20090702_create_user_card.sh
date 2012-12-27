#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_card_table_sql() {
cat <<EOF >$tmp_file
alter TABLE t_user_class_$1 add class_medal VARBINARY(804) NOT NULL;
alter TABLE t_user_pet_$1 add sick_type INT UNSIGNED NOT NULL;
CREATE TABLE IF NOT EXISTS t_user_swap_card_$1(
	userid					INT UNSIGNED NOT NULL,
	card_id					INT UNSIGNED NOT NULL,
	card_num 				INT UNSIGNED NOT NULL,
	swap_id					INT UNSIGNED NOT NULL,
	swap_flag				INT UNSIGNED NOT NULL,
	user_attire				VARBINARY(25) NOT NULL,
	primary key(userid,card_id),
	index(card_id, swap_id, swap_flag)
)ENGINE=innodb, CHARSET=utf8;
EOF
}



db_index=0
end_index=100

while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbx
	table_index=0
	while [ $table_index -lt 10 ] ; do
	echo $table_index
		tbx=`printf "%d" $table_index`
		create_card_table_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
		table_index=`expr $table_index + 1`
	done
	let "db_index+=1"
done

