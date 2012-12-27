#!/bin/sh
#
#保存游戏排名
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="GAME_SCORE_DB"
create_table_sql() {
cat <<EOF >$tmp_file
   CREATE TABLE IF NOT EXISTS t_game_score(
        id 	INT UNSIGNED AUTO_INCREMENT ,
        gameid INT UNSIGNED NOT NULL DEFAULT '0',
        userid INT UNSIGNED NOT NULL DEFAULT '0',
        score  INT UNSIGNED NOT NULL DEFAULT '0',
        nick   char(16),
		PRIMARY KEY (id)
) ENGINE=innodb, CHARSET=utf8;
EOF
}
if test x$1 = x"drop" ; then
        mysqladmin -f -u $user --password="$password" -h $host DROP $dbname  
else
        mysqladmin -f -u $user --password="$password" -h $host CREATE $dbname 
        create_table_sql
        cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
fi   
