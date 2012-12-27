#!/bin/bash
#拉姆运动会
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="DB_CHATLOG_0"

create_pet_temp_sql() {
cat <<EOF >$tmp_file
CREATE TABLE t_chatlog_history (
  chat_sender_id int(10) NOT NULL default '0',
  chat_recver_id int(10) NOT NULL default '0',
  chat_time int(10) NOT NULL default '0',
  chat_msg_len int(5) NOT NULL default '0',
  chat_msg_buf varchar(100) default NULL,
  PRIMARY KEY  (chat_sender_id, chat_time)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
EOF
}

create_pet_temp_sql 
cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname 
