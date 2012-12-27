#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_lamu_diary_sql() {
cat <<EOF >$tmp_file
	create table t_user_lamu_diary_$1(
			id 				INT UNSIGNED NOT NULL AUTO_INCREMENT,
			userid 			INT UNSIGNED NOT NULL DEFAULT 0,
			date			INT UNSIGNED NOT NULL DEFAULT 0,
			state			TINYINT UNSIGNED NOT NULL DEFAULT 1,
			mood        	TINYINT UNSIGNED NOT NULL DEFAULT 0,
			flower      	INT UNSIGNED NOT NULL DEFAULT 0,
			title			CHAR(31),
			lamu		 	VARCHAR(61),
			whisper 		VARCHAR(31),
			star			VARCHAR(61),
			abc				VARCHAR(145),
			content			VARCHAR(601),
			PRIMARY KEY(id),
			index(userid,id)
	) ENGINE=innodb, CHARSET=utf8;
EOF
}

db_index=0
end_index=100

while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbx
	table_index=0
	while [ $table_index -lt 10 ] ; do
	tbx=`printf "%01d" $table_index`
		create_lamu_diary_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host  "USER_$dbx" 
		let "table_index+=1"
	done
	let "db_index+=1"
done
