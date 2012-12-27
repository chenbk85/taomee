#!/usr/bin/perl

#本脚本从指定数据库中抽取出mole数据内容，并生成相应脚本
#因为脚本对给定的数据库一次处理，所以如果同时指定的数据库太多，则生成的中间SQL太复杂，
#使得MYSQL语言分析模块缓存大小不够，所以可以尝试多次运行该脚本来解决
#
#默认情况下，脚本会把命令行解释为数据库名，可以通过下面的参数来改变这个行为
#

use strict;

#适当填写下面的参数，以使得脚本顺利工作
#########################################################################################
#
my $host="localhost";
my $password="ta0mee";
my $user="root";

#指定需要处理的数据库列表，默认从命令行读取
my @db_list=@ARGV;


#以下数据没有需要设置
##########################################################################################
#
#
my $tmp_database="tmp_2009_11_30_xx";
my $view_name ="view_user_attire";


#生成的所有sql语句都放在这个变量里
my $sql_string="";

#创建表的子过程
sub gen_create_table
{
	foreach my $v (@_)
	{
		foreach my $tb ((0..99))
		{
			my $xx = sprintf "%02d", $tb;
			my $sql_create= <<EOF;

			drop table if exists  $v.t_user_pic_book_$xx;
			create table if not exists $v.t_user_pic_book_$xx
			(
				user_id int unsigned not null,
				attire_id int unsigned not null,
				primary key (user_id, attire_id)
			) engine=InnoDB, default charset=utf8;
EOF

			$sql_string .= $sql_create;
		}
	}
}

#产生视图的子过程
sub gen_veiw
{
	my @sql_all;
	my $sql_where="where attireid in (190022, 190027, 190028, 190141, 190142, 190166, 190167, 190186, 190196, 190201, 190202, 190203, 190216, 190228, 190230, 190238, 190239, 190241, 190244, 190245, 190247, 190250, 190252, 190254, 190256, 190257, 190259, 190260, 190261, 190263, 190265, 190267, 190350, 190351, 190352, 190379, 190380, 190388, 190418, 190419, 190425, 190437, 190442, 190458)";

	sub gen_attire_sql
	{
		my $db=shift (@_);
		foreach my $v (@_)
		{
			my $xx = sprintf "%02d", $v;   
			my $sql="select userid, attireid, substr(userid, -2, 2) as db_index, substr(userid, -4, 2) as tb_index from $db.t_user_attire_$xx $sql_where";
			push(@sql_all, $sql);
		}
	}

	sub gen_animal_sql
	{
		my $sql_where="where animalid in (190022, 190027, 190028, 190141, 190142, 190166, 190167, 190186, 190196, 190201, 190202, 190203, 190216, 190228, 190230, 190238, 190239, 190241, 190244, 190245, 190247, 190250, 190252, 190254, 190256, 190257, 190259, 190260, 190261, 190263, 190265, 190267, 190350, 190351, 190352, 190379, 190380, 190388, 190418, 190419, 190425, 190437, 190442, 190458)";
		my $db=shift (@_);
		foreach my $v (@_)
		{
			my $x = sprintf "%d", $v;   
			my $sql="select userid, animalid, substr(userid, -2, 2) as db_index, substr(userid, -4, 2) as tb_index from $db.t_user_animal_$x $sql_where";
			push(@sql_all, $sql);
		}
	}

	sub gen_jy_sql
	{
		my $db=shift (@_);
		foreach my $v (@_)
		{
			my $x = sprintf "%d", $v;   
			my $sql="select userid, attireid, substr(userid, -2, 2) as db_index, substr(userid, -4, 2) as tb_index from $db.t_user_jy_$x $sql_where";
			push(@sql_all, $sql);
		}
	}

	$sql_string .= "create  or replace view $view_name as \n";
	foreach my $v (@_)
	{
		do gen_attire_sql(($v, 0..99));
		do gen_animal_sql(($v, 0..9));
		do gen_jy_sql(($v, 0..9));
	}

	my $num=0;
	foreach my $v (@sql_all)
	{
		$sql_string .= $v."\n";
		if($num!=$#sql_all)
		{
			$sql_string .= "union\n";
		}
		$num++;
	}
	$sql_string .= ";";
}

#新建一个数据库，它是临时的，并且没有实际数据存在
$sql_string .= "create database if not exists $tmp_database; use $tmp_database; ";

#建立相应的表，在指定的数据
do gen_create_table(@db_list);

#在临时的数据里建立一个视图
do gen_veiw(@db_list);

#最终生成的insert语句, 它负责把数据插入
$sql_string .=  <<EOF;

select
	concat("insert into USER_",
		db_index,
		".",
		"t_user_pic_book_",
		tb_index,
		"(user_id, attire_id) values(",
		userid,
		",",
		attireid,
		");"
		) as insert_statement
	from $view_name;
EOF

#删除临时的数据库
$sql_string .= "drop database if exists $tmp_database;";


#查询数据库，从数据库生成脚本
open  H, "| mysql -u $user -h $host --password=$password --skip-column-names";
print H $sql_string;

