#!/bin/bash

db_index=0
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_user_table_sql() {

    cat <<EOF >$tmp_file
	  create table USER_$1.t_user_card_$2 (
		userid 		INT UNSIGNED NOT NULL,
		wincnt 		INT UNSIGNED NOT NULL,
		losecnt 	INT UNSIGNED NOT NULL,
  		cardlist 	blob,
		PRIMARY KEY (userid)
	) ;
EOF
}

if [ "$1" == ""  ] ; then
        echo need 0,1..3
        exit;
fi

db_index=0
end_index=100

if [ "$1" == "0" ]; then
        let "db_index=0"
        let "end_index=100"
else
        let "db_index=($1-1)*12+($1/2)"
        let "end_index=$1*12+(($1+1)/2)"
fi



echo $db_index  $end_index

while [ $db_index -lt $end_index ] ; do
	echo $db_index
	dbx=`printf "%02d" $db_index`
		table_index=0
		while [ $table_index -lt 10 ] ; do
			tbx=`printf "%01d" $table_index`
			create_user_table_sql  $dbx $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

