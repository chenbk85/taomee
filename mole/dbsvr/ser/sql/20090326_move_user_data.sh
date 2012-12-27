#!/bin/bash

db_index=0
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
#将USER_INFO中的一部分数据转移到USER
create_user_table_sql() {
    cat <<EOF >$tmp_file
    insert into USER_$1.t_user_ex_$3   
		select  userid,flag2&0x03,nick,friendlist,blacklist, 0x00000000, 0x0000
		from USER_INFO_$1.t_user_info_$2;
EOF
}

if [ "$1" == ""  ] ; then
        echo need 0,1..16
        exit;
fi

db_index=0
end_index=99

if [ "$1" == "0" ]; then
    let "db_index=0"
    let "end_index=99"
else
    let "fix=($1-1)/4"
    let "add1flag=($1-1)%4"
    if [ "$add1flag" == "0" ]; then
        let "db_index=($1-1)*6+fix"
    else
        let "db_index=($1-1)*6+fix+1"
    fi
    let "end_index=($1)*6+fix"
fi
echo "do db:" $db_index  "-" $end_index

while [ $db_index -le $end_index ] ; do
	echo $db_index
	dbx=`printf "%02d" $db_index`
		table_index=0
		while [ $table_index -lt 100 ] ; do
			tbx2=`printf "%02d" $table_index`
        	let "tbx1_id=$table_index/10"
			tbx1=`printf "%01d" $tbx1_id`

			create_user_table_sql  $dbx $tbx2 $tbx1
			cat $tmp_file | mysql -u $user --password="$password" -h $host
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

