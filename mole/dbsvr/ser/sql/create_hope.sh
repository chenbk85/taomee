#!/bin/sh
#
#用户的许愿信息
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="HOPE_DB"


create_email_table_sql() {
cat <<EOF >$tmp_file
  CREATE TABLE IF NOT EXISTS t_hope_history(
        groupid 		INT UNSIGNED NOT NULL DEFAULT '0',
        groupname  		CHAR(64) NOT NULL,
        groupmsg 		CHAR(255) NOT NULL,
        memberlist 		INT UNSIGNED NOT NULL DEFAULT '0',
        PRIMARY KEY  (hopedate,send_id)
   ) ENGINE=innodb, CHARSET=utf8;
    CREATE UNIQUE INDEX idx_userid on t_hope_history( hopedate , send_id);
    CREATE INDEX idx2 on t_hope_history( recv_id );
EOF
}

if test x$1 = x"drop" ; then
        mysqladmin -f -u $user --password="$password" -h $host DROP $dbname  
else
        mysqladmin -f -u $user --password="$password" -h $host CREATE $dbname 

        create_email_table_sql
        cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
fi   
