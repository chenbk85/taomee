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
                    create table t_plant_reward_$j(\
                        user_id int not null,\
                        hole_id tinyint not null default 0,\
                        reward_id int not null default 0,\
                        reward_happy int not null default 0,\
                        reward_exp int not null default 0,\
                        reward_coins int not null default 0,\
                        primary key(user_id, hole_id, reward_id)
                    )engine=InnoDB default charset=utf8;"
            mysql -h$host -u$user -p$password -e "$tb_sentence"
    done
done

