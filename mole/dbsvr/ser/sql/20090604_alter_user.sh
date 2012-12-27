#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_user_ex_table_sql() {
cat <<EOF >$tmp_file
	alter table t_user_ex_$1 
		add class_firstid INT UNSIGNED default 0;

	CREATE TABLE  t_user_class_$1 (
	classid    	   		INT UNSIGNED NOT NULL DEFAULT 0,
	class_name		    CHAR(16),
	class_slogan		CHAR(60),
	create_time			INT UNSIGNED DEFAULT 0,
	interest			INT UNSIGNED DEFAULT 0,
	class_logo			INT UNSIGNED DEFAULT 0,
	class_color			INT UNSIGNED DEFAULT 0,
	class_word			INT UNSIGNED DEFAULT 0,
	class_jion_flag			INT UNSIGNED,
	class_access_flag		INT UNSIGNED,
	member_list			VARBINARY(804),
	homeattirelist 		VARBINARY(1604),
	noused_homeattirelist    VARBINARY(3204),
	benefactor_list		VARBINARY(1654),
	PRIMARY KEY (classid)
	) ENGINE=innodb, CHARSET=utf8;

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
		let "table_index+=1"
	done
	let "db_index+=1"
done

