#!/bin/sh
#毛毛树
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"


create_email_table_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_mms_$1(
        type 			INT UNSIGNED NOT NULL DEFAULT '0', /*类型*/
        userid 			INT UNSIGNED NOT NULL DEFAULT '0', 
       	opt_time  		INT UNSIGNED NOT NULL DEFAULT '0',/*操作时间*/
       	opt_count 		INT UNSIGNED NOT NULL DEFAULT '0',/*操作次数*/
       	v1 				INT UNSIGNED NOT NULL DEFAULT '0',/*水*/
       	v2 				INT UNSIGNED NOT NULL DEFAULT '0',/*泥*/
       	v3 				INT UNSIGNED NOT NULL DEFAULT '0',/*总共多少个果实*/
       	v4 				INT UNSIGNED NOT NULL DEFAULT '0',/*拿了多少果实*/
       	v5 				INT UNSIGNED NOT NULL DEFAULT '0',
        opt_list 		blob(1504),
        PRIMARY KEY  (type,userid)
	) ENGINE=innodb, CHARSET=utf8;
EOF
}


db_index=0
while [ $db_index -lt 10 ] ; do
		dbx=`printf "%01d" $db_index`
		mysqladmin -f -u $user --password="$password" -h $host CREATE "MMS_$dbx"
		table_index=0
		while [ $table_index -lt 10 ] ; do
			tbx=`printf "%01d" $table_index`
			create_email_table_sql $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host "MMS_$dbx" 
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done
