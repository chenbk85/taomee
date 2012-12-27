#!/bin/sh

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="VALIDATIONCODE_DB"


create_table_sql() {
cat <<EOF >$tmp_file
   CREATE TABLE IF NOT EXISTS t_validationcode(
		id  INT UNSIGNED NOT NULL AUTO_INCREMENT,	
		code  char(16),
        PRIMARY KEY  (id)
) ENGINE=memory, CHARSET=utf8;
EOF
}

if test x$1 = x"drop" ; then
        mysqladmin -f -u $user --password="$password" -h $host DROP $dbname  
else
        mysqladmin -f -u $user --password="$password" -h $host CREATE $dbname 

        create_table_sql
        cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
fi   
