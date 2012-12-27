#!/bin/sh

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

update_lamu_classroom_sql() {
cat << EOF >$tmp_file
	update DININGROOM.t_lamu_classroom set exp = $2, outstand_sum = $3 where userid = $1;
EOF
}

count=0
exp=0
uid=0
uid_file="uid.txt"

while read count exp uid;
do
#	echo $val1 $val2 $uid
	update_lamu_classroom_sql $uid $exp $count
	cat $tmp_file | mysql -u $user --password="$password" -h $host "DININGROOM" 
done <$uid_file

#for val in `cat uid.txt`
#do
#echo $val;
#	if [ $val_idx -eq 0 ] ; then
#		let "uid=$val"
#		echo $uid
#	fi
#	if [ $val_idx -eq 1 ] ; then
#		let "val1=$val"
#		echo $val1
#	fi
#	if [ $val_idx -eq 2 ] ; then
#		let "val2=$val"
#		echo $val2
#	fi
#	val_idx=`expr $val_idx + 1`
#	if [ $val_idx -eq 3 ] ; then
#		update_lamu_classroom_sql $uid $val1 $val2
#		cat $tmp_file | mysql -u $user --password="$password" -h $host "DININGROOM"
#		val_idx=0
#	fi
#done
