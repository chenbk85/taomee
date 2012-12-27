#!/bin/bash

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_user_lamu_classroom_table_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_user_lamu_classroom_$1(
		userid      INT(10) UNSIGNED NOT NULL DEFAULT 0,/*米米号*/
		room_id		INT(10) UNSIGNED NOT NULL DEFAULT 0,/*拉姆教室id*/
		room_name	CHAR(16) NOT NULL,					/*教室名称*/
		inner_style INT(10) UNSIGNED NOT NULL DEFAULT 0,/*教室内部装潢*/
		exam_times	TINYINT UNSIGNED NOT NULL DEFAULT 0,/*考试的次数*/
		exp			INT(10) UNSIGNED NOT NULL DEFAULT 0,	/*教师经验*/
		energy		INT(10) UNSIGNED NOT NULL DEFAULT 0,/*学生精力*/
		lovely		INT(10) UNSIGNED NOT NULL DEFAULT 0,/*亲密度*/
		difficulty	INT(10) UNSIGNED NOT NULL DEFAULT 0,/*教学困难度*/
		level_s		INT(10) UNSIGNED NOT NULL DEFAULT 0,/*S级考评总次数*/
		graduate_sum INT(10) UNSIGNED NOT NULL DEFAULT 0,/*毕业生总数*/
		outstand_sum INT(10) UNSIGNED NOT NULL DEFAULT 0,/*优秀毕业生总数*/
		evaluate	TINYINT UNSIGNED NOT NULL DEFAULT 0,/*当前教师考评*/
		class_sum	INT(10) UNSIGNED NOT NULL DEFAULT 0,/*已教授的班级数量*/
		course_sum	INT(10) UNSIGNED NOT NULL DEFAULT 0,/*累计上课次数*/
		term_left	INT(10) UNSIGNED NOT NULL DEFAULT 0,	/*本学期剩余课程数*/
		today_sum	INT(10) UNSIGNED NOT NULL DEFAULT 0,/*今日累计课程数*/
		term_sum	INT(10) UNSIGNED NOT NULL DEFAULT 0,/*本学期总的课程数*/
		honor_flag	BINARY(8) NOT NULL DEFAULT 0,/*荣誉标识*/
		class_flag	TINYINT UNSIGNED NOT NULL DEFAULT 0,/*当前上课状态*/
		start_time 	INT(10) UNSIGNED NOT NULL DEFAULT 0,/*当前正在上的课程的开始上课时间*/
		cur_course_id INT(10) UNSIGNED NOT NULL DEFAULT 0,/*当前上课的课程id*/
		PRIMARY KEY (userid)
	) ENGINE=innodb, CHARSET=utf8;
EOF
}

db_index=0
table_index=0

while [ $db_index -lt 100 ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbx
	while [ $table_index -lt 10 ] ; do
		tbx=`printf "%01d" $table_index`
		echo $tbx
		create_user_lamu_classroom_table_sql $tbx
		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx"
		table_index=`expr $table_index + 1`
	done
	table_index=0
	db_index=`expr $db_index + 1`
done
