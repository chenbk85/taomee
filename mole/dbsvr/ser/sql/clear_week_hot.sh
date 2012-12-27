#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
create_user_table_sql() {
cat <<EOF >$tmp_file
        update  ROOMINFO_$1.t_roominfo_$2 set weekhot=0;
EOF
}

if [ "$1" == ""  ] ; then
        echo need 0,1, 2
        exit;
fi

if [ "$1" == "0" ]; then
	start_dbid=0
	end_dbid=9
elif [ "$1" == "1" ]; then
	start_dbid=0
	end_dbid=4
			
elif [ "$1" == "2" ]; then
	start_dbid=5
	end_dbid=9
fi

for(( dbid=start_dbid;dbid<=end_dbid; dbid++ )) {

	echo  "deal " $dbid
	for((tableid=0;tableid<=9; tableid++ )){
		create_user_table_sql $dbid $tableid
		cat $tmp_file | mysql -u $user --password="$password" -h $host 
	}

}

