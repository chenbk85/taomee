#!/bin/bash

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="INCREMENT_DB"


create_increment_table_sql() {
cat <<EOF >$tmp_file
   CREATE TABLE IF NOT EXISTS t_increment_$1 (
        userid 			INT UNSIGNED NOT NULL DEFAULT '0',
        flag 			INT UNSIGNED NOT NULL DEFAULT '0',
        damee 			INT UNSIGNED NOT NULL DEFAULT '0',
		month_paytype   INT UNSIGNED NOT NULL DEFAULT '0',
        month_used 	   	INT UNSIGNED NOT NULL DEFAULT '0', /*启用标志 0,1 */
        month_enabletime INT UNSIGNED NOT NULL DEFAULT '0', /*启用日期 time_t*/
        month_duetime	 INT UNSIGNED NOT NULL DEFAULT '0',/*到期日期 time_t*/
        month_nexterrcount INT UNSIGNED NOT NULL DEFAULT '0',/*续期失败次数*/
        PRIMARY KEY  (userid )
	) ENGINE=innodb, CHARSET=utf8;
        CREATE UNIQUE INDEX idx on t_increment_$1( userid);

EOF
}
create_month_sql() {
cat <<EOF >$tmp_file
	/*支付历史*/
 	CREATE TABLE IF NOT EXISTS t_pay_history_$1(
        transid 	INT UNSIGNED NOT NULL AUTO_INCREMENT, 	/*内部交易ID*/
        out_transid	INT UNSIGNED NOT NULL, 					/*第三方交易ID*/
        date 		INT UNSIGNED NOT NULL DEFAULT '0',		/*20050101*/	
        userid 	   	INT UNSIGNED NOT NULL DEFAULT '0',
        paytype 	INT UNSIGNED NOT NULL DEFAULT '0',
        damee 		INT UNSIGNED NOT NULL DEFAULT '0',
        monthcount  INT UNSIGNED NOT NULL DEFAULT '0', /*支付月份数*/
    	private_msg	char(30),
		dealcode 	INT UNSIGNED NOT NULL DEFAULT '0',	/*处理代码*/
		validateflag INT UNSIGNED NOT NULL DEFAULT '0',	/*已对账(确认)标志*/
        PRIMARY KEY  (transid)
	)ENGINE=innodb, CHARSET=utf8;
    CREATE  INDEX idx1 on t_pay_history_$1(out_transid,paytype);
	insert into t_pay_history_$1 values (($2+1)*100000000,0,0,0,0,0,0,"",0,1);

	CREATE TABLE IF NOT EXISTS t_damee_history_$1(
        transid 	INT UNSIGNED NOT NULL AUTO_INCREMENT, 	/*内部交易ID*/
        date 		INT UNSIGNED NOT NULL DEFAULT '0',		/*20050101*/	
        userid 	   	INT UNSIGNED NOT NULL DEFAULT '0',
        paytype 	INT UNSIGNED NOT NULL DEFAULT '0',
        damee 		INT  NOT NULL DEFAULT '0',
        leave_damee	INT UNSIGNED NOT NULL DEFAULT '0',
    	private_msg	char(20),
        PRIMARY KEY (transid)
	)ENGINE=innodb, CHARSET=utf8;
    CREATE INDEX idx1 on t_damee_history_$1(userid);
	insert into t_damee_history_$1 values (($2+1)*100000000,0,0,0,0,0,"");
EOF
}


if test x$1 = x"drop" ; then
        mysqladmin -f -u $user --password="$password" -h $host DROP $dbname  
else
        mysqladmin -f -u $user --password="$password" -h $host CREATE $dbname 

		table_index=0
		while [ $table_index -lt 100 ] ; do
			tbx=`printf "%02d" $table_index`
        	create_increment_table_sql $tbx 
        	cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
            table_index=`expr $table_index + 1`
        done

		table_index=0
		while [ $table_index -lt 40 ] ; do
			let 'table_fix=(2008+table_index/12)*100+table_index%12+1' 
            create_month_sql $table_fix   $table_index 
        	cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
            table_index=`expr $table_index + 1`
        done	
fi   
