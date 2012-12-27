#!/bin/sh
#
#系统数
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="PARTY_DB"

create_email_table_sql() {
cat <<EOF >$tmp_file
   CREATE TABLE IF NOT EXISTS t_party(
            partydate   INT UNSIGNED NOT NULL,
            serverid    INT UNSIGNED NOT NULL,
            partytime   INT UNSIGNED NOT NULL,
            mapid       INT UNSIGNED NOT NULL,
            partytype   INT UNSIGNED NOT NULL,
            ownerid     INT UNSIGNED NOT NULL,
            ownerflag   INT UNSIGNED NOT NULL,
            ownercolor  INT UNSIGNED NOT NULL,
            ownernick   char (16) NOT NULL,
            KEY idx1 (partydate,serverid,partytime),
            KEY idx2 (ownerid,partytime)
   ) ENGINE=innodb, CHARSET=utf8;
EOF
}

if test x$1 = x"drop" ; then
        mysqladmin -f -u $user --password="$password" -h $host DROP $dbname  
else
        mysqladmin -f -u $user --password="$password" -h $host CREATE $dbname 
        create_email_table_sql
        cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
fi   
