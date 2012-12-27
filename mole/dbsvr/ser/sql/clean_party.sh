#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

logdate=`date +%Y%m%d `
create_other_table_sql() {
cat <<EOF >$tmp_file
 delete from PARTY_DB.t_party where partydate<$logdate;
EOF
}
create_other_table_sql $tbx
cat $tmp_file | mysql -u $user --password="$password" -h $host

