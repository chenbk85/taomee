#!/bin/sh
host=10.1.1.24
port=3306
user=monster
password=monster@24

tb_sentence="set names utf8;use db_monster_config;\
create table t_encourage(\
	type tinyint(4) not null default 0,\
	user_id int not null default 0,\
	timestamp int not null default 0,\
	score int not null default 0,\
	primary key(type, user_id, timestamp)\
)engine=InnoDB default charset=utf8;"

mysql -h$host -u$user -p$password -e "$tb_sentence"

