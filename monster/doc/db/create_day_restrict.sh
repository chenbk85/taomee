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
                    create table t_day_restrict_$j(\
                        user_id int not null,\
                        time int not null default 0,\
                        type int not null default 0,\
                        value int not null default 0,\
                        total_value int not null default 0,\
                        primary key(user_id, time, type)
                    )engine=InnoDB default charset=utf8;"
            mysql -h$host -u$user -p$password -e "$tb_sentence"
    done
done

