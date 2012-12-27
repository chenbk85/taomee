#!/bin/sh
host=10.1.1.24
port=3306
user=monster
password=monster@24
user_id=$1

for i in `seq 0 9`
do
    for j in `seq 0 99`
    do
        tb_sentence="set names utf8;use db_monster_$i;delete from t_role_$j where user_id = $user_id;delete from t_room_$j where user_id = $user_id;delete from t_day_restrict_$j where user_id = $user_id;delete from t_pet_$j where user_id = $user_id;delete from t_pinboard_$j where user_id = $user_id;delete from t_plant_$j where user_id = $user_id;delete from t_puzzle_$j where user_id = $user_id;delete from t_friend_$j where user_id = $user_id;delete from t_stuff_$j where user_id = $user_id;delete from t_badge_$j where user_id = $user_id;delete from t_plant_reward_$j where user_id = $user_id;delete from t_name_$j where user_id = $user_id;delete from t_npc_score_$j where user_id = $user_id; delete from t_game_$j where user_id = $user_id; delete from t_game_changed_$j where user_id = $user_id"
           mysql -h$host -u$user -p$password -e "$tb_sentence"
    done
done


tb_sentence="set names utf8;use db_monster_config;delete from t_name;"
mysql -h$host -u$user -p$password -e "$tb_sentence"
