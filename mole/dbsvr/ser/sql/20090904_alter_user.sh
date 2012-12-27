#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_user_ex_table_sql() {
cat <<EOF >$tmp_file
	alter TABLE t_user_$1  
		change noused_homeattirelist noused_homeattirelist  varbinary(8004),	
		change homeattirelist homeattirelist_1  varbinary(1604),	
	  	add homeattirelist_3  varbinary(1604) default 0x00000000 after homeattirelist_1 ,	
	  	add homeattirelist_2  varbinary(1604) default 0x00000000 after homeattirelist_1 ;	

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

