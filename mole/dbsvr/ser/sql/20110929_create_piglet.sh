#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
create_user_table_sql() {
cat <<EOF >$tmp_file
CREATE TABLE IF NOT EXISTS t_user_piglet_$1(
	id 				INT UNSIGNED NOT NULL AUTO_INCREMENT,
	userid          INT UNSIGNED NOT NULL DEFAULT 0,
	pigletid        INT UNSIGNED NOT NULL DEFAULT 0,
	nick        	char(16) NOT NULL,
	mother_nick     char(16) NOT NULL,
	father_nick     char(16) NOT NULL,
	breed	 		INT UNSIGNED NOT NULL DEFAULT 0,
	sex		 		INT UNSIGNED NOT NULL DEFAULT 0,
	weight		 	INT UNSIGNED NOT NULL DEFAULT 0,
	charm		 	INT UNSIGNED NOT NULL DEFAULT 0,
	strong		 	INT UNSIGNED NOT NULL DEFAULT 0,
	birthday		INT UNSIGNED NOT NULL DEFAULT 0,
	growth_rate     INT UNSIGNED NOT NULL DEFAULT 0,
	growth_stage    INT UNSIGNED NOT NULL DEFAULT 0,
	feed_time 		INT UNSIGNED NOT NULL DEFAULT 0,
	lifetime    	INT UNSIGNED NOT NULL DEFAULT 0,
	parentid    	INT UNSIGNED NOT NULL DEFAULT 0,
	pregnant_time   INT UNSIGNED NOT NULL DEFAULT 0,
	train_point     INT UNSIGNED NOT NULL DEFAULT 0,
	train_cnt    	INT UNSIGNED NOT NULL DEFAULT 0,
	amuse_cnt     	INT UNSIGNED NOT NULL DEFAULT 0,
	carry_flag      INT UNSIGNED NOT NULL DEFAULT 0,
	transform       INT UNSIGNED NOT NULL DEFAULT 0,
	trans_time      INT UNSIGNED NOT NULL DEFAULT 0,
	trans_last      INT UNSIGNED NOT NULL DEFAULT 0,
	birth_cnt       INT UNSIGNED NOT NULL DEFAULT 0,
	dress       	varbinary(24) NOT NULL DEFAULT 0x00000000,
	generate        INT UNSIGNED NOT NULL DEFAULT 0,
	price       	INT UNSIGNED NOT NULL DEFAULT 0,
	primary key (id),
	index (userid,pigletid)
	)ENGINE=innodb,CHARSET=utf8;
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
		create_user_table_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
		let "table_index+=1"
	done
	let "db_index+=1"
done

