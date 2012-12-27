#!/bin/sh
#
#LAMUCLASS拉姆教室
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_sysarg_love_auction_db_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_sysarg_love_auction(
		auto_id			INT(10) UNSIGNED NOT NULL auto_increment,
		auction_itemid 	INT(10) UNSIGNED NOT NULL DEFAULT 0,
		contri_userid	INT(10) UNSIGNED NOT NULL DEFAULT 0,
		contri_nick		char(16) default 0x0000,
		auction_userid	INT(10) UNSIGNED NOT NULL DEFAULT 0,
		auction_nick	char(16) default 0x0000,
		money 			INT(10) UNSIGNED NOT NULL DEFAULT 0,
		is_old_flag		INT(10) UNSIGNED NOT NULL DEFAULT 0,
		start_time		INT(10) UNSIGNED NOT NULL DEFAULT 0,
		PRIMARY KEY (auto_id),
		UNIQUE KEY (auction_itemid, is_old_flag)
	)ENGINE=innodb CHARSET=UTF8;
EOF
}

create_sysarg_love_auction_db_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host SYSARG_DB
