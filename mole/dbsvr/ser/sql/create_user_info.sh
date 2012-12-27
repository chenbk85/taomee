#!/bin/bash
#
#用户的配置文件
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_user_table_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_user_info_$1 (
		userid 		 INT UNSIGNED NOT NULL,
		flag1 		 INT UNSIGNED NOT NULL,/*原有FLAG:是否发的修改密码邮件*/
		flag2 		 INT UNSIGNED NOT NULL,
		regflag 	 INT UNSIGNED NOT NULL,
		passwd		 BINARY(16) NOT NULL,
		paypasswd	 BINARY(16) NOT NULL,
		nick		 CHAR(16) NOT NULL,
		email 		 CHAR(64) NOT NULL,
		parent_email CHAR(64) NOT NULL,
		passwdemail 	CHAR(64) NOT NULL,
		question 	 CHAR(64) NOT NULL,/*用于取回密码:Q*/
		answer 		 CHAR(64) NOT NULL,/*用于取回密码:A*/
        friendlist  BLOB(404),/*好友列表*/
        blacklist   BLOB(404),/*黑名单列表*/
		signature   VARCHAR(128),/*个人签名*/				
		PRIMARY KEY (userid)
	) ENGINE=innodb CHARSET=UTF8;

	CREATE TABLE IF NOT EXISTS t_user_info_ex_$1 (
		userid 		INT UNSIGNED NOT NULL,
		flag 		INT UNSIGNED NOT NULL,/*原有FLAG:0x02:性别*/
		birthday 	INT UNSIGNED NOT NULL,/*20000101*/
		telephone 	CHAR(16),/*家庭电话*/
		mobile		CHAR(16),/*移动电话*/
		mail_number INT UNSIGNED NOT NULL,/*邮编*/
		addr_province SMALLINT  NOT NULL DEFAULT 0 ,/*省份*/
		addr_city 	SMALLINT  NOT NULL DEFAULT 0 ,/*市*/
		addr 			VARCHAR(64),/*住址*/
		interest 		VARCHAR(64),/*兴趣爱好*/
		PRIMARY KEY (userid)
	) ENGINE=innodb CHARSET=UTF8;
EOF
}
db_index=0
end_index=100
if [ "$1" == ""  ] ; then
        echo need 0,1, 2, 3, 4 
        exit;
fi
if [ "$1" == "0" ]; then
	let "db_index=0"
	let "end_index=100"
else
	let "db_index=($1-1)*25"
	let "end_index=$1*25"
fi



while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%02d" $db_index`
		mysqladmin -f -u $user --password="$password" -h $host DROP "USER_INFO_$dbx"
		mysqladmin -f -u $user --password="$password" -h $host CREATE "USER_INFO_$dbx"
table_index=0
		while [ $table_index -lt 100 ] ; do
			tbx=`printf "%02d" $table_index`
			create_user_table_sql $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_INFO_$dbx" 
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

