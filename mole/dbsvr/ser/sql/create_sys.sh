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
   CREATE TABLE IF NOT EXISTS t_sys_arg(
        type   INT UNSIGNED NOT NULL DEFAULT '0',
        value  INT NOT NULL DEFAULT '0',
        PRIMARY KEY  (type )
   ) ENGINE=innodb, CHARSET=utf8;

   CREATE TABLE IF NOT EXISTS t_questionnaire(
        maintype INT UNSIGNED NOT NULL DEFAULT '0',
        subtype  INT UNSIGNED NOT NULL DEFAULT '0',
        value  	 INT NOT NULL DEFAULT '0',
        PRIMARY KEY  (maintype,subtype)
   ) ENGINE=innodb, CHARSET=utf8;

	/*每天只能做一次,每周只能做一次*/
   CREATE TABLE IF NOT EXISTS t_day (
        type   INT UNSIGNED NOT NULL DEFAULT '0',
        userid INT UNSIGNED NOT NULL DEFAULT '0',
        count INT UNSIGNED NOT NULL DEFAULT '0',
        PRIMARY KEY  (type , userid)
   ) ENGINE=memory, CHARSET=utf8;

EOF
}

if test x$1 = x"drop" ; then
        mysqladmin -f -u $user --password="$password" -h $host DROP $dbname  
else
        mysqladmin -f -u $user --password="$password" -h $host CREATE $dbname 

        create_email_table_sql
        cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
fi   
