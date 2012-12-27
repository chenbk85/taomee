#!/bin/sh
#
#保存记者投稿
#

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="MSGBOARD_DB"


create_table_sql() {
cat <<EOF >$tmp_file
   CREATE TABLE IF NOT EXISTS t_msgboard(
		id  	INT UNSIGNED NOT NULL AUTO_INCREMENT,	
		flag  	INT UNSIGNED NOT NULL,	
		boardid INT UNSIGNED NOT NULL,	
        logdate INT UNSIGNED NOT NULL,
        userid 	INT UNSIGNED NOT NULL,
        hot 	INT UNSIGNED NOT NULL,
        color 	INT UNSIGNED NOT NULL,
		nick	char(16),
		msg  	BLOB(1024),
        PRIMARY KEY (id)
	) ENGINE=innodb, CHARSET=utf8;
    CREATE INDEX idx1 on t_msgboard(userid);
EOF
}

if test x$1 = x"drop" ; then
        mysqladmin -f -u $user --password="$password" -h $host DROP $dbname  
else
        mysqladmin -f -u $user --password="$password" -h $host CREATE $dbname 

        create_table_sql
        cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
fi   
