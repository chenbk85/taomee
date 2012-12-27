#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_user_ex_table_sql() {
cat <<EOF >$tmp_file
	insert into t_user_pic_book_$1 select userid, attireid from t_user_attire_$1 where attireid in (190022, 190027, 190028, 190141, 190142, 190166, 190167, 190186, 190196, 190201, 190202, 190203, 190216, 190228, 190230, 190238, 190239, 190241, 190244, 190245, 190247, 190250, 190252, 190254, 190256, 190257, 190259, 190260, 190261, 190263, 190265, 190267, 190350, 190351, 190352, 190379, 190380, 190388, 190418, 190419, 190425, 190437, 190442, 190458);
EOF
}

db_index=0
end_index=100

while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbx
	table_index=0
	while [ $table_index -lt 100 ] ; do
		tbx=`printf "%02d" $table_index`
		alter_user_ex_table_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
		let "table_index+=1"
	done
	let "db_index+=1"
done

