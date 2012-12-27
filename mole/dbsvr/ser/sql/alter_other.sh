#!/bin/sh

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="USERMSG_DB"


create_table_sql() {
cat <<EOF >$tmp_file
   CREATE TABLE IF NOT EXISTS t_user_writing_questionnaire (
		logdate 	datetime,
        type 		INT UNSIGNED NOT NULL,
        flag 		INT UNSIGNED NOT NULL,
        value 		INT UNSIGNED NOT NULL,
        userid 		INT UNSIGNED NOT NULL,
		msg 		BLOB(4004),
        report 		BLOB(1004) 
	)ENGINE=innodb, CHARSET=utf8;
	insert into  t_user_writing_questionnaire
	select * from t_user_writing where type>=1010;
	delete  from t_user_writing where type>=1010;

    CREATE INDEX idx1 on t_user_writing_questionnaire(logdate);
    CREATE INDEX idx2 on t_user_writing_questionnaire(userid);
    CREATE INDEX idx2 on t_user_writing(userid);
EOF

}

create_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
