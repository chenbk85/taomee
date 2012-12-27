#!/bin/sh
#
#保存用户邮件
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="EMAIL_SYS"


create_email_table_sql() {
cat <<EOF >$tmp_file

    alter table t_user_email_$1
		MODIFY	msg blob;
EOF
}

db_index=0
while [ $db_index -lt 10 ] ; do
		dbx=`printf "%01d" $db_index`
		table_index=0
		while [ $table_index -lt 10 ] ; do
			tbx=`printf "%01d" $table_index`
			create_email_table_sql $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host "EMAIL_SYS_$dbx" 
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done
