#!/bin/sh

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"


create_other_table_sql() {
cat <<EOF >$tmp_file
	/*原始数据*/
	/*server*/
   	CREATE TABLE IF NOT EXISTS t_server(
		logtime DATETIME NOT NULL,
		IP  char(20) NOT NULL,
		/*上送记录数 */
		logcount INT UNSIGNED NOT NULL  
	)ENGINE=innodb CHARSET=utf8;
	CREATE  UNIQUE INDEX idx on t_server( logtime ,IP );

   CREATE TABLE IF NOT EXISTS t_login (
		logtime 	DATETIME NOT NULL,
		userid 		INT UNSIGNED NOT NULL , 
		usertype 	SMALLINT UNSIGNED NOT NULL, 
		logintime 	DATETIME NOT NULL,
		onlinetime 	INT UNSIGNED NOT NULL 
	)ENGINE=innodb CHARSET=utf8;
	CREATE INDEX idx on t_login (usertype,logtime);

 	CREATE TABLE IF NOT EXISTS t_petgrade(
		logtime DATETIME NOT NULL,
		usertype SMALLINT UNSIGNED NOT NULL DEFAULT '0', 
		petgrade SMALLINT UNSIGNED NOT NULL 
	)ENGINE=innodb CHARSET=utf8;
	CREATE INDEX idx on t_petgrade (logtime,petgrade);

 	CREATE TABLE IF NOT EXISTS t_user_opt(
		logtime DATETIME NOT NULL,
		userid 	INT UNSIGNED NOT NULL DEFAULT '0', 
		flag 	INT UNSIGNED NOT NULL DEFAULT '0', 
		regpost INT UNSIGNED NOT NULL DEFAULT '0', 
		PRIMARY KEY  (logtime,userid)					
	)ENGINE=innodb CHARSET=utf8;
	CREATE INDEX idx on t_user_opt (logtime,userid );
	
	
	/*	
	--用户数据统计	用户(普通) 用户(vip) 用户在线时长统计
	--用户在线时长统计	在线0-9min	在线10min-30min	
	--在线30min-60min	在线1-2h 	在线2-3h 	在线3h 以上
	*/
   CREATE TABLE IF NOT EXISTS t_user_info(
		logtime 					DATETIME NOT NULL,
		/*用户类型*/
		usertype 					SMALLINT UNSIGNED NOT NULL DEFAULT '0', 
		/*用户总数*/
		usercount					INT UNSIGNED NOT NULL DEFAULT '0', 
		/*新增*/
		addusercount				INT UNSIGNED NOT NULL DEFAULT '0' ,
		/*登入数*/
		logincount  				INT UNSIGNED NOT NULL DEFAULT '0', 
		/*登入数(独立)*/
		logincountindependent 		INT UNSIGNED NOT NULL DEFAULT '0', 
		/*最大在线数*/
		maxonlinecount 				INT UNSIGNED NOT NULL DEFAULT '0', 
		/*在线总时长*/
		onlinetimetotal 			INT UNSIGNED NOT NULL DEFAULT '0' ,
		/*总收入/付费用户数*/
		arpu 						INT UNSIGNED NOT NULL DEFAULT '0' ,
		/*登入时间等级*/	
		logincountonlinetimelevel1	INT UNSIGNED NOT NULL DEFAULT '0' ,
		logincountonlinetimelevel2	INT UNSIGNED NOT NULL DEFAULT '0' ,
		logincountonlinetimelevel3	INT UNSIGNED NOT NULL DEFAULT '0' ,
		logincountonlinetimelevel4	INT UNSIGNED NOT NULL DEFAULT '0' ,
		logincountonlinetimelevel5	INT UNSIGNED NOT NULL DEFAULT '0' ,
		logincountonlinetimelevel6	INT UNSIGNED NOT NULL DEFAULT '0' 
	)ENGINE=innodb CHARSET=utf8;
	CREATE UNIQUE INDEX idx on t_user_info (usertype,logtime);


	/*--小米存量:按等级 1-99 100-999 1000-10000 10000以上 */
	CREATE TABLE IF NOT EXISTS t_xiaomi_level_info(
		logtime DATETIME NOT NULL,
		levelcount1	SMALLINT UNSIGNED NOT NULL DEFAULT '0', 
		levelcount2	SMALLINT UNSIGNED NOT NULL DEFAULT '0', 
		levelcount3	SMALLINT UNSIGNED NOT NULL DEFAULT '0', 
		levelcount4	SMALLINT UNSIGNED NOT NULL DEFAULT '0' 
	)ENGINE=innodb CHARSET=utf8;
	CREATE UNIQUE INDEX idx on t_xiaomi_level_info (logtime);
	/*--在线人数统计--*/
	CREATE TABLE IF NOT EXISTS t_online_usercount(
		logdate   INT UNSIGNED NOT NULL DEFAULT '0',
		msid 	  INT UNSIGNED NOT NULL DEFAULT '0',
		serverid  INT UNSIGNED NOT NULL DEFAULT '0',
		onlinecount INT UNSIGNED NOT NULL DEFAULT '0'
	)ENGINE=innodb CHARSET=utf8;
	CREATE UNIQUE INDEX idx on t_online_usercount(logdate,msid,serverid);

	CREATE TABLE IF NOT EXISTS t_xiaomee_use(
		logdate   INT UNSIGNED NOT NULL DEFAULT '0',
		hour   	  INT UNSIGNED NOT NULL DEFAULT '0',
		usecount  INT UNSIGNED NOT NULL DEFAULT '0'
	)ENGINE=innodb CHARSET=utf8;
	CREATE UNIQUE INDEX idx on t_xiaomee_use (logdate,hour);

	CREATE TABLE IF NOT EXISTS t_day_count(
		type        INT UNSIGNED NOT NULL DEFAULT '0',
		logdate     INT UNSIGNED NOT NULL DEFAULT '0',
		count       INT UNSIGNED NOT NULL DEFAULT '0'
	)ENGINE=innodb CHARSET=utf8;
	CREATE UNIQUE INDEX idx on t_day_count (type,logdate);

	CREATE TABLE IF NOT EXISTS t_day_count_ex(
		type        INT 	UNSIGNED NOT NULL DEFAULT '0',
		logdate     INT 	UNSIGNED NOT NULL DEFAULT '0',
		id1     	INT 	UNSIGNED NOT NULL DEFAULT '0',
		id2     	INT  	UNSIGNED NOT NULL DEFAULT '0',
		count       INT 	UNSIGNED NOT NULL DEFAULT '0'
	)ENGINE=innodb CHARSET=utf8;
	CREATE UNIQUE INDEX idx on t_day_count_ex (type,logdate,id1,id2);

	/*t_day_count_ex_1*/
	/*下载用时*/
	/*type=1:下载时间,id1:ip_int, id2:每隔5分钟*/

	/*愿望*/
	/*许愿类型*/
	/*type=2:统计日期,id1:愿望类型, id2:0; count:(多少人) */
	/*type=3:许愿物品分布*/

	CREATE TABLE IF NOT EXISTS t_day_count_ex_1(
		type        INT 	UNSIGNED NOT NULL DEFAULT '0',
		logdate     INT 	UNSIGNED NOT NULL DEFAULT '0',
		id1     	INT 	UNSIGNED NOT NULL DEFAULT '0',
		id2     	INT  	UNSIGNED NOT NULL DEFAULT '0',
		count       INT 	UNSIGNED NOT NULL DEFAULT '0'
	)ENGINE=innodb CHARSET=utf8;
	CREATE UNIQUE INDEX idx on t_day_count_ex_1 (type,logdate,id1,id2);

EOF
}

mysqladmin -f -u $user --password="$password" -h $host DROP "LOGDB"
mysqladmin -f -u $user --password="$password" -h $host CREATE "LOGDB"
create_other_table_sql $tbx
cat $tmp_file | mysql -u $user --password="$password" -h $host "LOGDB" 

