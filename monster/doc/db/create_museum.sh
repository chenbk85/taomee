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
                    create table t_museum_$j(\
                        user_id int not null,\
                        museum_id int not null default 0,\
                        timestamp int not null default 0,\
                        cur_level int not null default 0,\
                        reward_flag tinyint not null default 0,\
                        primary key(user_id, museum_id, timestamp)
                    )engine=InnoDB default charset=utf8;"
            mysql -h$host -u$user -p$password -e "$tb_sentence"
    done
done

