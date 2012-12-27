/*
 * =====================================================================================
 * 
 *       Filename:  dbdeal.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月02日 16时40分57秒 CST
 *        PRIu64
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CROUTE_FUNC_INC
#define  CROUTE_FUNC_INC

#include "Csem.h"
#include "Croute_func_def.h"
#include "Cuser.h"
#include "Cmonth_task.h"
#include "Cappeal.h"
#include "Cappeal_ex.h"
#include "Cuser_jy.h"
#include "Cchristmas.h"
#include "Cuser_card.h"
#include "Cuser_bank.h"
#include "Cuser_task_ex.h"
#include "Cuser_attire.h"
#include "Cuser_class.h"
#include "Cserial.h"
#include "Cemail.h"
#include "Cgame_score.h"
#include "Clogin_info.h"
#include "Cserial.h"
#include "Cmsgboard.h"
#include "Cuser_ip_map.h"
#include "Cregister_serial.h"
#include "Cuser_pet.h"
#include "Cusermsg.h"
#include "Cuser_writing.h"
#include "Creg_userid.h"
#include "Cmsg_attime.h"
#include "Cclientproto.h"
//#include "Cmc.h"
#include "Cdwg_user_friend.h"
#include "Cdwg_user_msg.h"
#include "Cadmin.h"
#include "Cadmin_all.h"
#include "Cchangelist.h"
#include "Cuser_ex.h"
#include "Cdvuser.h"
#include "Cusermap_day.h"
#include "Cuser_connect.h"
#include "Cclass_qa.h"
//#include "Ccup.h"
#include "Cemail_msg.h"
#include "Cuser_email.h"
#include "Csys_arg.h"
#include "Chope_history.h"
#include "Creport_deal_history.h"
#include "Cuser_task.h"
#include "Cuser_info.h"
#include "Cuser_info_ex.h"
#include "Csend_email.h"
#include "Cconfect.h"
#include "Croominfo.h"
#include "Ctempgs.h"
#include "Cquestionnaire.h"
#include "Cgroup_main.h"
#include "Cgroup.h"
#include "Cuser_group.h"
#include "Cmms.h"
#include "Croommsg.h"
#include "Cchat.h"
#include "Cchange_log.h"
#include "Csync_user.h"

#include "Cparty.h"
#include "Cuser_serial.h"
#include "Cadmin_power.h"
#include "Cuser_picture.h"
#include "Cspring.h"
#include "Cuser_diary.h"
#include "Cqa.h"
#include "Cuser_pet_attire.h"
#include "Csports_count.h"
#include "Cpet_sports.h"
#include "Cpet_max_score.h"
#include "Csend_log.h"
#include "Cwriting_day_count.h"
#include "Cchange_passwd.h"
#include "Croominfo_question.h"
#include "Clogin_log.h"
#include "Cuser_classmsg.h"
#include "Croominfo_gen_vip.h"
#include "Cuser_farm.h"
#include "Cuser_farm_animal.h"
#include "Cuser_swap_card.h"
#include "Croominfo_clothe_waste.h"
#include "Croominfo_point.h"
#include "Croominfo_fire_cup.h"
#include "Csys_fire_cup.h"
#include "Cip_history.h"
#include "Cgame.h"
#include "Cuser_game.h"
#include "Cuser_game_pk.h"
#include "Cuser_lamu_diary.h"
#include "Croominfo_work.h"
#include "Cuser_auto.h"
#include "Croominfo_rally.h"
#include "Csys_rally.h"
#include "Croommsg_bibo.h"
#include "Csys_msg.h"
#include "Cuser_npc.h"
#include "Cuser_greenhouse.h"
#include "Cuser_egg.h"
#include "Csys_animal.h"
#include "Cuser_npc_task.h"
#include "Croominfo_cake_gift_swap.h"
#include "Cuser_pic_book.h"
#include "Croominfo_animal_weight.h"
#include "Croominfo_flower.h"
#include "Croominfo_list.h"
#include "Croominfo_stone_move.h"
#include "Cuser_pet_task.h"
#include "Croominfo_client_data.h"
#include "Croominfo_skill_limit.h"
#include "Croominfo_day_limit.h"
#include "Cuser_noah.h"
#include "Cuser_noah_info.h"
#include "Cuser_sn.h"
#include "Cuser_mdog.h"
#include "Cdiningroom.h"
#include "Cuser_diningroom.h"
#include "Cuser_dining_dish.h"
#include "Cuser_dining_employer.h"
#include "Cuser_dining_dish_history.h"
#include "Croominfo_sbh.h"
#include "Croominfo_sports_meet.h"
#include "Csysarg_db_sports_teaminfo.h"
#include "Croominfo_client_buf.h"
#include "Csysarg_db_sports_tmpuser.h"
#include "Csysarg_db_sports_petscore.h"
#include "Csysarg_db_sports_gamescore.h"
#include "Croominfo_sports_pet_info.h"
#include "Csysarg_type_buf.h"
#include "Cuser_new_card.h"
#include "Cuser_dragon_egg.h"
#include "Cuser_dragon.h"
#include "Croominfo_item_sell.h"
#include "Cuser_friend_box.h"
#include "Clamu_classroom.h"
#include "Cuser_lamu_classroom.h"
#include "Cuser_lamu_classroom_lamu.h"
#include "Cuser_lamu_classroom_course.h"
#include "Cuser_lamu_classroom_remembrance.h"
#include "Cuser_lamu_classroom_exam.h"
#include "Cuser_lamu_classroom_score.h"
#include "Cuser_temp_item.h"
#include "Cuser_rank_buf.h"
#include "Csysarg_ranklist.h"
#include "Csysarg_auction.h"
#include "Csysarg_contribute_rank.h"
#include "Csysarg_contribute_total.h"
#include "Csysarg_top_rank.h"
#include "Csysarg_user_rank.h"
#include "Croominfo_shake_rewards_times.h"
#include "Croominfo_consume_money.h"
#include "Croominfo_sock.h"
#include "Croominfo_cake.h"
#include "Croominfo_user_cake.h"
#include "Croominfo_footprint.h"
#include "Croominfo_login_gifts.h"
#include "Croominfo_kakunian.h"
#include "Cuser_farm_fairy.h"
#include "Cuser_farm_speed_tool.h"
#include "Cuser_farm_fairy_skill.h"
#include "Cuser_paradise.h"
#include "Cuser_paradise_angel.h"
#include "Cuser_paradise_contract.h"
#include "Croominfo_login_weekend.h"
#include "Cuser_angel_statistics.h"
#include "Csysarg_paradise_rank.h"
#include "Csysarg_hot_play.h" 
#include "Cuser_angel_battle.h"
#include "Cuser_angel_barrier.h"
#include "Cuser_angel_capture.h"
#include "Cuser_angel_hospital.h"
#include "Cuser_exchange_bean.h"
#include "Cuser_angel_honor.h"
#include "Cuser_dig_treasure.h"
#include "Cuser_dig_treasure_info.h"
#include "Cuser_synth_history.h"
#include "Cuser_treasure_show.h"
#include "Cuser_treasure_collection.h"
#include "Csysarg_treasure_collection.h"
#include "Croominfo_strike_penguin.h"
#include "Cuser_continue_login.h"
#include "Cuser_hide_and_seek.h"
#include "Cuser_cow_milk.h"
#include "Cuser_cow_pasture.h"
#include "Croominfo_random_gain_gift.h"
#include "Cuser_battle_character_attribute.h"
#include "Cuser_battle_skill_equip.h"
#include "Cuser_battle_props_info.h"
#include "Cuser_battle_articles.h"
#include "Cuser_battle_card_booklet.h"
#include "Cuser_battle_friend_event.h"
#include "Cuser_battle_challenge_note.h"
#include "Cuser_battle_pass_barrier.h"
#include "Angel_battle.h"
#include "Cuser_battle_flip_card_info.h"
#include "Cuser_battle_accident.h"
#include "Cuser_battle_fight_record.h"
#include "Cuser_battle_day_task.h"
#include "Cuser_battle_master.h"
#include "Cuser_battle_prentice.h"
#include "Cuser_battle_master_prentice_msg.h"
#include "Cuser_through_time_mail.h"
#include "Cuser_through_time_item.h"
#include "Citem_change_log.h"
#include "Cuser_piglet.h"
#include "Cuser_piglet_house.h"
#include "Cuser_piglet_attire.h"
#include "Cuser_piglet_task.h"
#include "Cuser_piglet_msg.h"
#include "Cuser_piglet_handbook.h"
#include "Cuser_lucky_ticket.h"
#include "Cuser_piglet_effect_tool.h"
#include "Cuser_response_friend.h"
#include "Cuser_halloween_candy.h"
#include "Cuser_profile_dress.h"
#include "Csysarg_beauty_contend.h"
#include "Cuser_as_common.h"
#include "Croominfo_beauty_piglet.h"
#include "Cuser_christmas_socks.h"
#include "Csysarg_christmas_socks.h"
#include "Cuser_vip_months.h"
#include "Cuser_dragon_explore.h"
#include "Csysarg_dragon_explore.h"
#include "Csysarg_love_auction.h"
#include "Csysarg_love_contribute.h"
#include "Cuser_piglet_honor.h"
#include "Csysarg_wish_wall.h"
#include "Cuser_wish_wall.h"
#include "Csysarg_prob_lahm.h"
#include "Csysarg_prob_month_lahm.h"
#include "Csysarg_guess_price.h"
#include "Csysarg_mark_attire.h"
#include "Cuser_mark_attire.h"
#include "Cuser_temp_active.h"
#include "Cuser_piglet_mining.h"
#include "Cuser_piglet_work.h"
#include "Cuser_temp_decorate.h"
#include "Cuser_new_friendship_box.h"
#include "Csysarg_vote_decorate.h"
#include "Cuser_temp_team.h"
#include "Cuser_mvp_team.h"
#include "Csysarg_mvp_team.h"
#include "Csysarg_mvp_day.h"
#include "Csysarg_tw_day_award.h"
#include "Cuser_together_photo.h"
#include "Cuser_shake_dice.h"
#include "Csysarg_cos_rank.h"
#include "Cuser_ocean.h"
#include "Cuser_fish.h"
#include "Cuser_ocean_info.h"
#include "Cuser_ocean_adventure.h"
#include "Cuser_self_games.h"
#include "Csysarg_game_hall.h"
#include "Cuser_ocean_handbook.h"
#include "Csysarg_ocean_info.h"

typedef  struct init_cfg_value
{
	int  xiaomee;
	int  damee;
	int  userflag;
	uint32_t  hope_point;/*有百分几被实现*/
} INIT_CFG_VALUE;

class Croute_func:public Cfunc_route_base < CMD_MAP >
{
private:
int ret;/* */
char *msglog_file;
INIT_CFG_VALUE cfg;

//struct  sockaddr_in change_log_addr;  
//int change_log_fd;  

Csem sem;
//Cmc mc;
mysql_interface *db;
Cclientproto *cp;
Citem_change_log item_change_log;
Cuser user; 
Cuser_ex user_ex; 
Csync_user  sync_user;
Cuser_attire user_attire; 
Cserial serial;
Cemail email;  
Cgame_score game_score;
Cmsgboard  msgboard ;
Cregister_serial register_serial;
Cuser_pet user_pet;
Cusermsg usermsg ;
Cuser_writing user_writing;
Cadmin admin;
Cchangelist changelist;
Cdvuser dvuser;
Cusermap_day usermap_day; 
Cmsg_attime msg_attime; 
Cuser_connect user_connect; 
//	Ccup cup; 
//	Cemail_msg email_msg;
Cuser_email user_email;
Csys_arg sys_arg;
Chope_history hope_history;
Creport_deal_history report_deal_history;
Cuser_task user_task;
Cuser_info user_info;
Cuser_info_ex user_info_ex;
Csend_email send_email;
Cconfect confect;
Croominfo roominfo;
Ctempgs tempgs;
Cquestionnaire questionnaire;
Cgroup_main group_main;
Cgroup group;
Cuser_group user_group;
Cmms  mms;
Croommsg roommsg;
Cchristmas christmas;
Cchat  chat;
Cchange_log change_log;
Cuser_card user_card; 
Cadmin_all admin_all;
Cparty party;
Cuser_serial user_serial;
Cadmin_power admin_power;
Cuser_picture  user_picture;
Cspring  spring;
Cuser_diary user_diary;
Cqa qa;
Cuser_pet_attire user_pet_attire;
Csports_count  sports_count;
Cpet_sports  pet_sports;
Cpet_max_score pet_max_score;
Cuser_bank user_bank;
Cuser_jy user_jy;
Cuser_task_ex user_task_ex;
Cappeal appeal;
Cappeal_ex appeal_ex;
Csend_log send_log;
Cchange_passwd change_passwd;
Croominfo_question roominfo_question;
Cdwg_user_friend dwg_user_friend;
Cdwg_user_msg dwg_user_msg;
Cuser_class  user_class;
Cmonth_task month_task;
Clogin_log login_log;
Cuser_classmsg  user_classmsg;
Croominfo_gen_vip roominfo_gen_vip;
Cclass_qa class_qa;
Cuser_farm user_farm;
Cuser_farm_animal user_farm_animal;
Cuser_swap_card   user_swap_card;
Croominfo_clothe_waste roominfo_clothe_waste;
Croominfo_point roominfo_point;
Croominfo_fire_cup roominfo_fire_cup;
Csys_fire_cup	sys_fire_cup;
Cip_history		ip_history;
Cgame game;
Cuser_game  user_game;
Cuser_game_pk  user_game_pk;
Cuser_lamu_diary user_lamu_diary;
Croominfo_work roominfo_work;
Cuser_auto	user_auto;
Croominfo_rally roominfo_rally;
Csys_rally	sys_rally;
Croommsg_bibo roommsg_bibo;
Csys_msg sys_msg;
Cuser_npc user_npc;
Cuser_greenhouse user_greenhouse;
Cuser_egg user_egg;
Csys_animal sys_animal;
Cuser_npc_task user_npc_task;
Ccake_gift_swap roominfo_cake_gift_swap;
Cuser_pic_book user_pic_book;
Canimal_weight roominfo_animal_weight;
Croominfo_flower roominfo_flower;
Clogin_info login_info;
Cuser_ip_map user_ip_map;
Croominfo_list roominfo_list;
Cwriting_day_count writing_day_count;
Croominfo_stone_move roominfo_stone_move;
Cuser_pet_task	user_pet_task;
Croominfo_client_data roominfo_client_data;
Croominfo_skill_limit roominfo_skill_limit;
Croominfo_day_limit roominfo_day_limit;
Cuser_noah	user_noah;
Cuser_noah_info	user_noah_info;
Cuser_sn	user_sn;
Cuser_mdog	user_mdog;
Cdiningroom diningroom;
Cuser_diningroom user_diningroom;
Cuser_dining_dish user_dining_dish;
Cuser_dining_employer user_dining_employer;
Cuser_dining_dish_history user_dining_dish_history;
Croominfo_sbh roominfo_sbh;
Croominfo_sports_meet roominfo_sports_meet;
Croominfo_client_buf roominfo_client_buf;
Creg_userid reg_userid;
Csysarg_db_sports_teaminfo sysarg_db_sports_teaminfo;
Csysarg_db_sports_tmpuser sysarg_db_sports_tmpuser;
Csysarg_db_sports_petscore sysarg_db_sports_petscore;
Csysarg_db_sports_gamescore sysarg_db_sports_gamescore;
Croominfo_sports_pet_info roominfo_sports_pet_info;
Csysarg_type_buf sysarg_type_buf;
Cuser_new_card user_new_card;
Cuser_dragon user_dragon;
Cuser_dragon_egg user_dragon_egg;
Croominfo_item_sell roominfo_item_sell;
Cuser_friend_box user_friend_box;
Clamu_classroom lamu_classroom;
Cuser_lamu_classroom user_lamu_classroom;
Cuser_lamu_classroom_lamu user_lamu_classroom_lamu;
Cuser_lamu_classroom_course user_lamu_classroom_course;
Cuser_lamu_classroom_remembrance user_lamu_classroom_remembrance; 
Cuser_lamu_classroom_exam user_lamu_classroom_exam;
Cuser_lamu_classroom_score user_lamu_classroom_score;
Cuser_temp_item user_temp_item;
Csysarg_auction sysarg_auction;
Csysarg_contribute_rank sysarg_contribute_rank;
Csysarg_contribute_total sysarg_contribute_total;
Csysarg_ranklist sysarg_ranklist;
Cuser_rank_buf user_rank_buf;
Csysarg_top_rank sysarg_top_rank;
Croominfo_consume_money roominfo_consume_money;
Croominfo_shake_rewards_times roominfo_shake_rewards_times;
Croominfo_sock roominfo_sock;
Croominfo_cake roominfo_cake;
Croominfo_user_cake roominfo_user_cake;
Croominfo_footprint roominfo_footprint;
Croominfo_login_gifts roominfo_login_gifts;
Croominfo_kakunian roominfo_kakunian;

Cuser_farm_fairy user_farm_fairy;
Cuser_farm_speed_tool user_farm_speed_tool;
Cuser_farm_fairy_skill user_farm_fairy_skill;
Cuser_paradise user_paradise;
Cuser_paradise_angel user_paradise_angel;
Cuser_paradise_contract user_paradise_contract;
Croominfo_login_weekend roominfo_login_weekend;
Cuser_angel_statistics user_angel_statistics;
Csysarg_paradise_rank sysarg_paradise_rank;
Csysarg_hot_play sysarg_hot_play;
Cuser_angel_battle user_angel_battle;
Cuser_angel_barrier user_angel_barrier;
Cuser_angel_capture user_angel_capture;
Cuser_angel_hospital user_angel_hospital;
Cuser_exchange_bean user_exchange_bean;
Cuser_angel_honor  user_angel_honor;
Cuser_dig_treasure user_dig_treasure;
Cuser_dig_treasure_info user_dig_treasure_info;
Cuser_synth_history user_synth_history;
Cuser_treasure_show user_treasure_show;
Cuser_treasure_collection user_treasure_collection;
Csysarg_treasure_collection sysarg_treasure_collection;
Croominfo_strike_penguin roominfo_strike_penguin;
Cuser_continue_login user_continue_login;
Cuser_hide_and_seek user_hide_and_seek;
Cuser_milk_cow		user_milk_cow;
Cuser_cow_pasture   user_cow_pasture;
Croominfo_random_gain_gift roominfo_random_gain_gift;

Cuser_battle_character_attribute user_battle_character_attribute;
Cuser_battle_skill_equip   user_battle_skill_equip;
Cuser_battle_props_info  user_battle_props_info;
Cuser_battle_articles user_battle_articles;
Cuser_battle_card_booklet user_battle_card_booklet;
Cuser_battle_friend_event user_battle_friend_event;
Cuser_battle_challenge_note user_battle_challenge_note;
Cuser_battle_pass_barrier user_battle_pass_barrier;
Angel_battle angel_battle;
Cuser_battle_flip_card_info user_battle_flip_card_info;
Cuser_battle_accident user_battle_accident;
Cuser_battle_fight_record user_battle_fight_record;
Cuser_battle_day_task user_battle_day_task;
Cuser_battle_master user_battle_master;
Cuser_battle_prentice user_battle_prentice;
Cuser_battle_master_prentice_msg user_battle_master_prentice_msg;
Cuser_through_time_mail      user_through_time_mail;
Cuser_through_time_item      user_through_time_item;
Cuser_piglet user_piglet;
Cuser_piglet_house user_piglet_house;
Cuser_piglet_attire user_piglet_attire;
Cuser_piglet_task user_piglet_task;
Cuser_piglet_msg user_piglet_msg;
Cuser_piglet_handbook user_piglet_handbook;
Cuser_lucky_ticket user_lucky_ticket;
Cuser_piglet_effect_tool user_piglet_effect_tool;
Cuser_response_friend user_response_friend;
Cuser_halloween_candy user_halloween_candy;
Cuser_profile_dress user_profile_dress;
Csysarg_beauty_contend sysarg_beauty_contend;
Cuser_as_common user_as_common;
Croominfo_beauty_piglet roominfo_beauty_piglet;
Cuser_christmas_socks user_christmas_socks;
Csysarg_christmas_socks sysarg_christmas_socks;
Cuser_vip_months user_vip_months;
Cuser_dragon_explore user_dragon_explore;
Csysarg_dragon_explore sysarg_dragon_explore;
Csysarg_love_contribute sysarg_love_contribute;
Csysarg_love_auction sysarg_love_auction;
Cuser_piglet_honor user_piglet_honor;
Csysarg_wish_wall  sysarg_wish_wall;
Cuser_wish_wall   user_wish_wall;
Csysarg_prob_lahm sysarg_prob_lahm;
Csysarg_prob_month_lahm sysarg_prob_month_lahm;
Csysarg_guess_price sysarg_guess_price;
Csysarg_mark_attire sysarg_mark_attire;
Cuser_mark_attire user_mark_attire;
Cuser_temp_active user_temp_active;
Cuser_piglet_mining user_piglet_mining;
Cuser_piglet_work user_piglet_work;
Cuser_temp_decorate user_temp_decorate;
Cuser_new_friendship_box user_new_friendship_box;
Csysarg_vote_decorate sysarg_vote_decorate;
Cuser_temp_team user_temp_team;
Cuser_mvp_team user_mvp_team;
Csysarg_mvp_team sysarg_mvp_team;
Csysarg_mvp_day sysarg_mvp_day;
Csysarg_tw_day_award sysarg_tw_day_award;
Cuser_together_photo user_together_photo;
Cuser_shake_dice user_shake_dice;
Csysarg_cos_rank sysarg_cos_rank;
Cuser_ocean user_ocean;
Cuser_fish user_fish;
Cuser_ocean_info user_ocean_info;
Cuser_ocean_adventure user_ocean_adventure;
Cuser_self_games user_self_games;
Csysarg_game_hall sysarg_game_hall;
Cuser_ocean_handbook user_ocean_handbook;
Csysarg_ocean_info sysarg_ocean_info;

public:

int	user_diningroom_employer_time(userid_t userid, uint32_t& flag);

int user_del_attire( userid_t userid, user_del_attire_in * p_in, uint32_t is_vip_opt_type = 0);
int user_add_attire(  userid_t userid, user_add_attire_in * p_in , uint32_t is_vip_opt_type = 0);
int user_add_attire_new(  userid_t userid, uint32_t attiretype, uint32_t attireid, uint32_t count, uint32_t maxcount, 
	   	uint32_t is_vip_opt_type = 0);
int user_del_attire_new(  userid_t userid, uint32_t attiretype, uint32_t attireid, uint32_t count, 
		uint32_t maxcount, uint32_t is_vip_opt_type = 0);

int log_change(change_log_item * p_item  );

int do_user_swap_list(userid_t userid,uint32_t reason,uint32_t reason_ex, 
	uint32_t in_item_1_count, user_swap_list_in_item_1 *p_item_1,
 uint32_t in_item_2_count, user_swap_list_in_item_2  *p_item_2);

Croute_func (mysql_interface * db,Cclientproto * cp ); 
int deal(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen );



//-----------------------------------------------------------------------------------------
int user_register(DEAL_FUN_ARG);
int user_get_user_with_msglist(DEAL_FUN_ARG);
int user_get_user(DEAL_FUN_ARG);
int user_set_tasklist(DEAL_FUN_ARG);
int user_set_petcolor(DEAL_FUN_ARG);
int user_set_attire(DEAL_FUN_ARG);
int user_set_user_part(DEAL_FUN_ARG);
int user_get_attire_list(DEAL_FUN_ARG);
int user_set_user_online(DEAL_FUN_ARG);
int user_change_user_value (DEAL_FUN_ARG);

int get_userid_by_email(DEAL_FUN_ARG);
int map_email_userid (DEAL_FUN_ARG);
int del_map_email_userid (DEAL_FUN_ARG);
int user_set_home_attirelist(DEAL_FUN_ARG);
int user_set_jy_attirelist(DEAL_FUN_ARG);
int user_get_user_pet(DEAL_FUN_ARG);
int user_buy_many_thing_in_one_time(DEAL_FUN_ARG);

int user_add_attire(DEAL_FUN_ARG);
int user_get_home ( DEAL_FUN_ARG);
int su_user_get_home(DEAL_FUN_ARG);
int user_get_jy( DEAL_FUN_ARG);
int get_game_score_list(DEAL_FUN_ARG);
int set_game_score(DEAL_FUN_ARG);
int init_game_score_list(DEAL_FUN_ARG );
int set_serial_used (DEAL_FUN_ARG );
int set_serial_unused (DEAL_FUN_ARG );
int serial_get_noused_id(DEAL_FUN_ARG );
int msgboard_get_msglist(DEAL_FUN_ARG);
int msgboard_set_msg_flag(DEAL_FUN_ARG);
int msgboard_set_msg_flag_su(DEAL_FUN_ARG);
int msgboard_add_msg(DEAL_FUN_ARG);
int user_add_offline_msg(DEAL_FUN_ARG);
int su_user_add_offline_msg(DEAL_FUN_ARG);
int user_del_user(DEAL_FUN_ARG);
int user_buy_attire_by_xiaomee(DEAL_FUN_ARG);
int user_set_user_pet_info(DEAL_FUN_ARG);
int get_register_serial(DEAL_FUN_ARG);
int user_set_user_pet_comm(DEAL_FUN_ARG);
int user_add_user_pet(DEAL_FUN_ARG);
int user_del_user_pet(DEAL_FUN_ARG);
int user_set_user_pet_poslist(DEAL_FUN_ARG);
int user_swap_attire_list(DEAL_FUN_ARG);
int user_swap_homeattire_list(DEAL_FUN_ARG);
int usermsg_set_report_msg (DEAL_FUN_ARG);
int user_set_flag_unused( DEAL_FUN_ARG );
int user_get_user_all(DEAL_FUN_ARG);
int user_check_existed(DEAL_FUN_ARG);

int usermsg_add_writing (DEAL_FUN_ARG);
int usermsg_get_writing (DEAL_FUN_ARG);
int usermsg_get_report_by_date(DEAL_FUN_ARG);
int usermsg_get_report_by_date_userid(DEAL_FUN_ARG);
int usermsg_set_del_writing(DEAL_FUN_ARG);
int usermsg_set_writing_report (DEAL_FUN_ARG);
int usermsg_add_report_deal(DEAL_FUN_ARG);
int usermsg_get_deal_report_list(DEAL_FUN_ARG);
int su_login(DEAL_FUN_ARG);
int su_change(DEAL_FUN_ARG);
int su_change_passwd(DEAL_FUN_ARG);
int usermsg_clear_by_date_userid(DEAL_FUN_ARG);

int su_get_msg_attime_by_date(DEAL_FUN_ARG);
int dv_change_passwd(DEAL_FUN_ARG);
int dv_change_nick(DEAL_FUN_ARG);
int dv_register(DEAL_FUN_ARG);

int msgboard_getmsg_by_msgid(DEAL_FUN_ARG);
int msgboard_add_hot(DEAL_FUN_ARG);

int user_set_flag_offline24( DEAL_FUN_ARG );

int msgboard_clear_by_del_flag(DEAL_FUN_ARG);
int msgboard_get_list_with_flag(DEAL_FUN_ARG);
int sysarg_day_add(DEAL_FUN_ARG);
int sysarg_day_query(DEAL_FUN_ARG);
int su_add_msg_attime(DEAL_FUN_ARG);
int su_get_msg_attime_all(DEAL_FUN_ARG);
int su_del_msg_attime(DEAL_FUN_ARG);
int usermsg_clear_writing(DEAL_FUN_ARG);
int usermsg_get_writing_list(DEAL_FUN_ARG);
int su_get_admin_list(DEAL_FUN_ARG);
int su_add_admin(DEAL_FUN_ARG);
int su_del_admin(DEAL_FUN_ARG);
int user_test_proto(DEAL_FUN_ARG);
int su_get_adminid_by_nick(DEAL_FUN_ARG);
int game_score_set_score(DEAL_FUN_ARG );
int user_connect_add_child(DEAL_FUN_ARG);
int email_set_email(DEAL_FUN_ARG);
int user_get_user_email(DEAL_FUN_ARG);
int user_set_home_attire_noused(DEAL_FUN_ARG);
int user_del_attire(DEAL_FUN_ARG);
int user_get_flag(DEAL_FUN_ARG);
int user_set_flag(DEAL_FUN_ARG);
int user_set_attire_max_limit(DEAL_FUN_ARG);

int user_connect_set_parent(DEAL_FUN_ARG);
int user_connect_get_info(DEAL_FUN_ARG);
int usermsg_get_writing_count (DEAL_FUN_ARG);
int user_sale_attire(DEAL_FUN_ARG);
int user_set_user_birthday_sex(DEAL_FUN_ARG);
int user_set_user_pet_all(DEAL_FUN_ARG);
int user_set_attire_ex(DEAL_FUN_ARG);
int user_is_set_user_birthday(DEAL_FUN_ARG);
int serial_set_serial_str_used (DEAL_FUN_ARG );
int serial_get_info_serial_str (DEAL_FUN_ARG );

int emailsys_add_email(DEAL_FUN_ARG);
int emailsys_get_email(DEAL_FUN_ARG);
int emailsys_get_emailid_list(DEAL_FUN_ARG);
int emailsys_del_email(DEAL_FUN_ARG);
int emailsys_set_is_ward(DEAL_FUN_ARG);
int emailsys_get_email_info_list(DEAL_FUN_ARG);
int sysarg_get_count(DEAL_FUN_ARG);
int sysarg_set_count(DEAL_FUN_ARG);
int hope_add_hope(DEAL_FUN_ARG);
int hope_set_hope_used(DEAL_FUN_ARG);
int hope_check_hope(DEAL_FUN_ARG);
int hope_get_hope_list(DEAL_FUN_ARG);
int hope_get_hope_list_by_date(DEAL_FUN_ARG);
int sysarg_get_count_list(DEAL_FUN_ARG);

int user_pet_task_set(DEAL_FUN_ARG);
int user_pet_task_get_list(DEAL_FUN_ARG);
int user_pet_hot_spring(DEAL_FUN_ARG);
int user_pet_skill_set(DEAL_FUN_ARG);
int user_pet_halt_get(DEAL_FUN_ARG);
int user_use_skill_one(DEAL_FUN_ARG);
int user_pet_halt_set(DEAL_FUN_ARG);
int userinfo_get_info_ex(DEAL_FUN_ARG);
int userinfo_set_info_ex(DEAL_FUN_ARG);

int userinfo_del_user(DEAL_FUN_ARG);
int userinfo_change_passwd(DEAL_FUN_ARG);
int userinfo_add_friend(DEAL_FUN_ARG);
int userinfo_del_friend(DEAL_FUN_ARG);
int userinfo_get_friend_list (DEAL_FUN_ARG);
int userinfo_add_blackuser(DEAL_FUN_ARG);
int userinfo_del_blackuser(DEAL_FUN_ARG);
int userinfo_get_blackuser_list(DEAL_FUN_ARG);
int userinfo_get_nick_email(DEAL_FUN_ARG);
int userinfo_change_passwd_nocheck(DEAL_FUN_ARG);
int userinfo_set_user_birthday_sex(DEAL_FUN_ARG);
int userinfo_is_set_user_birthday(DEAL_FUN_ARG);
int userinfo_set_user_email(DEAL_FUN_ARG);
int userinfo_get_user_email(DEAL_FUN_ARG);
int userinfo_get_nick(DEAL_FUN_ARG);
int userinfo_get_info_all(DEAL_FUN_ARG);
int user_class_del(DEAL_FUN_ARG);
int userinfo_set_flag_change_passwd( DEAL_FUN_ARG );
int userinfo_set_payinfo(DEAL_FUN_ARG);
int userinfo_check_existed(DEAL_FUN_ARG);
int userinfo_check_question(DEAL_FUN_ARG);
int userinfo_isset_question(DEAL_FUN_ARG);
int userinfo_get_security_flag(DEAL_FUN_ARG);
int userinfo_set_pemail_by_qa(DEAL_FUN_ARG);
int userinfo_set_pemail_isok(DEAL_FUN_ARG);
int userinfo_get_nick_flist(DEAL_FUN_ARG);

int other_send_email(DEAL_FUN_ARG);
int sysarg_day_clear(DEAL_FUN_ARG);

int confect_change_value(DEAL_FUN_ARG);
int confect_get_value(DEAL_FUN_ARG);
int confect_set_task(DEAL_FUN_ARG);
int user_home_attire_change(DEAL_FUN_ARG);

int msgboard_get_msglist_userid(DEAL_FUN_ARG);
int user_set_used_homeattire(DEAL_FUN_ARG);
int user_set_used_jyattire(DEAL_FUN_ARG);
int msgboard_get_msg_nocheck(DEAL_FUN_ARG);

int user_set_user_pet_skill(DEAL_FUN_ARG);
int emailsys_get_email_msg(DEAL_FUN_ARG);
int userinfo_sync_dv(DEAL_FUN_ARG);
int roominfo_get_hot(DEAL_FUN_ARG);
int roominfo_vote_room(DEAL_FUN_ARG);
int roominfo_pug_add(DEAL_FUN_ARG);
int roominfo_pug_list(DEAL_FUN_ARG);
int roominfo_init_user(DEAL_FUN_ARG);
//question
int roominfo_question_set(DEAL_FUN_ARG);
int roominfo_question_get(DEAL_FUN_ARG);

int roominfo_sports_meet_set(DEAL_FUN_ARG);
int roominfo_sports_meet_teamid_get(DEAL_FUN_ARG);
//int roominfo_set_jy_state(DEAL_FUN_ARG);
//int roominfo_set_jy_fire(DEAL_FUN_ARG);
//int roominfo_jy_fire_get(DEAL_FUN_ARG);

int confect_set_value(DEAL_FUN_ARG);
int roominfo_init_mud(DEAL_FUN_ARG);
int emailsys_clean_email(DEAL_FUN_ARG);
int tempgs_get_value(DEAL_FUN_ARG);
int tempgs_set_count(DEAL_FUN_ARG);
int tempgs_set_task(DEAL_FUN_ARG);
int sysarg_add_questionnaire(DEAL_FUN_ARG);

int groupmain_gen_groupid(DEAL_FUN_ARG);

int group_add_group(DEAL_FUN_ARG);
int group_add_member(DEAL_FUN_ARG);
int group_del_member(DEAL_FUN_ARG);
int group_set_msg(DEAL_FUN_ARG);
int group_get_info(DEAL_FUN_ARG);
int userinfo_get_group(DEAL_FUN_ARG);
int userinfo_join_group(DEAL_FUN_ARG);
int userinfo_leave_group(DEAL_FUN_ARG);
int group_del_group(DEAL_FUN_ARG);
int userinfo_check_friend_existed(DEAL_FUN_ARG);
int group_check_group(DEAL_FUN_ARG);
int userinfo_add_group(DEAL_FUN_ARG);
int userinfo_del_group(DEAL_FUN_ARG);
int group_get_ownerid(DEAL_FUN_ARG);
int group_get_memberlist(DEAL_FUN_ARG);
int mms_get_info(DEAL_FUN_ARG);
int mms_opt(DEAL_FUN_ARG);
int mms_get_optlist(DEAL_FUN_ARG);
int mms_pick_one_fruit(DEAL_FUN_ARG);
int mms_get_type_list(DEAL_FUN_ARG);
int mms_get_type_list_ex(DEAL_FUN_ARG);
int mms_set_water_list(DEAL_FUN_ARG);
int mms_get_water_list(DEAL_FUN_ARG);
int roominfo_del_mud(DEAL_FUN_ARG);

int roommsg_add_msg(DEAL_FUN_ARG);
int roommsg_show_msg(DEAL_FUN_ARG);
int roommsg_list_msg(DEAL_FUN_ARG);
int roommsg_del_msg(DEAL_FUN_ARG);
int roommsg_res_msg(DEAL_FUN_ARG);
int mms_set_value(DEAL_FUN_ARG);
int roominfo_set_info(DEAL_FUN_ARG);
int user_check_attirelist(DEAL_FUN_ARG);
int user_set_group_flag(DEAL_FUN_ARG);
int user_set_birthday(DEAL_FUN_ARG);
int user_isset_birthday(DEAL_FUN_ARG);
int userinfo_check_paypasswd(DEAL_FUN_ARG);
int userinfo_set_paypasswd(DEAL_FUN_ARG);
int userinfo_isset_paypasswd(DEAL_FUN_ARG);
int userinfo_set_qa(DEAL_FUN_ARG);
int christmas_add_msg(DEAL_FUN_ARG);
int christmas_get_msg(DEAL_FUN_ARG);
int christmas_is_get_msg(DEAL_FUN_ARG);
int christmas_is_add_msg(DEAL_FUN_ARG);
int userinfo_clean_paypasswd(DEAL_FUN_ARG);
int userinfo_get_question(DEAL_FUN_ARG);
int userinfo_set_paypasswd_with_check(DEAL_FUN_ARG);
int mms_get_info_by_type(DEAL_FUN_ARG);

int passwd_add_change(DEAL_FUN_ARG);
int log_add_chat(DEAL_FUN_ARG);
int log_add_change(DEAL_FUN_ARG);
int user_card_add_card(DEAL_FUN_ARG);
int user_card_add_win_lose(DEAL_FUN_ARG);
int user_card_getinfo(DEAL_FUN_ARG);

int admin_add_admin(DEAL_FUN_ARG);
int admin_get_adminlist(DEAL_FUN_ARG);
int admin_change_passwd(DEAL_FUN_ARG);
int admin_del_admin(DEAL_FUN_ARG);
int admin_login(DEAL_FUN_ARG);
int party_add_party(DEAL_FUN_ARG);
int party_get_partylist(DEAL_FUN_ARG);
int party_getlist_by_ownerid(DEAL_FUN_ARG);

int userserial_gen_serial(DEAL_FUN_ARG);
int userserial_del_serial(DEAL_FUN_ARG);
int userserial_get_serial(DEAL_FUN_ARG);

int userinfo_set_passwdemail(DEAL_FUN_ARG);
int userinfo_isset_passwdemail(DEAL_FUN_ARG);
int userinfo_isconfirm_passwdemail(DEAL_FUN_ARG);
int userinfo_confirm_passwdemail(DEAL_FUN_ARG);
int userinfo_get_passwdemail(DEAL_FUN_ARG);
int admin_add_power(DEAL_FUN_ARG);
int admin_del_power(DEAL_FUN_ARG);
int party_get_count_by_server(DEAL_FUN_ARG);
int admin_get_powerlist(DEAL_FUN_ARG);
int admin_root_change_passwd(DEAL_FUN_ARG);
int user_card_add_lower_card(DEAL_FUN_ARG);
int user_card_init(DEAL_FUN_ARG);
int user_card_is_inited(DEAL_FUN_ARG);
int user_card_get_enable_count(DEAL_FUN_ARG);
int pic_add_pic(DEAL_FUN_ARG);
int pic_get_user_ablum_piclist(DEAL_FUN_ARG);
int pic_get_pic_by_photoid_userid(DEAL_FUN_ARG);
int pic_del_pic_by_photoid_userid(DEAL_FUN_ARG);
int sysarg_day_clean_by_type(DEAL_FUN_ARG);
int msgboard_reset_msg(DEAL_FUN_ARG);
int spring_get_value(DEAL_FUN_ARG);
int spring_change_value(DEAL_FUN_ARG);
int spring_set_msg(DEAL_FUN_ARG);
int admin_set_use_flag(DEAL_FUN_ARG);
int user_card_set_win_lose(DEAL_FUN_ARG);
int spring_get_msg(DEAL_FUN_ARG);
int spring_init_gold_silver(DEAL_FUN_ARG);
int spring_get_flag(DEAL_FUN_ARG);
int spring_get_spring(DEAL_FUN_ARG);
int spring_set_spring(DEAL_FUN_ARG);
int roommsg_add_diary(DEAL_FUN_ARG);
int roommsg_set_diary(DEAL_FUN_ARG);
int roommsg_set_diary_pub_flag(DEAL_FUN_ARG);
int roommsg_get_diary_list(DEAL_FUN_ARG);
int roommsg_get_diary(DEAL_FUN_ARG);
int admin_get_adminid_by_nick(DEAL_FUN_ARG);
int admin_get_adminlist_by_powerid(DEAL_FUN_ARG);
int msgboard_qa_add_msg(DEAL_FUN_ARG);
int msgboard_qa_get_list(DEAL_FUN_ARG);
int msgboard_qa_del_msg(DEAL_FUN_ARG);
int user_temp_add_value(DEAL_FUN_ARG);
int msgboard_qa_get_list_ex(DEAL_FUN_ARG);
int msgboard_qa_get_count(DEAL_FUN_ARG);
int user_buy_pet_attire_by_xiaomee(DEAL_FUN_ARG);
int user_get_pet_attire_list(DEAL_FUN_ARG);
int user_set_pet_attire(DEAL_FUN_ARG);
int user_temp_get_value(DEAL_FUN_ARG);
int temp_pet_sports_get_list(DEAL_FUN_ARG);
int user_get_pet_attire_used_all(DEAL_FUN_ARG);
int temp_pet_user_reported(DEAL_FUN_ARG);
int temp_pet_get_pet_list(DEAL_FUN_ARG);
int temp_pet_report(DEAL_FUN_ARG);
int user_get_pet_attire_all(DEAL_FUN_ARG);
int user_pet_attire_update_ex(DEAL_FUN_ARG);
int user_pet_attire_set_used(DEAL_FUN_ARG);
int user_card_getinfo_ex(DEAL_FUN_ARG);
int temp_pet_get_score_list(DEAL_FUN_ARG);
int temp_pet_sports_set_score(DEAL_FUN_ARG);
int hope_get_hope_all(DEAL_FUN_ARG);
int temp_pet_sports_get_max_score(DEAL_FUN_ARG);
int user_register_ex(DEAL_FUN_ARG);
int temp_pet_get_group_allcount(DEAL_FUN_ARG);
int user_get_info_ex(DEAL_FUN_ARG);

int user_bank_add(DEAL_FUN_ARG);
int user_bank_get_account(DEAL_FUN_ARG);
int user_bank_delete_account(DEAL_FUN_ARG);
int user_swap_jyattire_list(DEAL_FUN_ARG);
int user_bank_set_account(DEAL_FUN_ARG);
int user_set_task(DEAL_FUN_ARG);
int userinfo_login_ex(DEAL_FUN_ARG);
int userinfo_log_with_stat(DEAL_FUN_ARG);
int user_get_jy_ex(DEAL_FUN_ARG);
int user_add_seed(DEAL_FUN_ARG);
int user_plant_growth_add_by_hour(DEAL_FUN_ARG);
int user_del_seed(DEAL_FUN_ARG);
int user_water_seed(DEAL_FUN_ARG);
int user_kill_bug(DEAL_FUN_ARG);
int user_get_seed(DEAL_FUN_ARG);
int user_catch_fruit(DEAL_FUN_ARG);
int user_get_jy_noused_list(DEAL_FUN_ARG);
int userinfo_register_ex(DEAL_FUN_ARG);
int user_get_seed_list(DEAL_FUN_ARG);
int user_set_seed_ex(DEAL_FUN_ARG);
int user_get_access_jylist(DEAL_FUN_ARG);
int user_access_jy(DEAL_FUN_ARG);
int userinfo_get_gameflag(DEAL_FUN_ARG);
int userinfo_add_game(DEAL_FUN_ARG);
int userinfo_del_game(DEAL_FUN_ARG);
int userinfo_get_meefan_gameflag(DEAL_FUN_ARG);
int user_login_ex(DEAL_FUN_ARG);
int user_register_ex2(DEAL_FUN_ARG);
int user_home_noused_attire_type_count(DEAL_FUN_ARG);
int user_check_existed_ex(DEAL_FUN_ARG);
int user_add_friend(DEAL_FUN_ARG);
int user_del_friend(DEAL_FUN_ARG);
int user_batch_del_friends(DEAL_FUN_ARG);
int user_vip_send_gift(userid_t userid, uint32_t vip_level);
int user_set_vip_flag( DEAL_FUN_ARG );
int user_vip_mark_last_set( DEAL_FUN_ARG );
int user_check_friend_existed(DEAL_FUN_ARG);
int user_add_blackuser(DEAL_FUN_ARG);
int user_del_blackuser(DEAL_FUN_ARG);
int user_get_blackuser_list(DEAL_FUN_ARG);
int user_get_nick_flist(DEAL_FUN_ARG);
int user_get_group(DEAL_FUN_ARG);
int user_join_group(DEAL_FUN_ARG);
int user_leave_group(DEAL_FUN_ARG);
int user_del_group(DEAL_FUN_ARG);
int user_set_nick(DEAL_FUN_ARG);
int user_get_nick(DEAL_FUN_ARG);
int user_vip_pet_level_set(DEAL_FUN_ARG);

//user_task_ex
int user_task_set(DEAL_FUN_ARG);
int user_task_del(DEAL_FUN_ARG);
int user_task_get(DEAL_FUN_ARG);

int user_set_tempbuf(DEAL_FUN_ARG);
int user_get_tempbuf(DEAL_FUN_ARG);

int user_get_ip_history(DEAL_FUN_ARG);
int user_get_attire_count(DEAL_FUN_ARG);
int userinfo_get_passwd_history(DEAL_FUN_ARG);
int roominfo_add_hot(DEAL_FUN_ARG);
int user_get_xiaomee(DEAL_FUN_ARG);
int user_get_vip_nick(DEAL_FUN_ARG);
//pet
int user_pet_set_nick(DEAL_FUN_ARG);
int user_pet_set_flag(DEAL_FUN_ARG);
int user_pet_set_flag2(DEAL_FUN_ARG);
int user_pet_set_birthday(DEAL_FUN_ARG);
int user_pet_set_color(DEAL_FUN_ARG);
int user_pet_set_sicktime(DEAL_FUN_ARG);
int user_pet_set_endtime(DEAL_FUN_ARG);
int user_pet_set_life(DEAL_FUN_ARG);
int user_pet_feed_pet(DEAL_FUN_ARG);
int user_pet_retrieve_pet(DEAL_FUN_ARG);
int user_pet_get_pet(DEAL_FUN_ARG);
int user_pet_travel_pet(DEAL_FUN_ARG);

int appeal_add_appeal(DEAL_FUN_ARG);
int appeal_ex_add_appeal(DEAL_FUN_ARG);
int appeal_get_list(DEAL_FUN_ARG);
int appeal_get_appeal(DEAL_FUN_ARG);

int hope_set_hope_ex(DEAL_FUN_ARG);
int user_swap_list(DEAL_FUN_ARG);
int su_user_swap_list(DEAL_FUN_ARG);
int userinfo_su_set_passwd(DEAL_FUN_ARG);
int userinfo_su_set_paypasswd(DEAL_FUN_ARG);
int appeal_deal(DEAL_FUN_ARG);
int userinfo_get_user_enable_flag(DEAL_FUN_ARG);
int user_pet_get_info(DEAL_FUN_ARG);
int roominfo_box_add(DEAL_FUN_ARG);
int roominfo_box_get_list(DEAL_FUN_ARG);
int user_get_friend_list(DEAL_FUN_ARG);
int user_jy_access_set_opt(DEAL_FUN_ARG);
int dwg_sendmsg_to_friends(DEAL_FUN_ARG);
int user_profession_get(DEAL_FUN_ARG);
int user_profession_set(DEAL_FUN_ARG);
int sysarg_get_questionnaire_list(DEAL_FUN_ARG);
int sysarg_day_get_list_by_userid(DEAL_FUN_ARG);
int sysarg_day_set(DEAL_FUN_ARG);
int user_set_passwdemail_ex(DEAL_FUN_ARG);
int su_changelist_add(DEAL_FUN_ARG);
int user_pet_set_skill_flag(DEAL_FUN_ARG);
int user_pet_set_change_value(DEAL_FUN_ARG);

//user_ex
int user_set_create_classid(DEAL_FUN_ARG);
int user_get_create_classid(DEAL_FUN_ARG);
int user_add_classlist_member(DEAL_FUN_ARG);
int user_del_classlist_member(DEAL_FUN_ARG);
int roominfo_presentlist_add(DEAL_FUN_ARG);
int user_class_create(DEAL_FUN_ARG);
int user_class_change_info(DEAL_FUN_ARG);
int user_class_get_info(DEAL_FUN_ARG);
int su_user_class_get_info(DEAL_FUN_ARG);
int roominfo_presentlist_get(DEAL_FUN_ARG);
int user_class_add_member(DEAL_FUN_ARG);
int user_class_del_member(DEAL_FUN_ARG);
int user_class_add_attire(DEAL_FUN_ARG);
int user_add_class(DEAL_FUN_ARG);
int user_del_class(DEAL_FUN_ARG);
int user_class_get_flag_info(DEAL_FUN_ARG);
int user_class_edit_home(DEAL_FUN_ARG);
int user_class_set_access_flag(DEAL_FUN_ARG);
int user_change_xiaomee(DEAL_FUN_ARG);
int sysarg_month_task_get_list(DEAL_FUN_ARG);
int sysarg_month_task_add(DEAL_FUN_ARG);
int user_set_first_class(DEAL_FUN_ARG);

int ff_login_add(DEAL_FUN_ARG);

int user_class_get_attirelist(DEAL_FUN_ARG);
int user_classmsg_add(DEAL_FUN_ARG);
int user_classmsg_set_show_flag(DEAL_FUN_ARG);
int user_classmsg_get_list(DEAL_FUN_ARG);
int user_classmsg_del(DEAL_FUN_ARG);
int user_classmsg_report(DEAL_FUN_ARG);
int user_class_reduce_attire(DEAL_FUN_ARG);
int user_class_get_simple_info(DEAL_FUN_ARG);
int user_get_first_class(DEAL_FUN_ARG);
int user_class_get_memberlist(DEAL_FUN_ARG);
int user_get_classlist(DEAL_FUN_ARG);
int user_thieve_fruit(DEAL_FUN_ARG);

int croominfo_gen_vip(DEAL_FUN_ARG);
int roominfo_check_if_gen_vip(DEAL_FUN_ARG);
int roominfo_class_qa_add_score(DEAL_FUN_ARG);
int roominfo_class_qa_check_add(DEAL_FUN_ARG);
int user_class_get_random_top(DEAL_FUN_ARG);
int roominfo_class_qa_get_info(DEAL_FUN_ARG);
//farm
int user_farm_get(DEAL_FUN_ARG);
int user_get_feedstuff(DEAL_FUN_ARG);
int user_del_animal(DEAL_FUN_ARG);
int user_farm_feedstuff_add(DEAL_FUN_ARG);
int user_farm_put_animal(DEAL_FUN_ARG);
int user_farm_get_fish(DEAL_FUN_ARG);
int user_farm_water(DEAL_FUN_ARG);
int user_farm_set_accestlist(DEAL_FUN_ARG);
int roominfo_gen_vip_del(DEAL_FUN_ARG);
int user_card_set_flag_bit(DEAL_FUN_ARG);
int user_connect_set_childcount(DEAL_FUN_ARG);
int user_farm_edit(DEAL_FUN_ARG);
int user_get_farm(DEAL_FUN_ARG);
int user_get_farm_inner(userid_t userid, user_get_farm_out_item_1 *p_list,
	uint32_t index, uint32_t &output);
int user_get_used_attierlist(DEAL_FUN_ARG);
int user_farm_get_accestlist(DEAL_FUN_ARG);
int user_farm_get_all_animal_info(DEAL_FUN_ARG);
int user_farm_update_animal_info(DEAL_FUN_ARG);
int user_farm_get_nousedlist(DEAL_FUN_ARG);
int user_farm_set_state(DEAL_FUN_ARG);
//card
int user_swap_card_search(DEAL_FUN_ARG);
int user_swap_card_set_swap(DEAL_FUN_ARG);
int user_swap_card_swap(DEAL_FUN_ARG);
int user_swap_card_get_super(DEAL_FUN_ARG);
int user_swap_card_get_card(DEAL_FUN_ARG);
int user_farm_animal_add(DEAL_FUN_ARG);
int user_farm_animail_del_ex(DEAL_FUN_ARG);
int user_task_get_list(DEAL_FUN_ARG);
int user_swap_card_set_card(DEAL_FUN_ARG);
//clothe waste
int roominfo_get_clothe_waste(DEAL_FUN_ARG);
int roominfo_set_clothe_waste(DEAL_FUN_ARG);
//point
int roominfo_get_point(DEAL_FUN_ARG);
int roominfo_set_point(DEAL_FUN_ARG);
int roominfo_get_pos(DEAL_FUN_ARG);
int roominfo_set_pos(DEAL_FUN_ARG);
int roominfo_clear_pos(DEAL_FUN_ARG);
int user_set_xiaomee_max_info(DEAL_FUN_ARG);
int roominfo_set_class_medal_flag(DEAL_FUN_ARG);
int user_class_get_medal_list(DEAL_FUN_ARG);
int user_class_set_medal_list(DEAL_FUN_ARG);
int user_set_punish_flag(DEAL_FUN_ARG);
int user_set_pet_sick_type(DEAL_FUN_ARG);

int do_user_farm_feedstuff_add( userid_t userid , user_farm_feedstuff_add_in *p_in,
	stru_user_farm_feedstuff_add_out_buf * p_out    );

int user_class_set_medal_info(DEAL_FUN_ARG);
int user_swap_card_add_clothe(userid_t userid);
//fire cup
int roominfo_sig_fire_cup(DEAL_FUN_ARG);
int roominfo_get_fire_cup_team(DEAL_FUN_ARG);
int roominfo_fire_cup_get(DEAL_FUN_ARG);
int roominfo_fire_cup_update(DEAL_FUN_ARG);
int sys_add_team_medal(DEAL_FUN_ARG);
int sys_get_all_medal_list(DEAL_FUN_ARG);
int sysarg_get_team(DEAL_FUN_ARG);
int sysarg_get_fire_cup(DEAL_FUN_ARG);
int roominfo_get_fire_cup_prize(DEAL_FUN_ARG);

int roominfo_set_task(DEAL_FUN_ARG);
int roominfo_get_task(DEAL_FUN_ARG);
int history_ip_record(DEAL_FUN_ARG);
int game_set_score(DEAL_FUN_ARG);
int game_user_get_list(DEAL_FUN_ARG);
int game_set_pk_info(DEAL_FUN_ARG);
int game_get_list(DEAL_FUN_ARG);
int roominfo_pk_add(DEAL_FUN_ARG);
int roominfo_pk_get_list(DEAL_FUN_ARG);
int game_get_pk_info(DEAL_FUN_ARG);
int user_update_game_score(DEAL_FUN_ARG);
int user_lamu_diary_edit(DEAL_FUN_ARG);
int user_lamu_catalog_get(DEAL_FUN_ARG);
int user_lamu_diary_delete(DEAL_FUN_ARG);
int user_lamu_diary_state_update(DEAL_FUN_ARG);
int user_lamu_diary_flower_update(DEAL_FUN_ARG);
int user_lamu_diary_content_get(DEAL_FUN_ARG);
int roominfo_work_set(DEAL_FUN_ARG);
int roominfo_work_get(DEAL_FUN_ARG);
int user_ex_set_diary_lock(DEAL_FUN_ARG);
int user_ex_diary_lock_get(DEAL_FUN_ARG);
int user_lamu_diary_count(DEAL_FUN_ARG);
int user_set_work_level(DEAL_FUN_ARG);
int user_pet_task_set_ex(DEAL_FUN_ARG);
int user_task_pet_first_stage_set(DEAL_FUN_ARG);
int user_reset_home(DEAL_FUN_ARG);
int user_task_taskid_del(DEAL_FUN_ARG);
int user_task_set_client_data(DEAL_FUN_ARG);
int user_task_get_client_data(DEAL_FUN_ARG);
int user_task_get_pet_task(DEAL_FUN_ARG);
int user_attire_put_chest(DEAL_FUN_ARG);
int user_attire_del_chest(DEAL_FUN_ARG);
int user_attire_get_chest_list(DEAL_FUN_ARG);
int roominfo_change_present(DEAL_FUN_ARG);
int roominfo_get_present_num(DEAL_FUN_ARG);
int user_magic_task_pet_get_all(DEAL_FUN_ARG);
int user_magic_task_pet_update(DEAL_FUN_ARG);
int user_attire_get_list_ex(DEAL_FUN_ARG);
int user_get_attire_list_all(DEAL_FUN_ARG);
int user_farm_get_sheep_num(DEAL_FUN_ARG);
int pp_set_vip(DEAL_FUN_ARG);
int user_auto_insert(DEAL_FUN_ARG);
int do_user_auto_insert(userid_t userid, user_auto_insert_in *p_in,
user_auto_insert_out *p_out, int *p_temp);
int user_auto_change_attire(DEAL_FUN_ARG);
int user_auto_change_propery(DEAL_FUN_ARG);
int user_auto_get_all(DEAL_FUN_ARG);
int user_auto_add_oil(DEAL_FUN_ARG);
int do_user_auto_add_oil(userid_t userid, user_auto_add_oil_in *p_in, 
user_auto_add_oil_out *p_out, int *p_temp);
int user_auto_get_one_info(DEAL_FUN_ARG);
int user_ex_set_driver_time(DEAL_FUN_ARG);
int user_ex_set_show_auto(DEAL_FUN_ARG);
int user_ex_get_show_auto(DEAL_FUN_ARG);
int user_ex_set_auto_skill(DEAL_FUN_ARG);
int user_auto_change_adron(DEAL_FUN_ARG);
int user_class_add_score(DEAL_FUN_ARG);
int user_class_get_score(DEAL_FUN_ARG);
int roominfo_add_candy(DEAL_FUN_ARG);
int roominfo_get_candy(DEAL_FUN_ARG);
int user_info_get_set_passwd_flag(DEAL_FUN_ARG);
int roominfo_add_class_score(DEAL_FUN_ARG);
int roominfo_get_class_score(DEAL_FUN_ARG);
int roominfo_get_score_candy(DEAL_FUN_ARG);
int user_farm_set_attirelist(DEAL_FUN_ARG);
int roominfo_add_catch_candy(DEAL_FUN_ARG);
int roominfo_rally_sign(DEAL_FUN_ARG);
int roominfo_rally_add_score(DEAL_FUN_ARG);
int roominfo_rally_get_score(DEAL_FUN_ARG);
int sys_rally_update_score(DEAL_FUN_ARG);
int sys_rally_update_race(DEAL_FUN_ARG);
int sys_get_team_race_list(DEAL_FUN_ARG);
int sys_get_team_score_list(DEAL_FUN_ARG);
int roominfo_rally_get_race_info(DEAL_FUN_ARG);
int roominfo_rally_get_today_score(DEAL_FUN_ARG);
int roominfo_rally_desc_race(DEAL_FUN_ARG);
int roominfo_rally_get_team(DEAL_FUN_ARG);
int user_auto_get_one_kind_num(DEAL_FUN_ARG);
int roominfo_rally_add_medal(DEAL_FUN_ARG);
int user_farm_set_outgo(DEAL_FUN_ARG);
int user_farm_set_eattime(DEAL_FUN_ARG);
int user_set_int_value(DEAL_FUN_ARG);
int user_auto_del(DEAL_FUN_ARG);
int user_attire_get_some_attire_count(DEAL_FUN_ARG);
int user_auto_get_all_web(DEAL_FUN_ARG);
int user_auto_set_auto_web(DEAL_FUN_ARG);
int roominfo_get_merge(DEAL_FUN_ARG);
int roominfo_merge_update(DEAL_FUN_ARG);
int roominfo_merge_set_get(DEAL_FUN_ARG);
int userinfo_log_for_blw(DEAL_FUN_ARG);
int user_attire_set_used(DEAL_FUN_ARG);
int roominfo_check_candy(DEAL_FUN_ARG);
int roominfo_rally_get_user_all_info_web(DEAL_FUN_ARG);
int user_farm_net_catch_fish(DEAL_FUN_ARG);
int roominfo_get_user_all_info_web(DEAL_FUN_ARG);
int user_farm_set_net(DEAL_FUN_ARG);
int user_farm_get_net(DEAL_FUN_ARG);
int roominfo_change_candy_ex(DEAL_FUN_ARG);
int roominfo_change_candy(DEAL_FUN_ARG);
int roominfo_set_user_all_info_web(DEAL_FUN_ARG);
int roominfo_check_candy_list(DEAL_FUN_ARG);
int do_user_farm_net_catch_fish(userid_t userid, user_farm_net_catch_fish_out_header *p_out_header, 
user_farm_net_catch_fish_out_item **pp_out_item);
int roommsg_insert_bibo(DEAL_FUN_ARG);
int roommsg_del_bibo(DEAL_FUN_ARG);
int roommsg_get_bibo_list(DEAL_FUN_ARG);
int sysarg_db_msg_edit(DEAL_FUN_ARG);
int sysarg_db_msg_del(DEAL_FUN_ARG);
int sysarg_db_msg_get(DEAL_FUN_ARG);
int sysarg_db_msg_get_all_web(DEAL_FUN_ARG);
int email_check_email_is_existed(DEAL_FUN_ARG);
int user_jy_add_exp(DEAL_FUN_ARG);
int user_npc_change(DEAL_FUN_ARG);
int user_farm_get_web(DEAL_FUN_ARG);
int user_farm_set_web(DEAL_FUN_ARG);
int user_npc_get_web(DEAL_FUN_ARG);
int user_npc_update_web(DEAL_FUN_ARG);
int user_farm_rabbit_foster(DEAL_FUN_ARG);
int user_farm_rabbit_del(DEAL_FUN_ARG);
int sysarg_day_get_list(DEAL_FUN_ARG);
int user_farm_get_plant_breed(DEAL_FUN_ARG);

//-----------------------------------------------------------------------------------------
//孵蛋相关的命令
int user_egg_get_all_web(DEAL_FUN_ARG);
int user_egg_update_web(DEAL_FUN_ARG);
int user_egg_insert(DEAL_FUN_ARG);
int user_egg_fudan(DEAL_FUN_ARG);
int user_egg_get_info(DEAL_FUN_ARG);
int user_egg_brood_egg(DEAL_FUN_ARG);
int roominfo_pic_insert(DEAL_FUN_ARG);
int user_farm_animal_get_wool(DEAL_FUN_ARG);
int sysarg_db_update_animal_limit(DEAL_FUN_ARG);
int sysarg_get_animal_limit(DEAL_FUN_ARG);
int user_npc_task_recv(DEAL_FUN_ARG);
int user_npc_task_finish(DEAL_FUN_ARG);
int user_npc_task_get_buf(DEAL_FUN_ARG);
int user_npc_task_set_buf(DEAL_FUN_ARG);
int user_npc_one_task_get_info(DEAL_FUN_ARG);
int user_npc_task_get_npc_task(DEAL_FUN_ARG);
int user_npc_task_delete_npc_task(DEAL_FUN_ARG);

//-----------------------------------------------------------------------------------------
//暖房相关的命令, 功能定义见 ${workdir}/include/proto.h
int user_greenhouse_insert(DEAL_FUN_ARG);
int user_greenhouse_drop(DEAL_FUN_ARG);
int user_greenhouse_remain_time(DEAL_FUN_ARG);
int user_greenhouse_user_exist(DEAL_FUN_ARG);
int user_greenhouse_user_all_animal(DEAL_FUN_ARG);
int user_ex_update_pic_clothe(DEAL_FUN_ARG);
int user_ex_get_pic_clothe(DEAL_FUN_ARG);

//-----------------------------------------------------------------------------------------
//服装打分
int roominfo_roominfo_dressing_mark_get(DEAL_FUN_ARG);
int roominfo_roominfo_dressing_mark_set(DEAL_FUN_ARG);
int user_mode_level_set(DEAL_FUN_ARG);

//-----------------------------------------------------------------------------------------
//鸭子状态
int roominfo_roominfo_duck_statu_get(DEAL_FUN_ARG);
int roominfo_roominfo_duck_statu_set(DEAL_FUN_ARG);
int user_attire_set_mode_attire(DEAL_FUN_ARG);
int user_attire_get_mode_attire(DEAL_FUN_ARG);
int user_attire_set_mode_to_mole(DEAL_FUN_ARG);
int user_attire_set_clothe_mole_to_mode(DEAL_FUN_ARG);

//----------------------------------------------------------------------------------------------------
//蛋糕交换礼品
int roominfo_cake_gift_swap_get_cake_num(DEAL_FUN_ARG);
int roominfo_cake_gift_swap_do_swap(DEAL_FUN_ARG);
int user_home_add_mode(DEAL_FUN_ARG);
int user_pic_book_update(DEAL_FUN_ARG);
int user_pic_book_get(DEAL_FUN_ARG);

//---------------------------------------------------------------------------------------------
//钓鱼，鱼的重量
int roominfo_fish_weight_get(DEAL_FUN_ARG);
int roominfo_fish_weight_set(DEAL_FUN_ARG);
int roominfo_flower_update(DEAL_FUN_ARG);
int roominfo_flower_get(DEAL_FUN_ARG);
int sysarg_animal_get_max_weight_fish(DEAL_FUN_ARG);
int user_farm_set_col(DEAL_FUN_ARG);
int roominfo_get_sockes(DEAL_FUN_ARG);
int ff_login_get_list(DEAL_FUN_ARG);
int user_animal_release_insect(DEAL_FUN_ARG);
int ip_history_get_lastip(DEAL_FUN_ARG);
int user_animal_set_animal_flag(DEAL_FUN_ARG);
int appeal_ex_deal(DEAL_FUN_ARG);
int appeal_ex_get_state(DEAL_FUN_ARG);
int appeal_ex_get_list(DEAL_FUN_ARG);
int user_jy_butterfly_pollinate_flower(DEAL_FUN_ARG);
int user_animal_pollinate(DEAL_FUN_ARG);
int roominfo_update_wish(DEAL_FUN_ARG);
int roominfo_get_wish(DEAL_FUN_ARG);
int roominfo_change_gift_num(DEAL_FUN_ARG);
int roominfo_get_gift_num(DEAL_FUN_ARG);
int roominfo_record_give_gift_userid(DEAL_FUN_ARG);
int roominfo_get_give_gift_userid(DEAL_FUN_ARG);
int userinfo_get_login_info(DEAL_FUN_ARG);
int roominfo_add_energy_star(DEAL_FUN_ARG);
int roominfo_get_energy_star(DEAL_FUN_ARG);
int user_attire_delete_attire_list(DEAL_FUN_ARG);
int user_get_vip_month(DEAL_FUN_ARG);

//--------------------------------------------------------------------------------------------
//新年相片服装
int user_dress_in_photo_set(DEAL_FUN_ARG);
int user_dress_in_photo_get(DEAL_FUN_ARG);
int appeal_ex_get_user_last_state(DEAL_FUN_ARG);
int user_info_set_falg_change_paypasswd(DEAL_FUN_ARG);
int user_info_change_paypasswd_nocheck(DEAL_FUN_ARG);
int user_jy_get_mature_fruit_id(DEAL_FUN_ARG);

//宠物任务完成后赠送物品
int user_pet_task_attire_give(DEAL_FUN_ARG);

/// 用户搬石头，取得当天搬的石头和总共的石头
int roominfo_stone_move_get (DEAL_FUN_ARG);
/// 用户搬石头，设置当天搬的石头和总共的石头
int roominfo_stone_move_set (DEAL_FUN_ARG);
/// 发现在企鹅，取得用户企鹅孵蛋的次数
int roominfo_penguin_egg_get (DEAL_FUN_ARG);
/// 发现在企鹅，设置用户企鹅孵蛋的次数
int roominfo_penguin_egg_set (DEAL_FUN_ARG);
/// 勇士抽奖，给予限制等等
int roominfo_warrior_lucky_draw(DEAL_FUN_ARG);
/// 勇士抽奖，查询当前抽奖状态
int roominfo_warrior_lucky_draw_get(DEAL_FUN_ARG);
int roominfo_client_data_set(DEAL_FUN_ARG);
int roominfo_client_data_get(DEAL_FUN_ARG);

/// 取得宠物装扮的数目
int user_pet_get_count_with_attireid(DEAL_FUN_ARG);

int user_ex_set_int_value(DEAL_FUN_ARG);

int user_attire_get_attire_except_chest(DEAL_FUN_ARG);

int user_pet_task_set_task(DEAL_FUN_ARG);

int user_pet_task_get_tasklist(DEAL_FUN_ARG);

int user_pet_task_set_temp_data(DEAL_FUN_ARG);
int user_pet_task_get_temp_data(DEAL_FUN_ARG);

/// 取得用户宠物的任务状态，所有的
int user_pet_task_get_by_user(DEAL_FUN_ARG);
int user_pet_task_get_client_by_user_task(DEAL_FUN_ARG);
int user_animal_set_flag(DEAL_FUN_ARG);
int roominfo_list_set_wish(DEAL_FUN_ARG);
int roominfo_list_get_wish(DEAL_FUN_ARG);

int user_change_object_id(DEAL_FUN_ARG);

///诺亚舟
int user_noah_sn_bind_userid(DEAL_FUN_ARG);
int user_noah_sn_unbind_userid(DEAL_FUN_ARG);
int user_noah_add_user_info(DEAL_FUN_ARG);
int user_noah_get_user_bind_info(DEAL_FUN_ARG);
int user_noah_get_userid_by_sn(DEAL_FUN_ARG);

int user_get_noah_info(DEAL_FUN_ARG);

int user_mdog_get_mdog(DEAL_FUN_ARG);
int user_mdog_del_mdog(DEAL_FUN_ARG);
int user_mdog_query_mdog_info(DEAL_FUN_ARG);
int user_mdog_supply_energe(DEAL_FUN_ARG);
int user_mdog_query_user_mdog(DEAL_FUN_ARG);
int userinfo_set_question2(DEAL_FUN_ARG);
int userinfo_get_question2(DEAL_FUN_ARG);
int userinfo_verify_question2(DEAL_FUN_ARG);

int user_pet_set_hot_skill(DEAL_FUN_ARG);
int user_su_get_change_value(DEAL_FUN_ARG);

int user_animal_milk_cow(DEAL_FUN_ARG);
int user_ex_set_event_flag(DEAL_FUN_ARG);
int user_ex_set_sendmail_flag(DEAL_FUN_ARG);
int user_ex_add_month(DEAL_FUN_ARG);
int user_ex_get_month(DEAL_FUN_ARG);

int roominfo_user_set_npc_score(DEAL_FUN_ARG);
int roominfo_user_get_npc_score(DEAL_FUN_ARG);
int userinfo_set_passwd_ex(DEAL_FUN_ARG);
int userinfo_set_change_passwd_flag_ex(DEAL_FUN_ARG);
int userinfo_set_passwd_only_check_change_flag(DEAL_FUN_ARG);

int user_catch_fruit_single(userid_t userid, jy_item *p_item, uint32_t& exp_out);
int user_catch_fruit_all(DEAL_FUN_ARG);
int userinfo_login_by_md5_two(DEAL_FUN_ARG);


int diningroom_user_create_diningroom(DEAL_FUN_ARG);
int diningroom_user_get_frame_diningroom(DEAL_FUN_ARG);
int diningroom_user_set_room_name(DEAL_FUN_ARG);
int diningroom_user_set_room_style(DEAL_FUN_ARG);
int diningroom_user_get_last_frameid(DEAL_FUN_ARG);
int user_diningroom_create_room(DEAL_FUN_ARG);
int user_diningroom_set_room_name(DEAL_FUN_ARG);
int user_diningroom_set_room_style(DEAL_FUN_ARG);
int user_diningroom_set_room_inner_style(DEAL_FUN_ARG);
int user_get_diningroom(DEAL_FUN_ARG);
int su_user_get_diningroom(DEAL_FUN_ARG);
int user_dining_add_employer(DEAL_FUN_ARG);
int user_dining_del_employer(DEAL_FUN_ARG);
int user_dining_cook_dish(DEAL_FUN_ARG);
int user_dining_dish_in_chest(DEAL_FUN_ARG);
int user_dining_eat_dish(DEAL_FUN_ARG);
int user_dining_fall_dish(DEAL_FUN_ARG);
int user_dining_cook_change_state(DEAL_FUN_ARG);
int user_dining_user_work_for_other(DEAL_FUN_ARG);
int user_dining_get_sys_employer_list(DEAL_FUN_ARG);
int user_dining_get_user_board_info(DEAL_FUN_ARG);

int user_diningroom_user_off(userid_t userid, uint32_t cur_time, uint32_t eat_time, uint32_t& off_xiaomee);
int user_dining_dec_evaluate(DEAL_FUN_ARG);
int user_dining_set_level(DEAL_FUN_ARG);
int user_dining_user_room_count(DEAL_FUN_ARG);
int user_pet_set_employ_user(DEAL_FUN_ARG);
int user_pet_employ_info_list(DEAL_FUN_ARG);
int user_dining_set_honor_flag(DEAL_FUN_ARG);
int roominfo_user_set_prove(DEAL_FUN_ARG);
int roominfo_user_set_item(DEAL_FUN_ARG);
int roominfo_user_get_prove(DEAL_FUN_ARG);
int userinfo_get_question2_set_flag(DEAL_FUN_ARG);
int user_farm_animal_get_type_mature(DEAL_FUN_ARG);

int roominfo_set_client_buf(DEAL_FUN_ARG);
int roominfo_get_client_buf(DEAL_FUN_ARG);

int user_dining_inc_exp(DEAL_FUN_ARG);
int user_dining_inc_xiaomee(DEAL_FUN_ARG);
int userid_get_userid(DEAL_FUN_ARG);
int userid_set_noused(DEAL_FUN_ARG);

//	sport meet
int sysarg_db_sports_teaminfo_set(DEAL_FUN_ARG);
int sysarg_db_sports_teaminfo_score_get(DEAL_FUN_ARG);
int sysarg_db_sports_teaminfo_medal_get(DEAL_FUN_ARG);
int sysarg_db_sports_tmpuser_user_set(DEAL_FUN_ARG);
int sysarg_db_sports_gamescore_set(DEAL_FUN_ARG);
int sysarg_db_sports_gamescore_get(DEAL_FUN_ARG);
int roominfo_sports_meet_medal_set(DEAL_FUN_ARG);
int roominfo_sports_pet_reward_set(DEAL_FUN_ARG);
int roominfo_sports_user_reward_set(DEAL_FUN_ARG);
int roominfo_sports_reward_get(DEAL_FUN_ARG);
int sysarg_db_sports_team_reward_get(DEAL_FUN_ARG);

int user_dec_dish(userid_t userid,user_dining_dec_dish_in *p_in);
int user_dining_dec_dish(DEAL_FUN_ARG);
int user_dining_cook_dish_list(DEAL_FUN_ARG);
int userinfo_set_mee_fans(DEAL_FUN_ARG);
int userinfo_get_mee_fans(DEAL_FUN_ARG);
int user_dining_stars_get(DEAL_FUN_ARG);
int select_dish_stars(DEAL_FUN_ARG);

int user_pet_pet_flag_set(DEAL_FUN_ARG);
int user_diningroom_event_num_set(DEAL_FUN_ARG);
int user_dining_get_dish_count(DEAL_FUN_ARG);

int user_sports_inc_xiaomee(DEAL_FUN_ARG);
int user_dining_dish_history_stars_set(DEAL_FUN_ARG);

int roominfo_reward_num_set(DEAL_FUN_ARG);
int roominfo_reward_num_get(DEAL_FUN_ARG);
int sysarg_set_type_buf(DEAL_FUN_ARG);
int sysarg_get_type_buf(DEAL_FUN_ARG);

int user_dining_dec_exp(DEAL_FUN_ARG);
int user_noah_inc_user_info(DEAL_FUN_ARG);

int user_new_card_get_card_info(DEAL_FUN_ARG);
int user_new_card_init_card(DEAL_FUN_ARG);
int user_new_card_add_card(DEAL_FUN_ARG);
int user_new_card_add_exp(DEAL_FUN_ARG);
int user_new_card_get_exp(DEAL_FUN_ARG);
int userinfo_set_user_enable_flag(DEAL_FUN_ARG);
int user_get_nick_color(DEAL_FUN_ARG);
int hatch_dragon_egg(DEAL_FUN_ARG);
int add_dragon_egg_hatch_time(DEAL_FUN_ARG);
int get_egg_hatch_time(DEAL_FUN_ARG);
int get_dragon_hatched(DEAL_FUN_ARG);
int get_dragon_list(DEAL_FUN_ARG);
int add_dragon_growth(DEAL_FUN_ARG);
int set_dragon_state(DEAL_FUN_ARG);
int release_dragon(DEAL_FUN_ARG);
int user_new_card_add_cards(DEAL_FUN_ARG);
int user_set_dragon_name(DEAL_FUN_ARG);
int roominfo_get_item_sell(DEAL_FUN_ARG);
int roominfo_add_item_sell(DEAL_FUN_ARG);
int user_get_user_board_info(DEAL_FUN_ARG);
int user_set_fight_monster(DEAL_FUN_ARG);
int get_current_dragon(DEAL_FUN_ARG);
int user_get_lamu_diary_list(DEAL_FUN_ARG);
int userinfo_get_game_act_flag(DEAL_FUN_ARG);
int userinfo_add_act_flag(DEAL_FUN_ARG);
int get_friend_box_items(DEAL_FUN_ARG);
int get_friend_box_history(DEAL_FUN_ARG);
int put_item_to_friend_box(DEAL_FUN_ARG);
int get_item_from_friend_box(DEAL_FUN_ARG);
int check_item_from_friend(DEAL_FUN_ARG);
int set_tag_item_from_friend(DEAL_FUN_ARG);
int user_checkin_black(DEAL_FUN_ARG);
int user_pet_is_super_lamu(DEAL_FUN_ARG);
int lamu_classroom_user_create_classroom(DEAL_FUN_ARG);
int lamu_classroom_user_set_classroom_name(DEAL_FUN_ARG);
int lamu_classroom_tw_get_exp_and_name(DEAL_FUN_ARG);
int lamu_classroom_user_get_frame_classroom(DEAL_FUN_ARG);
int lamu_classroom_user_get_last_frameid(DEAL_FUN_ARG);
int lamu_classroom_get_rand_user(DEAL_FUN_ARG);
int lamu_classroom_set_teacher_exp(DEAL_FUN_ARG);
int lamu_classroom_set_outstand_sum(DEAL_FUN_ARG);
int lamu_classroom_get_friend_rank(DEAL_FUN_ARG);
int lamu_classroom_set_exp_and_outstand_sum(DEAL_FUN_ARG);
int user_lamu_classroom_create_classroom(DEAL_FUN_ARG);
int user_lamu_classroom_set_name(DEAL_FUN_ARG);
int user_lamu_classroom_set_inner_style(DEAL_FUN_ARG);
int user_lamu_classroom_add_lamus(DEAL_FUN_ARG);
int user_lamu_classroom_abandon_lamus(DEAL_FUN_ARG);
int user_lamu_classroom_get_class_info(DEAL_FUN_ARG);
int user_lamu_classroom_get_lamus_info(DEAL_FUN_ARG);
int user_lamu_classroom_get_record(DEAL_FUN_ARG);
int user_lamu_classroom_get_remembrance(DEAL_FUN_ARG);
int user_lamu_classroom_get_teach_plan(DEAL_FUN_ARG);
int user_lamu_classroom_get_course_list(DEAL_FUN_ARG);
int user_lamu_classroom_class_begin(DEAL_FUN_ARG);
int user_lamu_classroom_set_teach_mode(DEAL_FUN_ARG);
int user_lamu_classroom_class_end(DEAL_FUN_ARG);
int user_lamu_classroom_get_exp_course(DEAL_FUN_ARG);
int user_lamu_classroom_is_class_time(DEAL_FUN_ARG);
int user_lamu_classroom_begin_exam(DEAL_FUN_ARG);
int user_lamu_classroom_get_exam_info(DEAL_FUN_ARG);
int user_lamu_classroom_tw_get_exam_result(DEAL_FUN_ARG);
int user_lamu_classroom_have_class(DEAL_FUN_ARG);
int user_lamu_classroom_get_prize(DEAL_FUN_ARG);
int user_lamu_classroom_get_class_flag(DEAL_FUN_ARG);
int user_lamu_classroom_class_graduate(DEAL_FUN_ARG);
int user_lamu_classroom_add_course_cnt(DEAL_FUN_ARG);
int user_lamu_classroom_check_add_course(DEAL_FUN_ARG);
int user_lamu_classroom_recover_teacher_attr(DEAL_FUN_ARG);
int user_lamu_classroom_set_lamu_mood(DEAL_FUN_ARG);
int user_lamu_classroom_update_lamu_attr(DEAL_FUN_ARG);
int user_lamu_classroom_add_course_score(DEAL_FUN_ARG);
int user_lamu_classroom_set_event_cnt(DEAL_FUN_ARG);
int user_lamu_classroom_set_honor_flag(DEAL_FUN_ARG);
int user_lamu_classroom_add_lamu_attrs(DEAL_FUN_ARG);
int sysarg_auction_begin_auction(DEAL_FUN_ARG);
int sysarg_auction_add_one_record(DEAL_FUN_ARG);
int sysarg_auction_get_attireid_and_time(DEAL_FUN_ARG);
int sysarg_auction_end_auction(DEAL_FUN_ARG);
int sysarg_contribute_xiamee_insert(DEAL_FUN_ARG);
int sysarg_contribute_get_rank(DEAL_FUN_ARG);
int sysarg_contribute_get_user_count(DEAL_FUN_ARG);
int user_get_attireid_and_cnt(DEAL_FUN_ARG);
int set_class_network(DEAL_FUN_ARG);
//苞子花用户排行榜
int sysarg_get_ranklist(DEAL_FUN_ARG);
int user_set_user_rank_buf(DEAL_FUN_ARG);
int sysarg_set_ranklist(DEAL_FUN_ARG);
int user_get_recv_record(DEAL_FUN_ARG);
int exchange_flower_to_present(DEAL_FUN_ARG);
int user_check_out_buf(DEAL_FUN_ARG);
int sysarg_get_double_count(DEAL_FUN_ARG);
//点点豆摇奖机
int roominfo_get_shake_rewards_times(DEAL_FUN_ARG);
int roominfo_change_shake_rewards_times(DEAL_FUN_ARG);
//米币消耗
int roominfo_inc_consume_money(DEAL_FUN_ARG);
int roominfo_get_consume_money(DEAL_FUN_ARG);
int roominfo_dec_consume_money(DEAL_FUN_ARG);

int roominfo_add_christmas_sock(DEAL_FUN_ARG);
int roominfo_get_christmas_sock_info(DEAL_FUN_ARG);
int roominfo_del_christmas_sock_info(DEAL_FUN_ARG);
int user_get_home_attire_count(DEAL_FUN_ARG);
//清空包月信息
int user_ex_clear_bonus_used(DEAL_FUN_ARG);

//蛋糕积分
int roominfo_inc_cake_score(DEAL_FUN_ARG);
int roominfo_dec_cake_score(DEAL_FUN_ARG);
int roominfo_get_cake_score(DEAL_FUN_ARG);

int roominfo_query_user_cake_info(DEAL_FUN_ARG);
int roominfo_user_do_cake_fin(DEAL_FUN_ARG);
int roominfo_user_send_cake(DEAL_FUN_ARG);

//小脚印
int roominfo_footprint_add(DEAL_FUN_ARG);
int roominfo_exchange_footprint(DEAL_FUN_ARG);
int roominfo_get_footprint_count(DEAL_FUN_ARG);

//连续登陆礼包
int roominfo_query_login_gifts(DEAL_FUN_ARG);
int user_farm_get_fairy(DEAL_FUN_ARG);
int user_farm_speed_animal_growth(DEAL_FUN_ARG);
int roominfo_get_train_kakunian_info(DEAL_FUN_ARG);
int roominfo_set_train_kakunian_count(DEAL_FUN_ARG);

int sysarg_add_skill_count(DEAL_FUN_ARG);
int sysarg_update_skill_count(DEAL_FUN_ARG);	

int user_fairy_skill_get_info(DEAL_FUN_ARG);
int user_fairy_skill_update_info(DEAL_FUN_ARG);
int user_fairy_get_growth(DEAL_FUN_ARG);

int user_farm_get_super_animal_count(DEAL_FUN_ARG);
int sysarg_get_skill_use_info(DEAL_FUN_ARG);
int user_get_paradise(DEAL_FUN_ARG);
int user_change_paradise_backgound(DEAL_FUN_ARG);
int user_change_angel_pos(DEAL_FUN_ARG);
int user_paradise_set_nimbus(DEAL_FUN_ARG);
int user_angel_sign_contract(DEAL_FUN_ARG);
int user_angel_free(DEAL_FUN_ARG);
int user_paradise_get_visitlist(DEAL_FUN_ARG);
int user_get_paradise_attirelist(DEAL_FUN_ARG);	
int user_del_attire_angel_tool(DEAL_FUN_ARG);
int user_get_angel_contract_all(DEAL_FUN_ARG);
int user_terminate_contract(DEAL_FUN_ARG);
int user_get_contract_angel_follow(DEAL_FUN_ARG);
int user_fairy_convert_angel(DEAL_FUN_ARG);
int roominfo_get_week_count(DEAL_FUN_ARG);
int roominfo_update_week_count(DEAL_FUN_ARG);
int user_angel_add_dragon( userid_t userid, uint32_t angelid);
int sysarg_get_paradise_friend_rank(DEAL_FUN_ARG);
int sysarg_update_hot_play(DEAL_FUN_ARG);
int sysarg_get_paradise_access_rank(DEAL_FUN_ARG);
int user_paradise_update_visitlist(DEAL_FUN_ARG);
int sysarg_get_hot_play(DEAL_FUN_ARG);
int sysarg_update_paradise_rank(DEAL_FUN_ARG);
int user_get_mole_lev_att_nick(DEAL_FUN_ARG);
int cal_mole_level(int exp);
int user_angel_speed_up(DEAL_FUN_ARG);
int user_get_battle_angel_info(DEAL_FUN_ARG);
int user_angel_get_battle_level(DEAL_FUN_ARG);
int user_angel_barrier_get_score(DEAL_FUN_ARG);
int user_angel_battle_over(DEAL_FUN_ARG);
int user_record_top_barrier_score(uint32_t userid, uint32_t id,
	uint32_t type, uint32_t barrier_score, uint32_t pass);
int user_capture_angel(DEAL_FUN_ARG);
int user_capture_random_angel(uint32_t userid, uint32_t angelid,
	uint32_t &partition, uint32_t &dinominator);
int user_get_angel_in_hospital(DEAL_FUN_ARG);
int user_angel_tool_recover(DEAL_FUN_ARG);
int user_angel_go_out_hospital(DEAL_FUN_ARG);
int user_add_angel_contract(DEAL_FUN_ARG);
int user_get_angel_favorite(DEAL_FUN_ARG);
int user_exhibit_favorite_angel(DEAL_FUN_ARG);
int user_get_used_background(DEAL_FUN_ARG);

int user_exchange_davis_bean(DEAL_FUN_ARG);
int user_get_angel_honors(DEAL_FUN_ARG);
int user_get_gift_after_unlock(DEAL_FUN_ARG);
int user_cal_paradise_honor(userid_t userid, uint32_t level, uint32_t *ret_type,
	uint32_t *honor_id, uint32_t angelid, uint32_t angel_count ,
	user_angel_statistic_stru *p_item, uint32_t total_sort, uint32_t first,
	uint32_t *p_list, uint32_t honor_num);
int user_cal_battle_honor(userid_t userid, uint32_t level, uint32_t barrier_id,
	uint32_t *ret_type, uint32_t *honor_id, uint32_t *p_list, uint32_t battle_cnt);
int user_get_dig_treasure_begin(DEAL_FUN_ARG);
int user_get_angel_contract_all_ex(DEAL_FUN_ARG);
int user_dig_treasure_update_player(DEAL_FUN_ARG);
int user_dig_treasure_used_tool(DEAL_FUN_ARG);
int user_dig_treasure_update_random_count(DEAL_FUN_ARG);
int user_get_expedition_bag(DEAL_FUN_ARG);
int user_synthesis_angel_get_material(DEAL_FUN_ARG);
int user_synthesis_angel_with_material(DEAL_FUN_ARG);
int user_set_attire_count(DEAL_FUN_ARG);
int user_set_angel_type_ex(DEAL_FUN_ARG);
int user_synthesis_history(DEAL_FUN_ARG);
int show_angel(uint32_t userid, uint32_t angelid, uint32_t exhibit_angelid, uint32_t flag);
int user_get_treasure_warehouse(DEAL_FUN_ARG);
int user_get_treasure_collection(DEAL_FUN_ARG);
int user_show_treasure(DEAL_FUN_ARG);
int user_exchange_treasure_show_pos(DEAL_FUN_ARG);
int user_treasure_collection_get_visitlist(DEAL_FUN_ARG);
int user_treasure_collection_update_visitlist(DEAL_FUN_ARG);
int user_change_show_treasure_status(DEAL_FUN_ARG);
int sysarg_update_treasure_collection(DEAL_FUN_ARG);
int sysarg_get_treasure_visit(DEAL_FUN_ARG);
int sysarg_get_treasure_friend(DEAL_FUN_ARG);

int user_get_type_item_list(DEAL_FUN_ARG);
int roominfo_strike_pengin_game(DEAL_FUN_ARG);
int roominfo_strike_penguin_get_info(DEAL_FUN_ARG);
int user_continue_login_shop(DEAL_FUN_ARG);

int user_hide_and_seek_get_award(DEAL_FUN_ARG);
int user_get_mongolian_cow_pasture(DEAL_FUN_ARG);
int user_play_with_milk_cow(DEAL_FUN_ARG);
int user_milk_cow_pasture_plant(DEAL_FUN_ARG);
int user_set_pasture_milk_storage(DEAL_FUN_ARG);
int emailsys_del_list(DEAL_FUN_ARG);
int roominfo_get_random_gain_times(DEAL_FUN_ARG);
int roominfo_change_random_gain_times(DEAL_FUN_ARG);

int user_get_battle_character_bag(DEAL_FUN_ARG);
int user_get_battle_skill(DEAL_FUN_ARG);
int user_get_battle_bag(DEAL_FUN_ARG);
int user_battle_use_props(DEAL_FUN_ARG);
int user_battle_get_card_booklet(DEAL_FUN_ARG);
int user_gameserv_battle_get_info(DEAL_FUN_ARG);
int user_battle_use_card(DEAL_FUN_ARG);
int user_battle_upgrade_skill(DEAL_FUN_ARG);
int user_equip_skill_attire(DEAL_FUN_ARG);
int user_get_battle_colletion(DEAL_FUN_ARG);
int user_battle_game_over_caculate(DEAL_FUN_ARG);
int user_battle_get_pass_achievement(userid_t userid, uint32_t barrier_id , uint32_t comp_degree, 
	 user_battle_over_info_out_header *p_out, user_battle_over_info_out_item_1* &p_offset);
int angel_battle_get_friends_exp(DEAL_FUN_ARG);
int angel_battle_update_exp(DEAL_FUN_ARG);
int user_battle_get_friend_fight_list(DEAL_FUN_ARG);
int user_battle_map_info(DEAL_FUN_ARG);
int user_battle_get_one_skill(DEAL_FUN_ARG);
int user_battle_gameserv_consume(DEAL_FUN_ARG);
int user_battle_get_flip_info(DEAL_FUN_ARG);
int user_battle_over_info(DEAL_FUN_ARG);
int user_diningroom_get_wish(DEAL_FUN_ARG);
int user_battle_able_to_wish(DEAL_FUN_ARG);
int user_battle_wish_succ(DEAL_FUN_ARG);
int user_battle_get_friend_event_info(DEAL_FUN_ARG);
int user_get_self_events_info(DEAL_FUN_ARG);
int user_battle_bless_friend_info(DEAL_FUN_ARG);
int user_diningroom_set_wish_cnt(DEAL_FUN_ARG);
int user_diningroom_bless_get_wish_info(DEAL_FUN_ARG);
int user_dining_room_get_c_event_info(DEAL_FUN_ARG);
int user_diningroom_set_com_event(DEAL_FUN_ARG);
int user_battle_handle_common_event(DEAL_FUN_ARG);
int user_get_wish_item_id(DEAL_FUN_ARG);
int user_battle_gefight_over_info(DEAL_FUN_ARG);
int user_battle_give_wish_article(DEAL_FUN_ARG);
int user_battle_add_exp_handle_event(userid_t userid, uint32_t exp_inc, uint32_t *is_upgrade,
	uint32_t *count, user_battle_over_info_out_item_1* p_offset);
int user_battle_get_day_tasklist(DEAL_FUN_ARG);
int user_battle_refresh_day_task(DEAL_FUN_ARG);
int user_battle_new_day_task(DEAL_FUN_ARG);
int user_battle_terminate_day_task(DEAL_FUN_ARG);
int user_battle_add_exp(DEAL_FUN_ARG);
int user_battle_new_a_task(DEAL_FUN_ARG);
int user_battle_get_challenge_times(DEAL_FUN_ARG);
int user_battle_get_prentice_market_filter(DEAL_FUN_ARG);
int user_battle_get_prentice_market(DEAL_FUN_ARG);
int user_battle_get_self_mp_relatrion(DEAL_FUN_ARG);
int user_battle_get_master_info(DEAL_FUN_ARG);
int user_battle_check_teach_prentice(DEAL_FUN_ARG);
int user_battle_get_train_info(DEAL_FUN_ARG);
int user_battle_train_prentice(DEAL_FUN_ARG);
int user_battle_evict_prentice(DEAL_FUN_ARG);
int user_battle_evict_prentice_another(DEAL_FUN_ARG);
int user_dininingroom_evict_prentice_another(DEAL_FUN_ARG);
int user_battle_respect_master(DEAL_FUN_ARG);
int user_battle_respect_set_in_master(DEAL_FUN_ARG);
int user_battle_master_receive_respect(DEAL_FUN_ARG);
int user_master_finish_apprentice(DEAL_FUN_ARG);
int user_check_finish_apprentice(DEAL_FUN_ARG);
int user_battle_prentice_finish_master(DEAL_FUN_ARG);
int user_battle_get_master_prentice_info(DEAL_FUN_ARG);
int user_battle_check_prentice(DEAL_FUN_ARG);
int user_battle_set_new_master(DEAL_FUN_ARG);
int user_battle_get_level(DEAL_FUN_ARG);
int user_battle_add_prentice(DEAL_FUN_ARG);
int user_battle_add_mp_msg(DEAL_FUN_ARG);
int user_send_time_mail(DEAL_FUN_ARG);
int user_get_time_mail(DEAL_FUN_ARG);
int user_send_time_item(DEAL_FUN_ARG);
int user_get_time_item(DEAL_FUN_ARG);
int user_get_non_dig_time_article(DEAL_FUN_ARG);
int user_battle_get_ini_skill_level_ex(DEAL_FUN_ARG);
int user_get_angel_battle_level_ex(DEAL_FUN_ARG);
int user_battle_get_pvp_info(DEAL_FUN_ARG);
int user_battle_recveive_pvp_card(DEAL_FUN_ARG);
int user_eat_moon_cake(DEAL_FUN_ARG);
int user_get_moon_cake(DEAL_FUN_ARG);
int user_get_exp_and_vip_level(DEAL_FUN_ARG);
int get_user_task_ex_idlist(DEAL_FUN_ARG);
int user_get_piglet_house(DEAL_FUN_ARG);
int user_get_single_piglet_info(DEAL_FUN_ARG);
int user_get_piglet_hungry(DEAL_FUN_ARG);
int user_feed_fancy_piglet(DEAL_FUN_ARG);
int user_train_piglet(DEAL_FUN_ARG);
int user_amuse_piglet(DEAL_FUN_ARG);
int user_bath_piglet(DEAL_FUN_ARG);
int user_change_piglet_nick(DEAL_FUN_ARG);
int user_change_piglet_formation(DEAL_FUN_ARG);
int user_check_mail_piglet(DEAL_FUN_ARG);
int user_femail_piglet_pregnant(DEAL_FUN_ARG);
int user_get_piglet_articles(DEAL_FUN_ARG);
int user_sale_piglet(DEAL_FUN_ARG);
int user_get_able_make_pair_mail_piglet(DEAL_FUN_ARG);
int sysarg_alter_piglet_house_level(DEAL_FUN_ARG);
int user_upgrade_piglet_house(DEAL_FUN_ARG);
int user_get_friend_piglet_house(DEAL_FUN_ARG);
int user_get_piglet_day_task(DEAL_FUN_ARG);
int user_new_piglet_task(DEAL_FUN_ARG);
int user_finish_piglet_task(DEAL_FUN_ARG);
int user_get_all_msg(DEAL_FUN_ARG);
int user_piglet_factory_process(DEAL_FUN_ARG);
int user_piglet_rand_fortune(DEAL_FUN_ARG);
int user_piglet_get_handbook(DEAL_FUN_ARG);
int user_piglet_upgrade_building(DEAL_FUN_ARG);
int user_piglet_use_tool(DEAL_FUN_ARG);
int user_piglet_speed_birth(uint32_t userid, uint32_t index,uint32_t pregnant_time, 
uint32_t sex, uint32_t *state, uint32_t *flag);
int user_piglet_task_change(DEAL_FUN_ARG);
int user_piglet_receive_ticket(DEAL_FUN_ARG);

int user_get_lucky_ticket(DEAL_FUN_ARG);
int user_check_get_ticket(DEAL_FUN_ARG);
int user_swap_ticket_prize(DEAL_FUN_ARG);
int user_get_history_ticket_info(DEAL_FUN_ARG);
int user_get_national_day_login(DEAL_FUN_ARG);
int user_set_national_day_gift(DEAL_FUN_ARG);
int user_piglet_get_effect_tools(DEAL_FUN_ARG);
int user_piglet_poll(DEAL_FUN_ARG);
int user_piglet_use_effect_tool(DEAL_FUN_ARG);
int sysarg_get_questionnaire_subtype(DEAL_FUN_ARG);
int user_check_response_friend(DEAL_FUN_ARG);
int user_check_exist_friend(DEAL_FUN_ARG);
int user_get_piglet_poll(DEAL_FUN_ARG);
int user_delete_bank_money(DEAL_FUN_ARG);
int user_swap_halloween_candy(DEAL_FUN_ARG);
int user_get_swap_halloween_history(DEAL_FUN_ARG);
int user_swap_halloween_item(DEAL_FUN_ARG);
int user_cal_new_paradise_honor(userid_t userid );
int su_user_get_all_piglets(DEAL_FUN_ARG);
int su_user_get_pilget_attire(DEAL_FUN_ARG);
int user_get_xhx_task_state(DEAL_FUN_ARG);
int user_get_profiles(DEAL_FUN_ARG);
int user_add_profile_dress(DEAL_FUN_ARG);
int user_remove_profile_dress(DEAL_FUN_ARG);
int user_swap_piglet_house_back(DEAL_FUN_ARG);
int user_set_continue_login_value(DEAL_FUN_ARG);
int user_get_task_list(DEAL_FUN_ARG);
int su_get_usermsg_mushroom_guide(DEAL_FUN_ARG);
int user_mushroom_guider_receive_award(DEAL_FUN_ARG);
int user_guid_mushroom_get_awards(DEAL_FUN_ARG);
int usermsg_mushroom_guide_del(DEAL_FUN_ARG);
int user_piglet_carry_piglet(DEAL_FUN_ARG);
int user_check_piglet_performance(DEAL_FUN_ARG);
int user_get_another_pighouse(DEAL_FUN_ARG);
int user_transfer_piglet(DEAL_FUN_ARG);
int user_piglet_add_beaty_tool(DEAL_FUN_ARG);
int user_get_beauty_piglet_tool(DEAL_FUN_ARG);
int su_user_drop_super_mushroom_record(DEAL_FUN_ARG);
int user_put_on_piglet_clothes(DEAL_FUN_ARG);
int user_change_beauty_piglet(DEAL_FUN_ARG);
int sysarg_join_beauty_contend(DEAL_FUN_ARG);
int sysarg_get_beauty_contend(DEAL_FUN_ARG);
int user_check_feed_piglet(DEAL_FUN_ARG);
int user_get_beauty_contend_cnt(DEAL_FUN_ARG);
int user_as_common_add(DEAL_FUN_ARG);
int user_as_common_query(DEAL_FUN_ARG);
int roominfo_set_beauty_continuation(DEAL_FUN_ARG);
int roominfo_get_award_beauty(DEAL_FUN_ARG);
int roominfo_get_award_history(DEAL_FUN_ARG);
int user_put_on_christmas_socks(DEAL_FUN_ARG);
int user_query_christmas_socks(DEAL_FUN_ARG);
int user_check_christmas_socks(DEAL_FUN_ARG);
int user_sysarg_get_christmas_top10(DEAL_FUN_ARG);
int user_sysarg_set_christmas_top10(DEAL_FUN_ARG);
int user_check_piglet_handbook(DEAL_FUN_ARG);
int user_check_christmas_sock_gift(DEAL_FUN_ARG);
int user_set_christmas_sock_gift(DEAL_FUN_ARG);
int user_get_vip_angel_info(DEAL_FUN_ARG);
int user_exchange_vip_angel_info(DEAL_FUN_ARG);
int user_sysarg_check_player_num(DEAL_FUN_ARG);
int user_sysarg_matching_map(DEAL_FUN_ARG);
int user_sysarg_drop_map(DEAL_FUN_ARG);
int user_sysarg_get_map(DEAL_FUN_ARG);
int user_explore_get_key(DEAL_FUN_ARG);
int user_explore_clear_state(DEAL_FUN_ARG);
int user_get_explore_state(DEAL_FUN_ARG);

int sysarg_add_contribute_item(DEAL_FUN_ARG);
int sysarg_begin_love_auction(DEAL_FUN_ARG);
int sysarg_get_auction_list(DEAL_FUN_ARG);
int sysarg_auction_item(DEAL_FUN_ARG);
int user_sysarg_clear_explore_num(DEAL_FUN_ARG);
int user_get_piglet_honor(DEAL_FUN_ARG);
int user_set_piglet_honor(DEAL_FUN_ARG);
int user_sysarg_set_wish_wall(DEAL_FUN_ARG);
int user_sysarg_get_wish_wall(DEAL_FUN_ARG);
int user_write_spring_couplet(DEAL_FUN_ARG);
int user_get_wish_wall(DEAL_FUN_ARG);
int sysarg_day_get_value(DEAL_FUN_ARG);
int user_set_wish(DEAL_FUN_ARG);
int sysarg_check_prob_lahm(DEAL_FUN_ARG);
int sysarg_add_prob_lahm(DEAL_FUN_ARG);
int sysarg_get_prob_lahm(DEAL_FUN_ARG);
int sysarg_set_two_team_energy(DEAL_FUN_ARG);
int sysarg_get_two_team_energy(DEAL_FUN_ARG);
int sysarg_add_guess_price(DEAL_FUN_ARG);
int sysarg_get_guess_price(DEAL_FUN_ARG);
int user_spring_festival_login(DEAL_FUN_ARG);
int user_spring_festival_recive_award(DEAL_FUN_ARG);
int user_get_spring_festival_login_info(DEAL_FUN_ARG);
int user_set_spring_feast(DEAL_FUN_ARG);
int user_get_wish_award(DEAL_FUN_ARG);
int user_vip_continue_week_login(DEAL_FUN_ARG);
int user_vip_get_continue_week(DEAL_FUN_ARG);
int user_vip_get_week_ward(DEAL_FUN_ARG);
int user_set_weekend_login(DEAL_FUN_ARG);
int user_get_weekend_login_award(DEAL_FUN_ARG);
int user_get_login_weekend(DEAL_FUN_ARG);
int sysarg_add_mark_attire(DEAL_FUN_ARG);
int sysarg_get_mark_attire(DEAL_FUN_ARG);
int user_set_mark_attire(DEAL_FUN_ARG);
int user_get_mark_attire(DEAL_FUN_ARG);
int user_strength_ice_dragon(DEAL_FUN_ARG);
int user_exchange_piglet_house(DEAL_FUN_ARG);
int user_get_golden_key(DEAL_FUN_ARG);
int user_get_piglet_machine_work(DEAL_FUN_ARG);
int user_send_piglet_mining(DEAL_FUN_ARG);
int user_inject_piglet_energy(DEAL_FUN_ARG);
int user_piglet_melt_ore(DEAL_FUN_ARG);
int user_get_piglet_mining_info(DEAL_FUN_ARG);
int user_finish_some_map_mining(DEAL_FUN_ARG);
int user_produce_machine_part(DEAL_FUN_ARG);
int user_piglet_finish_work(DEAL_FUN_ARG);
int user_get_piglet_work_machine(DEAL_FUN_ARG);
int user_use_machine_tools(DEAL_FUN_ARG);
int user_get_process(DEAL_FUN_ARG);
int user_set_process(DEAL_FUN_ARG);
int user_get_menue(DEAL_FUN_ARG);
int user_finish_menue(DEAL_FUN_ARG);
int user_submit_menue(DEAL_FUN_ARG);
int user_produce_special_machine(DEAL_FUN_ARG);
int sysarg_set_greate_produce(DEAL_FUN_ARG);
int sysarg_get_greate_produce(DEAL_FUN_ARG);
int user_produce_product(DEAL_FUN_ARG);
int get_partymenue_setting(DEAL_FUN_ARG);
int set_partymenue_setting(DEAL_FUN_ARG);
int get_new_friendship_box_items(DEAL_FUN_ARG);
int get_new_friendship_box_history(DEAL_FUN_ARG);
int put_item_to_new_friendship_box(DEAL_FUN_ARG);
int get_item_from_new_friendship_box(DEAL_FUN_ARG);
int check_item_from_new_friendship_box(DEAL_FUN_ARG);
int set_tag_item_from_new_friendship_box(DEAL_FUN_ARG);
int sysarg_vote_birthday_decorate(DEAL_FUN_ARG);
int sysarg_get_birthday_decorate(DEAL_FUN_ARG);
int user_get_butterfly(DEAL_FUN_ARG);
int user_reset_pigmap(DEAL_FUN_ARG);
int user_create_team(DEAL_FUN_ARG);
int user_mvp_get_teamid(DEAL_FUN_ARG);
int user_mvp_set_teamid(DEAL_FUN_ARG);
int user_mvp_removeone(DEAL_FUN_ARG);
int user_mvp_addone(DEAL_FUN_ARG);
int user_mvp_deleteall(DEAL_FUN_ARG);
int user_get_teaminfo(DEAL_FUN_ARG);
int sysarg_create_mvp_team(DEAL_FUN_ARG);
int sysarg_drop_mvp_team(DEAL_FUN_ARG);
int sysarg_get_mvp_team(DEAL_FUN_ARG);
int sysarg_set_mvp_badge(DEAL_FUN_ARG);
int user_set_mvp_badge(DEAL_FUN_ARG);
int sysarg_change_mvp_team_member_count(DEAL_FUN_ARG);
int sysarg_get_random_mvp_team(DEAL_FUN_ARG);
int user_get_mvp_badge(DEAL_FUN_ARG);
int user_get_teamid(DEAL_FUN_ARG);
int user_get_sysarg_mvp_teaminfo(DEAL_FUN_ARG);
int user_set_pig_guess(DEAL_FUN_ARG);
int user_get_pig_guess(DEAL_FUN_ARG);
int user_get_pig_prize(DEAL_FUN_ARG);
int su_user_get_piglet_machine_info(DEAL_FUN_ARG);
int su_user_get_work_piglet_info(DEAL_FUN_ARG);
int su_user_get_piglet_machine_warehouse(DEAL_FUN_ARG);
int sysarg_tw_day_award_get(DEAL_FUN_ARG);
int user_get_together_photo_info(DEAL_FUN_ARG);
int user_set_together_photo(DEAL_FUN_ARG);
int sysarg_magic_bean_watering(DEAL_FUN_ARG);
int sysarg_magic_bean_querying(DEAL_FUN_ARG);
int user_miss_note_stone(DEAL_FUN_ARG);
int user_get_miss_not_stone(DEAL_FUN_ARG);
int user_shake_dice_act(DEAL_FUN_ARG);
int user_get_shake_dice_left_time(DEAL_FUN_ARG);
int user_check_expedition_lock(DEAL_FUN_ARG);
int user_set_story_chapter_state(DEAL_FUN_ARG);
int user_get_story_chapter_state(DEAL_FUN_ARG);
int user_get_make_submarine(DEAL_FUN_ARG);
int user_stone_scissors_cloth(DEAL_FUN_ARG);
int user_check_roll_dice(DEAL_FUN_ARG);
int sysarg_get_free_vip_player(DEAL_FUN_ARG);
int user_get_merman_kingdom(DEAL_FUN_ARG);
int user_middle_year_cheap_seal(DEAL_FUN_ARG);
int user_join_cosplay_race(DEAL_FUN_ARG);
int sysarg_set_cos_user_info(DEAL_FUN_ARG);
int sysarg_get_cos_rank(DEAL_FUN_ARG);
int user_enter_ocean(DEAL_FUN_ARG);
int user_feed_fish(DEAL_FUN_ARG);
int user_put_in_ocean(DEAL_FUN_ARG);
int user_diy_ocean(DEAL_FUN_ARG);
int user_apply_fish_tool(DEAL_FUN_ARG);
int user_expand_ocean_capacity(DEAL_FUN_ARG);
int user_get_ocean_items(DEAL_FUN_ARG);
int user_get_ocean_offline_shells(DEAL_FUN_ARG);
int user_get_ocean_online_shells(DEAL_FUN_ARG);
int user_clear_online_ocean_drop(DEAL_FUN_ARG);
int user_set_expedition_pray(DEAL_FUN_ARG);
int user_get_expedition_pray(DEAL_FUN_ARG);
int sysarg_get_user_rank_flag(DEAL_FUN_ARG);
int user_sale_ocean_fish(DEAL_FUN_ARG);
int user_set_ocean_shell(DEAL_FUN_ARG);
int user_buy_ocean_things(DEAL_FUN_ARG);
int user_add_mole_adventure_pass(DEAL_FUN_ARG);
int user_get_mole_adventure_pass(DEAL_FUN_ARG);
int user_get_games_info(DEAL_FUN_ARG);
int user_get_ocean_handbook(DEAL_FUN_ARG);
int sysarg_get_ocean_friend(DEAL_FUN_ARG);
int sysarg_get_game_top_ten(DEAL_FUN_ARG);
int user_add_game_socre(DEAL_FUN_ARG);
int user_get_socre_by_gameid(DEAL_FUN_ARG);
int sysarg_game_hall_change(DEAL_FUN_ARG);
int user_get_scroll_map_state(DEAL_FUN_ARG);
int user_get_scroll_dragment(DEAL_FUN_ARG);
int sysarg_ocean_synchro_exp(DEAL_FUN_ARG);
int sysarg_self_game_rank(DEAL_FUN_ARG);
int user_finish_merman_kindom(DEAL_FUN_ARG);

}; /* -----  end of class  Croute_func  ----- */

#endif   /* ----- #ifndef CROUTE_FUNC_INC  ----- */

