#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

#班级问题题
create_vip_table(){
cat << EOF >$tmp_file
	alter table t_class_qa_$1 add class_flag VARBINARY(804) NOT NULL;
EOF
}


db_index=0
table_index=0
for((db_index=0;db_index<10;db_index++ )){
	for((table_index=0;table_index <10;table_index++ )){
		table=`printf "%01d" $table_index`
		create_vip_table $table
		cat $tmp_file | mysql -u $user --password="$password" -h $host "ROOMINFO_$db_index"
	}
}

