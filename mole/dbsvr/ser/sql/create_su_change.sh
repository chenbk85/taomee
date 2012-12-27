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
   CREATE TABLE IF NOT EXISTS t_changelist(
		logdate 	datetime,
        adminid		INT UNSIGNED NOT NULL,
        userid 		INT UNSIGNED NOT NULL,
        attrid 		INT UNSIGNED NOT NULL,
        changvalue 	INT NOT NULL,
        reason 		INT NOT NULL
	)ENGINE=innodb,CHARSET=utf8;
   	CREATE INDEX idx1 on t_changelist(logdate);
   	CREATE INDEX idx2 on t_changelist(userid);

 	CREATE TABLE IF NOT EXISTS t_admin(
        adminid		INT UNSIGNED NOT NULL,
      	flag 		INT UNSIGNED NOT NULL,
        nick 		char(16),	
       	passwd 		BINARY(16),
      	used 		INT UNSIGNED NOT NULL,
		PRIMARY KEY (adminid)
	)ENGINE=innodb,CHARSET=utf8;

 	CREATE TABLE IF NOT EXISTS t_msg_attime(
        deal_date	INT UNSIGNED NOT NULL,
        deal_hour	INT UNSIGNED NOT NULL,
        deal_minute	INT UNSIGNED NOT NULL,
      	flag 		INT UNSIGNED NOT NULL,
		deal_msg    BLOB(4004),
		PRIMARY KEY (deal_date,deal_hour,deal_minute)
	)ENGINE=innodb,CHARSET=utf8;
	
	insert into t_admin values (20000,0xFF,"admin",0xA420384997C8A1A93D5A84046117C2AA,1);
EOF
}

if test x$1 = x"drop" ; then
        mysqladmin -f -u $user --password="$password" -h $host DROP $dbname  
else
        mysqladmin -f -u $user --password="$password" -h $host CREATE $dbname 
        create_table_sql
        cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
fi
