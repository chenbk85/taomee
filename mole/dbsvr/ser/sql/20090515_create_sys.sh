#!/bin/sh
#
#系统数
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="SYSARG_DB"

create_email_table_sql() {
cat <<EOF >$tmp_file
   CREATE TABLE IF NOT EXISTS t_questionnaire(
        maintype INT UNSIGNED NOT NULL DEFAULT '0',
        subtype  INT UNSIGNED NOT NULL DEFAULT '0',
        value  	 INT NOT NULL DEFAULT '0',
        PRIMARY KEY  (maintype,subtype)
   ) ENGINE=innodb, CHARSET=utf8;

EOF
}

if test x$1 = x"drop" ; then
else
        create_email_table_sql
        cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
fi   
