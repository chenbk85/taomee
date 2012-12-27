#!/bin/sh

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="APPEAL_DB"


create_table_sql() {
cat <<EOF >$tmp_file
   CREATE TABLE IF NOT EXISTS t_appeal(
		id  		INT UNSIGNED NOT NULL AUTO_INCREMENT,	
		state		INT UNSIGNED ,	
		dealflag  	INT UNSIGNED ,	
		dealtime  	INT UNSIGNED ,	
		logtime  	INT UNSIGNED ,	
		userid  	INT UNSIGNED ,	
		realname    char(16),
		email   	char(64),
		mobile  	char(16),
		telephone  	char(16),
		realpasswd1 char(20),
		realpasswd2 char(20),
		realpasswd3 char(20),
		addr 		varchar(200),
		othermsg 	varchar(600),
		key(state,dealflag, logtime),
        PRIMARY KEY (id)
	) ENGINE=innodb, CHARSET=utf8;
EOF
}

mysqladmin -f -u $user --password="$password" -h $host DROP $dbname
mysqladmin -f -u $user --password="$password" -h $host CREATE $dbname
create_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
