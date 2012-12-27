#!/bin/sh
#
#保存群组信息
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"


create_email_table_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_group_$1(
        groupid 		INT UNSIGNED NOT NULL DEFAULT '0',
        type 			INT UNSIGNED NOT NULL DEFAULT '0', /*类型*/
        ownerid 		INT UNSIGNED NOT NULL DEFAULT '0', /*群主ID*/
        groupname  		CHAR(25) NOT NULL,
        groupmsg 		CHAR(255) NOT NULL,
        memberlist 		blob(404),
        PRIMARY KEY  (groupid)
	) ENGINE=innodb, CHARSET=utf8;
EOF
}

create_group_main(){
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_group_main(
		groupid		 INT UNSIGNED NOT NULL  AUTO_INCREMENT ,
        PRIMARY KEY  (groupid)
	) ENGINE=innodb, CHARSET=utf8,AUTO_INCREMENT=50000;
EOF
}



mysqladmin -f -u $user --password="$password" -h $host CREATE "GROUP_MAIN"
create_group_main
cat $tmp_file | mysql -u $user --password="$password" -h $host "GROUP_MAIN" 

db_index=0
while [ $db_index -lt 10 ] ; do
		dbx=`printf "%01d" $db_index`
		mysqladmin -f -u $user --password="$password" -h $host CREATE "GROUP_$dbx"
		table_index=0
		while [ $table_index -lt 10 ] ; do
			tbx=`printf "%01d" $table_index`
			create_email_table_sql $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host "GROUP_$dbx" 
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done
