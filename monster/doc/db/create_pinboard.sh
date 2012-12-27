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
                    create table t_pinboard_$j(\
                        id int not null auto_increment,\
                        user_id int not null,\
                        peer_id int not null,\
                        message varchar(512) not null default '',\
                        icon tinyint not null,\
                        color int not null,\
                        status tinyint not null default 0,\
                        create_time int not null,\
                        primary key(id)
                    )engine=InnoDB default charset=utf8;"
            mysql -h$host -u$user -p$password -e "$tb_sentence"
    done
done

