#!/bin/sh
host=127.0.0.1
port=3306
user=monster
password=monster@24

        tb_sentence="set names utf8;use db_monster_config;\
                    create table t_name(\
                        name char(16) not null default '',\
                        user_id int not null default 0,\
                        primary key(name, user_id)
                    )engine=InnoDB default charset=utf8;"
            mysql -h$host -u$user -p$password -e "$tb_sentence"

