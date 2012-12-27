#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_user_table_sql() {
cat <<EOF >$tmp_file
   	drop  table t_user_$2$3;
   	drop  table t_user_pet_$2$3;
   	drop  table t_user_connect_$2$3;
   	drop  table t_user_task_$2$3;
EOF
}

db_index=0
end_index=100
if [ "$1" == ""  ] ; then
        echo need 0,1..3
        exit;
fi

if [ "$1" == "0" ]; then
    let "db_index=0"
    let "end_index=100"
else
    let "db_index=($1-1)*33+1"
    let "end_index=($1)*33+1"
fi
if [ "$db_index" = "1" ];  then
    db_index=0
fi
echo $db_index  $end_index

while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbx
	for((table_index=0;table_index<10;table_index++)){
		for((j=0;j<10;j++)){
#echo    $dbx $table_index $j
			create_user_table_sql  $dbx $table_index  $j
			cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
		}
	}
	db_index=`expr $db_index + 1`
done

