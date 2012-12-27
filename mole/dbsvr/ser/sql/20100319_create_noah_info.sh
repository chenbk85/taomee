#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_noah_info_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_noah_user_info_$1(
		userid			INT UNSIGNED NOT NULL DEFAULT '0',
		sn	    		CHAR(20) NOT NULL,		
		last_time		INT UNSIGNED NOT NULL DEFAULT '0',
		xiaomee			INT UNSIGNED NOT NULL DEFAULT '0',
		exp				INT UNSIGNED NOT NULL DEFAULT '0',
		iq				INT UNSIGNED NOT NULL DEFAULT '0',
		charm			INT UNSIGNED NOT NULL DEFAULT '0',
		strong			INT UNSIGNED NOT NULL DEFAULT '0',
		PRIMARY KEY(userid,sn,last_time)
	) ENGINE=innodb, CHARSET=utf8;
EOF
}


table_index=0
while [ $table_index -lt 10 ];do
	tabx=`printf "%d" $table_index` 
    create_noah_info_sql $tabx
    cat $tmp_file | mysql -u $user --password="$password" -h $host "NOAH"
	table_index=$(($table_index + 1))
done

