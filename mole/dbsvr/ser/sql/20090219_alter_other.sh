#!/bin/bash
#拉姆运动会
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="PET_SPORTS_DB"

create_cup_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_sports_count(
		logdate			INT UNSIGNED NOT NULL DEFAULT '0',  
        groupcount1    	INT UNSIGNED NOT NULL DEFAULT '0',
        groupcount2    	INT UNSIGNED NOT NULL DEFAULT '0',
        groupcount3    	INT UNSIGNED NOT NULL DEFAULT '0',
        groupcount4   	INT UNSIGNED NOT NULL DEFAULT '0',
		PRIMARY KEY (logdate,type)
	) ENGINE=innodb, CHARSET=utf8;
EOF
}

create_pet_temp_sql() {
cat <<EOF >$tmp_file
   CREATE TABLE IF NOT EXISTS t_pet_sports_$1(
		userid 		INT UNSIGNED NOT NULL,
		petid		INT UNSIGNED NOT NULL,
		groupid 	INT UNSIGNED NOT NULL, 
		score1		INT UNSIGNED NOT NULL, 
		score2		INT UNSIGNED NOT NULL, 
		score3		INT UNSIGNED NOT NULL, 
		PRIMARY KEY (userid,petid)
    ) ENGINE=innodb, CHARSET=utf8;
EOF
}


if test x$1 = x"drop" ; then
        mysqladmin -f -u $user --password="$password" -h $host DROP $dbname  
else
        mysqladmin -f -u $user --password="$password" -h $host CREATE $dbname 
        create_cup_sql
        cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
		table_index=0
		while [ $table_index -lt 100 ] ; do
			tbx=`printf "%02d" $table_index`
			create_pet_temp_sql $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname 
			table_index=`expr $table_index + 1`
		done



fi   

