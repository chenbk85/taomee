#!/bin/bash
#
#庄园用户的个人数据
#
#

db_index=0
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_user_table_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_user_$1 (
		userid 		INT UNSIGNED NOT NULL,
		vip 		INT UNSIGNED NOT NULL,
		flag 		INT UNSIGNED NOT NULL,
		petcolor 	INT UNSIGNED NOT NULL,
		petbirthday INT UNSIGNED NOT NULL DEFAULT '0',
		xiaomee		INT UNSIGNED NOT NULL DEFAULT '0', /*小米*/
		xiaomee_max	INT UNSIGNED NOT NULL DEFAULT '0', /*当天赚了多少小米*/
		exp			INT UNSIGNED NOT NULL DEFAULT '0',
		strong		INT UNSIGNED NOT NULL DEFAULT '0',
		iq 			INT UNSIGNED NOT NULL DEFAULT '0', /*IQ*/
		charm		INT UNSIGNED NOT NULL DEFAULT '0', 
		Ol_count  	INT UNSIGNED NOT NULL,						 
		Ol_today 	INT UNSIGNED NOT NULL,						 
		Ol_last 	INT UNSIGNED NOT NULL,						 
		Ol_time 	INT UNSIGNED NOT NULL,						 
		lastip 		INT UNSIGNED NOT NULL,						 
  		friendlist blob,
    	blacklist blob,
		homeattirelist  		BLOB(1604), /*100个*/
		noused_homeattirelist 	BLOB(3204), /*400种*/
		homemap  	BLOB(50), 
		tasklist  	char(8), 
		msglist  	BLOB(2008), 
		PRIMARY KEY (userid)
	) ENGINE=innodb CHARSET=UTF8;

   	CREATE TABLE IF NOT EXISTS t_user_attire_$1 (
				   userid   	INT UNSIGNED NOT NULL,
		           attireid 	INT UNSIGNED NOT NULL,
		           usedcount 	INT UNSIGNED NOT NULL,
				   count 		INT UNSIGNED NOT NULL,
				   PRIMARY KEY  (userid,attireid )
				) ENGINE=innodb CHARSET=UTF8;
	CREATE INDEX idx_userid on t_user_attire_$1(userid);

	CREATE TABLE IF NOT EXISTS t_user_connect_$1 (
                   userid       INT UNSIGNED NOT NULL,
                   parentid     INT UNSIGNED NOT NULL,
                   childcount   INT UNSIGNED NOT NULL,
                   childlist    BLOB(800), 
                   PRIMARY KEY  (userid)
                ) ENGINE=innodb CHARSET=UTF8;


   	CREATE TABLE IF NOT EXISTS t_user_pet_$1 (
		        petid 		INT UNSIGNED NOT NULL AUTO_INCREMENT,
				userid   	INT UNSIGNED NOT NULL,
				flag 		INT UNSIGNED NOT NULL,
				flag2 		INT UNSIGNED NOT NULL,
				birthday 	INT UNSIGNED NOT NULL,
				nick		CHAR(16) NOT NULL,
				color 		INT UNSIGNED NOT NULL,
				sicktime 	INT UNSIGNED NOT NULL,
				pos_x  		INT UNSIGNED NOT NULL,
				pos_y  		INT UNSIGNED NOT NULL,

				hungry 		INT UNSIGNED NOT NULL,
				thirsty 	INT UNSIGNED NOT NULL,
				sanitary 	INT UNSIGNED NOT NULL,
				spirit 		INT UNSIGNED NOT NULL,
				endtime 	INT UNSIGNED NOT NULL,
        		starttime 	INT UNSIGNED NOT NULL ,
        		taskid 		INT UNSIGNED NOT NULL ,
        		skill		INT UNSIGNED NOT NULL ,/*技巧*/
				stamp 		INT UNSIGNED NOT NULL,
				PRIMARY KEY (petid)
				) ENGINE=innodb CHARSET=UTF8;
	CREATE INDEX idx_userid on t_user_pet_$1(userid);
	

	CREATE TABLE IF NOT EXISTS t_user_task_$1 (
				userid   	INT UNSIGNED NOT NULL,
		        petid 		INT UNSIGNED NOT NULL,
		        taskid 		INT UNSIGNED NOT NULL,
				flag 		INT UNSIGNED NOT NULL,
				usetime 	INT UNSIGNED NOT NULL,
				starttime 	INT UNSIGNED NOT NULL,
				endtime 	INT UNSIGNED NOT NULL,
				PRIMARY KEY (userid,petid,taskid)
				) ENGINE=innodb CHARSET=UTF8;
	CREATE INDEX idx_userid on t_user_task_$1(userid);


EOF
}

db_index=0
end_index=100
if [ "$1" == ""  ] ; then
        echo need 0,1..8
        exit;
fi

if [ "$1" == "0" ]; then
    let "db_index=0"
    let "end_index=100"
else
    let "db_index=($1-1)*12+($1/2)"
    let "end_index=$1*12+(($1+1)/2)"
fi
 echo $db_index $end_index
while [ $db_index -lt $end_index ] ; do
	echo $db_index
	dbx=`printf "%02d" $db_index`
		mysqladmin -f -u $user --password="$password" -h $host DROP "USER_$dbx"
		mysqladmin -f -u $user --password="$password" -h $host CREATE "USER_$dbx"
table_index=0
		while [ $table_index -lt 100 ] ; do
			tbx=`printf "%02d" $table_index`
			create_user_table_sql $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

