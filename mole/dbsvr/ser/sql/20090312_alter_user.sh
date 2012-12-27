#!/bin/bash

db_index=0
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_user_table_sql() {
    cat <<EOF >$tmp_file
		/*家园 */
		alter table t_user_$1 
        	add   noused_jyattirelist varbinary(3204) 
				default  0x00000000 
			after noused_homeattirelist, 
        	add  jyattirelist  varbinary(1604)  
			 default 0x01000000A19D1200000000000000000000000000
			 after   noused_homeattirelist ;

		/*銀行 */
		CREATE TABLE t_user_bank_$1 (
		  userid int(10) unsigned NOT NULL,
		  time int(10) unsigned NOT NULL,
		  money int(10) unsigned NOT NULL,
		  flag int(10) unsigned NOT NULL,
		  timelimit int(10) unsigned NOT NULL,
		  PRIMARY KEY  (userid,time)
		) ENGINE=InnoDB DEFAULT CHARSET=utf8 ;
		
		/*种子 */
	create table t_user_jy_$1 (
		id 				INT UNSIGNED NOT NULL AUTO_INCREMENT,	
		userid 			INT UNSIGNED NOT NULL,
		attireid		INT UNSIGNED NOT NULL,
		x  				INT UNSIGNED NOT NULL,
		y  				INT UNSIGNED NOT NULL,
		value  			INT UNSIGNED NOT NULL,
		sickflag  		INT UNSIGNED NOT NULL,
		fruitnum  		INT UNSIGNED NOT NULL,
		cal_value_time  INT UNSIGNED NOT NULL,
		water_time  	INT UNSIGNED NOT NULL,
		kill_bug_time  	INT UNSIGNED NOT NULL,
		status			INT UNSIGNED NOT NULL,
		mature_time  	INT UNSIGNED NOT NULL,
		PRIMARY KEY (id),
		key(userid)
	)ENGINE=innodb CHARSET=UTF8;

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
		while [ $table_index -lt 10 ] ; do
			tbx=`printf "%01d" $table_index`
			create_user_table_sql  $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host USER_$dbx
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

