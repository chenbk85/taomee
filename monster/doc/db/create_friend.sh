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
                    create table t_friend_$j(\
                        user_id int not null,\
                        friend_id int not null,\
                        create_time int not null,\
                        type tinyint(4) not null default 0,\
                        is_best_friend tinyint(4) not null default 0,\
                        remark varchar(512) not null default '',\
                        primary key(user_id, friend_id)
                    )engine=InnoDB default charset=utf8;"
            mysql -h$host -u$user -p$password -e "$tb_sentence"
    done
done

