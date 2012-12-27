#!/bin/sh
#
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_sysarg_db_questionnaire_sql() {
cat <<EOF >$tmp_file
	ALTER TABLE  t_questionnaire	add	 type  INT(10) UNSIGNED NOT NULL DEFAULT 0 first;
	ALTER TABLE  t_questionnaire	drop  PRIMARY KEY;
	ALTER TABLE  t_questionnaire	add  PRIMARY KEY(type,maintype,subtype) ;
EOF
}

alter_sysarg_db_questionnaire_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host SYSARG_DB
