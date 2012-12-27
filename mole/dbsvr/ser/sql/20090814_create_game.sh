#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_user_ex_table_sql() {
cat <<EOF >$tmp_file
 CREATE TABLE t_user_game_$1(
 	userid  INT UNSIGNED NOT NULL DEFAULT 0,
 	gameid 	tinyint UNSIGNED NOT NULL DEFAULT 0,
 	score 	int UNSIGNED NOT NULL DEFAULT 0,
 	pkflag tinyint UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY ( userid , gameid )
  ) ENGINE=memory DEFAULT CHARSET=utf8 ;
 CREATE TABLE t_user_game_pk_$1(
 	userid  INT UNSIGNED NOT NULL DEFAULT 0,
 	win_count int UNSIGNED NOT NULL DEFAULT 0,
 	lose_count int UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY ( userid )
  ) ENGINE=memory DEFAULT CHARSET=utf8 ;



 CREATE TABLE t_game_$1(
 	userid  INT UNSIGNED NOT NULL DEFAULT 0,
 	score 	INT UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY ( userid , score )
  ) ENGINE=memory DEFAULT CHARSET=utf8 ;

EOF
}


mysqladmin -f -u $user --password="$password" -h $host CREATE "MOLE_GAME_DB" 
for((i=0;i<100;i++)){
	tbx=`printf "%02d" $i`
	alter_user_ex_table_sql $tbx 
	cat $tmp_file | mysql -u $user --password="$password" -h $host "MOLE_GAME_DB" 
}

