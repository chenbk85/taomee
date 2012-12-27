#!/bin/sh
#
#客服程序
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="SU_CHANGE_DB"


create_table_sql() {
cat <<EOF >$tmp_file
   CREATE TABLE t_changelist(
		logdate 	datetime,
        cmdid		INT UNSIGNED NOT NULL,
        adminid		INT UNSIGNED NOT NULL,
        userid 		INT UNSIGNED NOT NULL,
        v1 			INT,
        v2 			INT,
        v3 			INT
	)ENGINE=innodb,CHARSET=utf8;
   	CREATE INDEX idx1 on t_changelist(logdate);
   	CREATE INDEX idx2 on t_changelist(userid);
EOF
}

if test x$1 = x"drop" ; then
        mysqladmin -f -u $user --password="$password" -h $host DROP $dbname  
else
        create_table_sql
        cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
fi
