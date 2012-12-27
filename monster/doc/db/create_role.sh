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
	                create table t_role_$j(\
                        user_id int not null,\
                        name char(16) not null default '',\
                        gender tinyint(4) not null default 0,\
                        country_id int not null default 0,\
                        vip tinyint(4) not null default 0,\
                        birthday int not null default 0,\
                        register_time int not null default 0,\
                        coins int not null default 0,\
                        last_login_time int not null default 0,\
                        last_logout_time int not null default 0,\
                        mon_id  int not null default 0,\
                        mon_name char(16) not null default '',\
                        mon_main_color int not null default 0,\
                        mon_exp_color int not null default 0,\
                        mon_eye_color int not null default 0,\
                        mon_exp int not null default 0,\
                        mon_level tinyint(4) not null default 0,\
                        mon_health int not null default 0,\
                        mon_happy int not null default 0,\
                        fav_color tinyint(4) not null default 0,\
                        fav_fruit tinyint(4) not null default 0,\
                        fav_pet tinyint(4) not null default 0,\
                        mood tinyint(4) not null default 0,\
                        max_puzzle_score tinyint(4) not null default 0,\
                        thumb int not null default 0,\
                        visits int not null default 0,\
                        recent_badge int not null default 0,\
                        drawing_id int not null default 0,\
                        compose_id int not null default 0,\
                        compose_time int not null default 0,\
                        flag1 int not null default 0,\
                        last_visit_plantation_time int not null default 0,\
                        online_time int not null default 0,\
                        offline_time int not null default 0,\
                        npc_score_daytime int not null default 0,\
                        npc_score int not null default 0,\
                        last_paper_reward int not null default 0,\
                        last_paper_read int not null default 0,\
                        personal_sign char(64) not null default '',\
                        remark char(42) not null default '',\
						guide_flag int not null default 0,\
                        primary key(user_id)
                    )engine=InnoDB default charset=utf8;"
            mysql -h$host -u$user -p$password -e "$tb_sentence"
    done
done

