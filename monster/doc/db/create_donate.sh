#!/bin/sh
host=10.1.1.24
port=3306
user=monster
password=monster@24

        tb_sentence="set names utf8;use db_monster_config;\
                    create table t_donate_detail(\
						id int not null auto_increment,\
						donate_id int not null default 0,\
                        user_id int not null,\
                        donate int not null default 0,\
                        timestamp   int not null default 0,\
                        is_reward tinyint not null default 0,\
                        primary key(id),\
						index user_denote_time(user_id, donate_id)\
                    )engine=InnoDB default charset=utf8;"
            mysql -h$host -u$user -p$password -e "$tb_sentence"


tb_sentence="set names utf8; use db_monster_config;\
             create table t_donate(\
			 	donate_id int not null default 0,\
                 donate int not null default 0,\
                     helped_pet int not null default 0,\
			 		donor_count int not null default 0,\
                     reach_time int not null default 0,\
                     primary key(donate_id)
                     )engine=InnoDB default charset=utf8;"

mysql -h$host -u$user -p$password -e "$tb_sentence"

#第一条记录手工插入
tb_sentence="set names utf8; use db_monster_config;\
             insert into t_donate(
			 donate_id, donate, helped_pet, donor_count, reach_time) values(1, 0, 0, 0, 0);"

mysql -h$host -u$user -p$password -e "$tb_sentence"

