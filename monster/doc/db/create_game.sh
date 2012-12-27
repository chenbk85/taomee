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
                    create table t_game_$j(\
                        user_id int not null,\
                        game_id int not null,\
                        level_id int not null,\
                        max_score int not null default 0,\
                        max_star int not null default 0,\
                        is_passed tinyint not null default 0,\
                        primary key(user_id, game_id, level_id)
                    )engine=InnoDB default charset=utf8;"
            mysql -h$host -u$user -p$password -e "$tb_sentence"
    done
done

