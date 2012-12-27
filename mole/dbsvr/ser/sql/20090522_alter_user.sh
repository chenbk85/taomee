#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

#设置所有的号都注册过摩尔庄园了
create_user_table_sql() {
cat <<EOF >$tmp_file
	alter table t_user_ex_$1 
	add  professionlist  binary(200) default  NULL; 
		
EOF
}



db_index=0
end_index=100

if [ "$1" == "0" ]; then
        let "db_index=0"
        let "end_index=100"
else
        let "db_index=($1-1)*12+($1/2)"
        let "end_index=$1*12+(($1+1)/2)"
fi



while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbx
	table_index=0
		while [ $table_index -lt 10 ] ; do
			tbx=`printf "%d" $table_index`
			create_user_table_sql $tbx 
			cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

