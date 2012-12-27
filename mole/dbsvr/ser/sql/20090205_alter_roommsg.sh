#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
create_user_table_sql() {
cat <<EOF >$tmp_file
 CREATE TABLE  ROOMMSG_$1.t_roommsg_$2 (
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
  )ENGINE=InnoDB DEFAULT CHARSET=utf8; 

  insert into  ROOMMSG_$1.t_roommsg_$2 select * from  DB_ROOMMSG_0.t_roommsg_$2$1;

EOF
}



db_index=0
end_index=10

while [ $db_index -lt $end_index ] ; do
	echo $db_index
	dbx=`printf "%01d" $db_index`
		mysqladmin -f -u $user --password="$password" -h $host DROP "ROOMMSG_$dbx"
		mysqladmin -f -u $user --password="$password" -h $host CREATE "ROOMMSG_$dbx"
		table_index=0
		while [ $table_index -lt 10 ] ; do
			tbx=`printf "%01d" $table_index`
			create_user_table_sql $dbx $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host 
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

