#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_user_ex_table_sql() {
cat <<EOF >$tmp_file
 CREATE TABLE t_user_classmsg_$1(
  msg_id int(10) unsigned NOT NULL auto_increment,
  userid int(10) unsigned NOT NULL default '0',
  guest_id int(10) unsigned NOT NULL default '0',
  guest_nick varchar(16) NOT NULL,
  edit_time int(10) unsigned NOT NULL,
  flag int(10) unsigned default '0',
  msg_buf varchar(200) NOT NULL,
  res_buf varchar(100) default NULL,
  PRIMARY KEY  (msg_id),
  KEY id_1 (userid,flag,msg_id)
  ) ENGINE=InnoDB AUTO_INCREMENT=1008 DEFAULT CHARSET=utf8 ;
EOF
}


db_index=0
end_index=100

while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbx
	table_index=0
		while [ $table_index -lt 10 ] ; do
			tbx=`printf "%d" $table_index`
			alter_user_ex_table_sql $tbx 
			cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

