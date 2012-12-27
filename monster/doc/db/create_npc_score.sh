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
                    create table t_npc_score_$j(\
                        user_id int not null,\
                        npc_id int not null default 0,\
                        day_time int not null default 0,\
                        npc_score int not null default 0,\
                        primary key(user_id, npc_id, day_time)
                    )engine=InnoDB default charset=utf8;"
            mysql -h$host -u$user -p$password -e "$tb_sentence"
    done
done

