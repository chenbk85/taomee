#!/bin/bash

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="CUP_DB"

create_cup_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_cup(
		logdate INT UNSIGNED NOT NULL DEFAULT '0',  
		groupid INT UNSIGNED NOT NULL DEFAULT '0',  
        cup1    INT UNSIGNED NOT NULL DEFAULT '0',
        cup2    INT UNSIGNED NOT NULL DEFAULT '0',
        cup3    INT UNSIGNED NOT NULL DEFAULT '0',
		PRIMARY KEY (logdate, groupid )
	) ENGINE=innodb, CHARSET=utf8;
	CREATE UNIQUE INDEX idx_userid on t_cup(logdate,groupid);
EOF
}

create_table_sql() {
cat <<EOF >$tmp_file
    CREATE TABLE IF NOT EXISTS t_user_cup_$1(
		userid  INT UNSIGNED NOT NULL DEFAULT '0',  
        cup1   INT UNSIGNED NOT NULL DEFAULT '0',
        cup2   INT UNSIGNED NOT NULL DEFAULT '0',
        cup3   INT UNSIGNED NOT NULL DEFAULT '0',
		PRIMARY KEY (userid)
	) ENGINE=innodb, CHARSET=utf8;
	CREATE UNIQUE INDEX idx_userid on t_user_cup_$1(userid);
	insert into t_cup values ($1,1,0,0,0);
	insert into t_cup values ($1,2,0,0,0);
	insert into t_cup values ($1,3,0,0,0);
	insert into t_cup values ($1,4,0,0,0);
	insert into t_cup values ($1,5,0,0,0);
EOF
}

if test x$1 = x"drop" ; then
        mysqladmin -f -u $user --password="$password" -h $host DROP $dbname  
else
        mysqladmin -f -u $user --password="$password" -h $host DROP $dbname  
        mysqladmin -f -u $user --password="$password" -h $host CREATE $dbname 
        create_cup_sql
        cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname

		for (( i=20080808; i<=20080831; i++ )){
        	create_table_sql $i
        	cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
		}
	
fi   

