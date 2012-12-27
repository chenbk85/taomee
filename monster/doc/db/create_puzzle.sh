#!/bin/sh
host=127.0.0.1
port=3306
user=monster
password=monster@24

for i in `seq 0 9`
do
    for j in `seq 0 99`
    do
        tb_sentence="set names utf8;use db_monster_$i;\
                    create table t_puzzle_$j(\
                        user_id int not null,\
                        type_id tinyint not null default 0,\
                        last_playtime int not null default 0,\
                        max_score int not null default 0,\
                        total_score int not null default 0,\
                        total_num int not null default 0,\
                        primary key(user_id, type_id)
                    )engine=InnoDB default charset=utf8;"
            mysql -h$host -u$user -p$password -e "$tb_sentence"
    done
done

