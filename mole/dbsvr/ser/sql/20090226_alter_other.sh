#!/bin/bash
#拉姆运动会
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="PET_SPORTS_DB"

create_cup_sql() {
cat <<EOF >$tmp_file
	DROP table  IF EXISTS t_sports_count;	
	CREATE TABLE IF NOT EXISTS t_sports_count(
		logdate			INT UNSIGNED NOT NULL DEFAULT '0',  
        groupcount1    	INT UNSIGNED NOT NULL DEFAULT '0',
        groupcount2    	INT UNSIGNED NOT NULL DEFAULT '0',
        groupcount3    	INT UNSIGNED NOT NULL DEFAULT '0',
        groupcount4   	INT UNSIGNED NOT NULL DEFAULT '0',
		PRIMARY KEY (logdate)
	) ENGINE=innodb, CHARSET=utf8;
	DROP table  IF EXISTS t_pet_max_score ;	
	CREATE TABLE IF NOT EXISTS t_pet_max_score(
		type           	INT UNSIGNED NOT NULL DEFAULT '0',
		score          	INT UNSIGNED NOT NULL DEFAULT '0',
		groupid 		INT UNSIGNED NOT NULL DEFAULT '0',
		usernick        char(16),
		petnick        char(16),
		PRIMARY KEY (type)
	) ENGINE=innodb, CHARSET=utf8;

	insert into  t_pet_max_score values(1,0,0,"","");
	insert into  t_pet_max_score values(2,0,0,"","");
	insert into  t_pet_max_score values(3,0,0,"","");

EOF
}

create_cup_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname

