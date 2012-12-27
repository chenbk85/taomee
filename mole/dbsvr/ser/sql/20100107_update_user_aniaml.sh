#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

#动物最大产出初始化

alter_user_ex_table_sql() {
	cat <<EOF >$tmp_file
	update t_user_animal_$1 set max_output =  10 where animalid=	1270001;
	update t_user_animal_$1 set max_output =  20 where animalid=	1270002;
	update t_user_animal_$1 set max_output =  10 where animalid=	1270003;
	update t_user_animal_$1 set max_output =  10 where animalid=	1270004;
	update t_user_animal_$1 set max_output =  10 where animalid=	1270005;
	update t_user_animal_$1 set max_output =   0 where animalid=	1270006;
	update t_user_animal_$1 set max_output =  10 where animalid=	1270007;
	update t_user_animal_$1 set max_output =  0 where animalid=		1270008;
	update t_user_animal_$1 set max_output =  10 where animalid=	1270009;
	update t_user_animal_$1 set max_output =  10 where animalid=	1270010;
	update t_user_animal_$1 set max_output =  10 where animalid=	1270011;
	update t_user_animal_$1 set max_output =  10 where animalid=	1270012;
	update t_user_animal_$1 set max_output =  10 where animalid=	1270013;
	update t_user_animal_$1 set max_output =  10 where animalid=	1270014;
	update t_user_animal_$1 set max_output =   1 where animalid=	1270015;
	update t_user_animal_$1 set max_output =   0 where animalid=	1270016;
	update t_user_animal_$1 set max_output =   0 where animalid=	1270017;
	update t_user_animal_$1 set max_output =   0 where animalid=	1270018;
	update t_user_animal_$1 set max_output =   6 where animalid=	1270019;
	update t_user_animal_$1 set max_output =   0 where animalid=	1270020;
	update t_user_animal_$1 set max_output =  0 where animalid=		1270021;
	update t_user_animal_$1 set max_output =   5 where animalid=	1270022;
	update t_user_animal_$1 set max_output =   3 where animalid=	1270023;
	update t_user_animal_$1 set max_output =   5 where animalid=	1270024;
	update t_user_animal_$1 set max_output =   5 where animalid=	1270025;
	update t_user_animal_$1 set max_output =   0 where animalid=	1270026;
	update t_user_animal_$1 set max_output =   0 where animalid=	1270027;
	update t_user_animal_$1 set max_output =  0 where animalid=		1270028;
	update t_user_animal_$1 set max_output =   0 where animalid=	1270029;
	update t_user_animal_$1 set max_output =  10 where animalid=	1270030;
	update t_user_animal_$1 set max_output =  10 where animalid=	1270031;
	update t_user_animal_$1 set max_output =  10 where animalid=	1270032;
	update t_user_animal_$1 set max_output =  10 where animalid=	1270033;
	update t_user_animal_$1 set max_output =  10 where animalid=	1270034;
	update t_user_animal_$1 set max_output =  10 where animalid=	1270035;
	update t_user_animal_$1 set max_output =  0 where animalid=		1270036;
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

