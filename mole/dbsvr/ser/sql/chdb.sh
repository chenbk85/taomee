#!/bin/bash

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
if [ "$1" == ""  ] ; then
        echo need 0,1, 2, 3, 4 
        exit;
fi

db_index=0
end_index=100
if [ "$1" == "0" ]; then
	let "db_index=0"
	let "end_index=100"
else
	let "db_index=($1-1)*25"
	let "end_index=$1*25"
fi

create_user_table_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_user_task_$1 (
				userid   	INT UNSIGNED NOT NULL,
		        petid 		INT UNSIGNED NOT NULL,
		        taskid 		INT UNSIGNED NOT NULL,
				flag 		INT UNSIGNED NOT NULL,
				usetime 	INT UNSIGNED NOT NULL,
				starttime 	INT UNSIGNED NOT NULL,
				endtime 	INT UNSIGNED NOT NULL,
				PRIMARY KEY (userid,petid,taskid)
				) ENGINE=innodb CHARSET=UTF8;
	CREATE UNIQUE INDEX idx_user_task_id on t_user_task_$1(userid,petid,taskid);
	CREATE INDEX idx_userid on t_user_task_$1(userid);

	alter table t_user_pet_$1
        ADD COLUMN  taskid INT UNSIGNED NOT NULL default 0  after endtime  ,
        ADD COLUMN  starttime INT UNSIGNED NOT NULL after endtime  ;
EOF
}

while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%02d" $db_index`
		table_index=0
		while [ $table_index -lt 100 ] ; do
			tbx=`printf "%02d" $table_index`
			create_user_table_sql $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

