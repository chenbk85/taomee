#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

##sql command
update_sql()
{
cat <<EOF >$tmp_file
	update $1 set $2;
EOF
}

delete_data_sql()
{
cat <<EOF >$tmp_file
	delete from $1;
EOF
}

## $1:flag $2:db_name $3:db_to $4:tb_name $5:tb_to $6:field

db_index=0
end_index=$3
echo ""

while [ $db_index -lt $end_index ] ; do
	if [ $end_index -eq 1 ] ; then
		db_name=`echo $2`
	elif [ $end_index -eq 10 ] ; then
		dbx=`printf "%01d" $db_index`
		db_name=`echo $2_$dbx`
	elif [ $end_index -eq 100 ] ; then
		dbx=`printf "%02d" $db_index`
		db_name=`echo $2_$dbx`
	fi

	if [ $1 -eq 1 ] ; then
		echo "update $db_name.$4 $6"
	else
		echo "delete $db_name.$4"
	fi

	table_index=0
	while [ $table_index -lt $5 ] ; do
		if [ $5 -eq 1 ] ; then
			tb_name=`echo $4`
		elif [ $5 -eq 10 ] ; then
			tbx=`printf "%d" $table_index`
			tb_name=`echo $4_$tbx`
		elif [ $5 -eq 100 ] ; then
			tbx=`printf "%02d" $table_index`
			tb_name=`echo $4_$tbx`
		fi
			
		if [ $1 -eq 1 ] ; then
			update_sql $db_name.$tb_name $6
		else
			delete_data_sql $db_name.$tb_name
		fi
		#cat $tmp_file
		#cat $tmp_file | mysql -u $user --password="$password" -h $host "$2_$dbx"
		let "table_index+=1"
	done
	let "db_index+=1"
done
