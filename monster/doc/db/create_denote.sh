#!/bin/sh
host=10.1.1.24
port=3306
user=monster
password=monster@24

for i in `seq 0 9`
do
    for j in `seq 0 99`
    do
        tb_sentence="set names utf8;use db_monster_$i;\
                    create table t_denote_$j(\
			id int not null auto_increment,\
                        user_id int not null,\
                        denote_id int not null default 0,\
                        denote_time   int not null default 0,\
                        denote_num int not null default 0,\
                        reward_flag tinyint not null default 0,\
                        primary key(id),\
			index user_denote_time(user_id, denote_id)\
                    )engine=InnoDB default charset=utf8;"
            mysql -h$host -u$user -p$password -e "$tb_sentence"
    done
done


tb_sentence="set names utf8; use db_monster_config;\
             create table t_denote(\
                     denote_id int not null default 0,\
                     cur_denote_num int not null default 0,\
                     total_denote_num int not null default 0,\
                     cur_denoter int not null default 0,\
                     helped_pet int not null default 0,\
                     reward_id int not null default 0,\
                     end_flag int not null default 0,\
                     primary key(denote_id)
                     )engine=InnoDB default charset=utf8;"

mysql -h$host -u$user -p$password -e "$tb_sentence"

#第一条记录手工插入
tb_sentence="set names utf8; use db_monster_config;\
             insert into t_denote(
                     denote_id, cur_denote_num, total_denote_num, cur_denoter, helped_pet, reward_id, end_flag) values(1, 0, 20000, 0, 1, 10001, 0);"

mysql -h$host -u$user -p$password -e "$tb_sentence"

