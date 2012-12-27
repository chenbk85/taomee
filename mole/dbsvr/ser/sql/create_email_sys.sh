#!/bin/sh
#
#保存用户邮件
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="EMAIL_SYS"


create_email_table_sql() {
cat <<EOF >$tmp_file

	CREATE TABLE IF NOT EXISTS t_user_email_$1(
		id			 INT UNSIGNED NOT NULL  AUTO_INCREMENT ,
        userid 		 INT UNSIGNED NOT NULL DEFAULT '0',
        flag 		 INT UNSIGNED NOT NULL DEFAULT '0',
        type 		 INT UNSIGNED NOT NULL DEFAULT '0',
    	sendtime 	 INT UNSIGNED NOT NULL DEFAULT '0',
    	senderid 	 INT UNSIGNED NOT NULL DEFAULT '0',
    	sendernick 	 char(16),
		mapid 		 int(10) unsigned NOT NULL,
        msg 		 char(164),
        PRIMARY KEY  (id)
	) ENGINE=innodb, CHARSET=utf8;
	CREATE UNIQUE INDEX idx on t_user_email_$1(id);
	CREATE INDEX user_idx on t_user_email_$1(userid );

EOF
}

db_index=0
while [ $db_index -lt 10 ] ; do
		dbx=`printf "%01d" $db_index`
		mysqladmin -f -u $user --password="$password" -h $host DROP "EMAIL_SYS_$dbx"
		mysqladmin -f -u $user --password="$password" -h $host CREATE "EMAIL_SYS_$dbx"
		table_index=0
		while [ $table_index -lt 10 ] ; do
			tbx=`printf "%01d" $table_index`
			create_email_table_sql $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host "EMAIL_SYS_$dbx" 
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done
