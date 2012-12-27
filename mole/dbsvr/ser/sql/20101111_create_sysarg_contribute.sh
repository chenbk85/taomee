#!/bin/sh
#
#LAMUCLASS拉姆教室
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_sysarg_contribute_rank_db_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_sysarg_contribute_rank(
		userid INT(10) UNSIGNED NOT NULL DEFAULT 0,
		contri_xiaomee INT(10) UNSIGNED NOT NULL DEFAULT 0,
		contri_attire INT(10) UNSIGNED NOT NULL DEFAULT 0,
		guess_right_num INT(10) UNSIGNED NOT NULL DEFAULT 0,
		PRIMARY KEY (userid),
		INDEX idx_xiaomee (contri_xiaomee),
		INDEX idx_attire (contri_attire),
		INDEX idx_right_num (guess_right_num)
	)ENGINE=innodb CHARSET=UTF8;
EOF
}

create_sysarg_contribute_rank_db_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host SYSARG_DB
