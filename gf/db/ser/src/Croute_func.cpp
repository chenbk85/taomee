/*
 * =====================================================================================
 *
 *
 *	   Filename:  route_func.cpp
 *
 *	Description:
 *
 *		Version:  1.0
 *		Created:  2009 
 *	   Revision:  none
 *	   Compiler:  gcc
 *
 *		 Author:  jim(jim), jim@taomee.com
 *		Company:  TAOMEE
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <string.h>
#include "Croute_func.h"
#include "benchapi.h"
#include "proto.h"
#include "db_error.h"
#include "gf_common.h"
#include <limits.h>
#include <libtaomee/time/time.h>
#include <libtaomee/bitmanip/bitmanip.h>
#include <libtaomee++/inet/pdumanip.hpp>
#include <kf/player_attr.hpp>

#include <libtaomee/time/time.h>
#include <libtaomee/bitmanip/bitmanip.h>
#include <libtaomee++/inet/pdumanip.hpp>
#include <kf/utils.hpp>



using namespace taomee ;

extern  char * version_str;
#ifdef MAKE_SEND_EMAIL
CMD_MAP g_cmdmap[]={
    FILL_CMD_WITH_PRI_IN    (other_gf_sync_vip),  
    FILL_CMD_WITH_PRI_IN    (other_gf_sync_base_svalue),   
    FILL_CMD_WITH_PRI_IN    (other_gf_sync_gold),
    FILL_CMD_WITH_PRI_IN_GE (other_gf_vip_sys_msg),
};
#else
CMD_MAP g_cmdmap[]={
	//--------------------------
    FILL_CMD_WITH_PRI_IN    (gf_set_account_forbid),
    FILL_CMD_WITHOUT_PRI_IN (gf_get_xiaomee),
    FILL_CMD_WITHOUT_PRI_IN (gf_fuck_version_check),
//vip start
    FILL_CMD_WITHOUT_PRI_IN (gf_get_user_vip),
    FILL_CMD_WITH_PRI_IN    (gf_set_vip),
    FILL_CMD_WITH_PRI_IN    (gf_set_base_svalue),
    FILL_CMD_WITH_PRI_IN    (gf_set_gold),
    // FILL_CMD_WITH_PRI_IN    (gf_sys_vip_msg),
    FILL_CMD_WITH_PRI_IN    (gf_set_vip_exinfo),
//vip end
//store start
    FILL_CMD_WITH_PRI_IN_GE (gf_prepare_add_product),
    FILL_CMD_WITH_PRI_IN_GE (gf_add_store_product),
    FILL_CMD_WITH_PRI_IN_GE (gf_prepare_repair_product),
    FILL_CMD_WITH_PRI_IN_GE (gf_repair_store_product),
    FILL_CMD_WITHOUT_PRI_IN (gf_get_outdated_product),
    FILL_CMD_WITH_PRI_IN    (gf_synchro_user_gfcoin),
    FILL_CMD_WITHOUT_PRI_IN (gf_query_user_gfcoin),
    FILL_CMD_WITH_PRI_IN_GE (gf_coupons_exchange),
//store end
	FILL_CMD_WITH_PRI_IN    (gf_set_cryptogram_gift),
	FILL_CMD_WITH_PRI_IN_GE (gf_set_magic_gift),
	FILL_CMD_WITH_PRI_IN_GE (gf_set_magic_box),
	FILL_CMD_WITH_PRI_IN	(gf_logout),

	FILL_CMD_WITHOUT_PRI_IN (get_warehouse_item_list),
	FILL_CMD_WITH_PRI_IN    (gf_move_item_warehouse_to_bag),
	FILL_CMD_WITH_PRI_IN    (gf_move_item_bag_to_warehouse),	
	FILL_CMD_WITHOUT_PRI_IN (get_warehouse_clothes_item_list),
	FILL_CMD_WITH_PRI_IN    (gf_move_clothes_item_bag_to_warehouse),
	FILL_CMD_WITH_PRI_IN    (gf_move_clothes_item_warehouse_to_bag),

	FILL_CMD_WITH_PRI_IN    (gf_learn_secondary_pro),
	FILL_CMD_WITH_PRI_IN_GE (gf_secondary_pro_fuse),
	FILL_CMD_WITHOUT_PRI_IN (gf_get_secondary_pro_list),


	FILL_CMD_WITHOUT_PRI_IN (gf_get_buy_item_limit_list),
	FILL_CMD_WITH_PRI_IN    (gf_set_buy_item_limit_data),
//mail begin
	FILL_CMD_WITHOUT_PRI_IN (gf_mail_head_list),  
	FILL_CMD_WITH_PRI_IN    (gf_mail_body),
	FILL_CMD_WITH_PRI_IN    (gf_delete_mail),
	FILL_CMD_WITH_PRI_IN_GE (gf_take_mail_enclosure),
	FILL_CMD_WITH_PRI_IN    (gf_send_mail),
	FILL_CMD_WITH_PRI_IN    (gf_send_system_mail),
	FILL_CMD_WITH_PRI_IN    (gf_reduce_money),
//kill boss begin
	FILL_CMD_WITHOUT_PRI_IN (gf_get_kill_boss_list),
	FILL_CMD_WITH_PRI_IN    (gf_replace_kill_boss),
//home begin
	FILL_CMD_WITHOUT_PRI_IN (gf_get_home_data),
	FILL_CMD_WITH_PRI_IN    (gf_set_home_data),
	FILL_CMD_WITH_PRI_IN    (gf_set_home_update_tm),
//	FILL_CMD_WITH_PRI_IN    (gf_replace_home_exp_level),
//	FILL_CMD_WITH_PRI_IN    (gf_update_home_active_point),
//achievement begin
	FILL_CMD_WITHOUT_PRI_IN (gf_get_achievement_data_list),
	FILL_CMD_WITH_PRI_IN    (gf_replace_achievement_data),
	FILL_CMD_WITHOUT_PRI_IN (gf_get_achieve_title),
	FILL_CMD_WITH_PRI_IN_GE (gf_add_achieve_title),
	FILL_CMD_WITH_PRI_IN    (gf_set_cur_achieve_title),
	FILL_CMD_WITH_PRI_IN (gf_get_ap_toplist),	
//achievement stat
	FILL_CMD_WITHOUT_PRI_IN (gf_get_stat_info),
	FILL_CMD_WITH_PRI_IN    (gf_set_stat_info),
//ambassador new
    FILL_CMD_WITH_PRI_IN    (gf_set_child_count),
    FILL_CMD_WITHOUT_PRI_IN (gf_set_achieve_count),
    FILL_CMD_WITH_PRI_IN    (gf_set_amb_status),
    FILL_CMD_WITH_PRI_IN    (gf_set_amb_reward_flag),
//end ambassador
	FILL_CMD_WITHOUT_PRI_IN	(gf_get_user_info),
	FILL_CMD_WITHOUT_PRI_IN	(gf_get_info_for_login),
	FILL_CMD_WITH_PRI_IN	(gf_set_nick),
	FILL_CMD_WITHOUT_PRI_IN	(gf_get_nick),
	FILL_CMD_WITH_PRI_IN    (gf_set_role_state),
	FILL_CMD_WITH_PRI_IN    (gf_set_open_box_times),
	FILL_CMD_WITHOUT_PRI_IN (gf_get_donate_count),
	FILL_CMD_WITH_PRI_IN	(gf_notify_use_item),
	FILL_CMD_WITH_PRI_IN	(gf_notify_user_get_shop_item),
	
	FILL_CMD_WITH_PRI_IN	(gf_add_buff),
	FILL_CMD_WITH_PRI_IN_GE	(gf_update_buff_list),
	FILL_CMD_WITH_PRI_IN    (gf_del_buff_on_player),

	FILL_CMD_WITH_PRI_IN	(gf_add_friend),
	FILL_CMD_WITH_PRI_IN	(gf_del_friend),

	FILL_CMD_WITH_PRI_IN    (gf_forbiden_add_friend_flag),
	FILL_CMD_WITHOUT_PRI_IN (gf_query_forbiden_friend_flag),

	FILL_CMD_WITH_PRI_IN	(gf_add_black),
	FILL_CMD_WITH_PRI_IN	(gf_del_black),
	FILL_CMD_WITH_PRI_IN	(gf_del_friend_whatever),
	FILL_CMD_WITHOUT_PRI_IN	(gf_get_blacklist),
	
	FILL_CMD_WITH_PRI_IN    (gf_get_friendlist_type),

	FILL_CMD_WITHOUT_PRI_IN	(gf_get_clothes_list),
	FILL_CMD_WITHOUT_PRI_IN	(gf_get_strengthen_material_list),
	FILL_CMD_WITH_PRI_IN_GE	(gf_wear_clothes),
	FILL_CMD_WITH_PRI_IN    (gf_take_off_clothes),
	FILL_CMD_WITH_PRI_IN	(gf_strengthen_attire),
	//added by cws 0620
	FILL_CMD_WITH_PRI_IN	(gf_strengthen_attire_without_material),
	FILL_CMD_WITHOUT_PRI_IN (gf_get_user_item_list),
	FILL_CMD_WITHOUT_PRI_IN (gf_get_package_iclist),
	FILL_CMD_WITH_PRI_IN	(gf_set_item_int_value),
	FILL_CMD_WITH_PRI_IN	(gf_add_item),
	FILL_CMD_WITH_PRI_IN    (gf_add_item_attire),
	FILL_CMD_WITH_PRI_IN	(gf_del_item),
	FILL_CMD_WITH_PRI_IN_GE	(gf_del_items),
	FILL_CMD_WITH_PRI_IN_GE	(gf_bitch_sell_items),
	FILL_CMD_WITH_PRI_IN    (gf_buy_item),
	FILL_CMD_WITH_PRI_IN	(gf_sell_item),
    FILL_CMD_WITH_PRI_IN    (gf_buy_goods),
    FILL_CMD_WITH_PRI_IN    (gf_sell_goods),
	FILL_CMD_WITH_PRI_IN	(gf_battle_use_item),
	FILL_CMD_WITH_PRI_IN_GE	(gf_pick_up_item),
	FILL_CMD_WITH_PRI_IN_GE (gf_pick_up_roll_item),
	FILL_CMD_WITH_PRI_IN_GE (gf_game_input),

	FILL_CMD_WITH_PRI_IN  (gf_get_user_partial_info),

	FILL_CMD_WITH_PRI_IN (gf_get_role_detail_info),

	FILL_CMD_WITH_PRI_IN_GE (gf_add_offline_msg),

	FILL_CMD_WITHOUT_PRI_IN	(gf_role_login),

    FILL_CMD_WITH_PRI_IN (gf_set_role_uinque_itembit),
	FILL_CMD_WITHOUT_PRI_IN (gf_get_user_base_info),
	FILL_CMD_WITH_PRI_IN (gf_set_user_base_info),
	FILL_CMD_WITH_PRI_IN (gf_set_role_pvp_info),

	FILL_CMD_WITHOUT_PRI_IN (gf_get_role_list),

	FILL_CMD_WITHOUT_PRI_IN (gf_get_role_list_ex),

	FILL_CMD_WITH_PRI_IN 		(gf_del_attire),
	FILL_CMD_WITH_PRI_IN 		(gf_add_attire),
	FILL_CMD_WITH_PRI_IN_GE     (gf_buy_attire),
	FILL_CMD_WITH_PRI_IN 		(gf_sell_attire),
	FILL_CMD_WITH_PRI_IN_GE		(gf_repair_all_attire),
	FILL_CMD_WITH_PRI_IN		(gf_attire_set_int_value),
	FILL_CMD_WITH_PRI_IN_GE		(gf_compose_attire),
	FILL_CMD_WITH_PRI_IN_GE		(gf_decompose_attire),

	FILL_CMD_WITHOUT_PRI_IN (gf_get_clothes_list_ex),

	FILL_CMD_WITH_PRI_IN (gf_add_role),

	FILL_CMD_WITHOUT_PRI_IN (gf_gray_delete_role),
	FILL_CMD_WITHOUT_PRI_IN (gf_logic_delete_role),
	FILL_CMD_WITHOUT_PRI_IN (gf_resume_gray_role),

	FILL_CMD_WITHOUT_PRI_IN (gf_del_role),

	FILL_CMD_WITH_PRI_IN	(gf_set_role_int_value),
	FILL_CMD_WITH_PRI_IN	(gf_exchange_coins_fight_value),
	
	FILL_CMD_WITH_PRI_IN	(gf_set_role_itembind),
	FILL_CMD_WITHOUT_PRI_IN	(gf_get_role_itembind),
	FILL_CMD_WITHOUT_PRI_IN (gf_user_login),
	FILL_CMD_WITH_PRI_IN_GE	(gf_set_role_base_info),
	FILL_CMD_WITH_PRI_IN_GE	(gf_set_role_base_info_2),
	FILL_CMD_WITH_PRI_IN	(gf_set_role_stage_info),
	FILL_CMD_WITHOUT_PRI_IN	(gf_get_double_exp_data),
	FILL_CMD_WITH_PRI_IN	(gf_set_double_exp_data),
	FILL_CMD_WITHOUT_PRI_IN	(gf_get_max_times_chapter),
	FILL_CMD_WITH_PRI_IN	(gf_set_max_times_chapter),

	FILL_CMD_WITHOUT_PRI_IN    (gf_get_task_finished),
	FILL_CMD_WITH_PRI_IN_GE (gf_del_outmoded_task),
	
	FILL_CMD_WITHOUT_PRI_IN    (gf_get_task_in_hand),

	FILL_CMD_WITHOUT_PRI_IN    (gf_get_all_task),
	FILL_CMD_WITHOUT_PRI_IN    (gf_get_task_list),
	
	FILL_CMD_WITH_PRI_IN       (gf_set_task_flg),
	
	FILL_CMD_WITH_PRI_IN_GE    (gf_set_task_value),

	FILL_CMD_WITH_PRI_IN_GE    (gf_task_swap_item),

	FILL_CMD_WITH_PRI_IN    (gf_add_skill_points),

	FILL_CMD_WITH_PRI_IN    (gf_add_skill_with_no_book),

	FILL_CMD_WITH_PRI_IN    (gf_del_skill),

	FILL_CMD_WITH_PRI_IN    (gf_upgrade_skill),

	FILL_CMD_WITHOUT_PRI_IN    (gf_get_skill_list),
	FILL_CMD_WITH_PRI_IN    	(gf_reset_skill),

	FILL_CMD_WITH_PRI_IN_GE		(gf_set_skill_bind_key),

	FILL_CMD_WITHOUT_PRI_IN		(gf_get_skill_bind_key),

	FILL_CMD_WITH_PRI_IN		(gf_learn_new_skill),

	FILL_CMD_WITH_PRI_IN	(gf_add_killed_boss),

	FILL_CMD_WITHOUT_PRI_IN	(gf_get_killed_boss),

	FILL_CMD_WITH_PRI_IN		(gf_check_invite_code),

	FILL_CMD_WITHOUT_PRI_IN		(gf_check_user_invited),

	FILL_CMD_WITH_PRI_IN		(gf_get_invit_code),

	FILL_CMD_WITHOUT_PRI_IN (gf_get_daily_action),
	FILL_CMD_WITH_PRI_IN    (gf_set_daily_action),
    //begin summon
    FILL_CMD_WITHOUT_PRI_IN (gf_get_summon_list),
	FILL_CMD_WITH_PRI_IN    (gf_hatch_summon),
	FILL_CMD_WITHOUT_PRI_IN	(gf_get_last_summon_nick),
	FILL_CMD_WITH_PRI_IN    (gf_feed_summon),
	FILL_CMD_WITH_PRI_IN    (gf_set_summon_nick),
	FILL_CMD_WITH_PRI_IN    (gf_change_summon_fight),
	FILL_CMD_WITH_PRI_IN    (gf_set_summon_property),
	FILL_CMD_WITH_PRI_IN    (gf_set_summon_skills),
	FILL_CMD_WITH_PRI_IN    (gf_use_summon_skills_scroll),
	FILL_CMD_WITH_PRI_IN    (gf_set_summon_mutate),
	FILL_CMD_WITH_PRI_IN_GE (gf_set_summon_type),
	FILL_CMD_WITH_PRI_IN    (gf_allocate_exp2summon),

	FILL_CMD_WITH_PRI_IN    (gf_fresh_summon_attr),
    //end summon
    //---- Begin Numen
    FILL_CMD_WITHOUT_PRI_IN (gf_get_numen_list),
    FILL_CMD_WITH_PRI_IN_GE (gf_invite_numen),
    FILL_CMD_WITH_PRI_IN    (gf_change_numen_status),
    FILL_CMD_WITH_PRI_IN    (gf_make_numen_sex),
    FILL_CMD_WITH_PRI_IN    (gf_change_numen_nick),
    //---- End Numen

	FILL_CMD_WITHOUT_PRI_IN	(gf_set_amb_info),
	FILL_CMD_WITHOUT_PRI_IN	(gf_get_amb_info),
	FILL_CMD_WITH_PRI_IN (gf_get_hero_top_info),
	FILL_CMD_WITH_PRI_IN (gf_set_hero_top_info),
    FILL_CMD_WITH_PRI_IN (gf_get_hunter_top_info),
    FILL_CMD_WITH_PRI_IN (gf_get_hunter_top_info_for_web),
    FILL_CMD_WITHOUT_PRI_IN (gf_get_clothes_info_for_web),
    FILL_CMD_WITH_PRI_IN (gf_get_hunter_info),
	FILL_CMD_WITH_PRI_IN (gf_set_hunter_top_info),
	FILL_CMD_WITHOUT_PRI_IN (gf_del_player_place_in_old_top),

	//天下第一比武大会	
	FILL_CMD_WITH_PRI_IN (gf_join_contest_team),
	FILL_CMD_WITHOUT_PRI_IN (gf_get_contest_team_info),
	FILL_CMD_WITHOUT_PRI_IN (gf_get_contest_donate),
	FILL_CMD_WITH_PRI_IN (gf_contest_donate_plant),
	FILL_CMD_WITH_PRI_IN_GE (gf_donate_item),
	FILL_CMD_WITHOUT_PRI_IN (gf_get_self_contest_team),

	FILL_CMD_WITH_PRI_IN_GE (gf_upgrade_item),
	FILL_CMD_WITH_PRI_IN_GE (gf_swap_action_func),
	FILL_CMD_WITHOUT_PRI_IN (gf_get_swap_action),
	FILL_CMD_WITH_PRI_IN    (gf_clear_swap_action),
	//added by cws 0608
	FILL_CMD_WITH_PRI_IN(gf_add_swap),

	FILL_CMD_WITH_PRI_IN    (gf_get_pvp_game_data),
	FILL_CMD_WITH_PRI_IN    (gf_set_pvp_game_data),
	FILL_CMD_WITH_PRI_IN    (gf_set_pvp_game_flower),
	FILL_CMD_WITH_PRI_IN    (gf_get_pvp_game_rank),
	FILL_CMD_WITH_PRI_IN_GE (gf_team_member_reward),
	FILL_CMD_WITH_PRI_IN	(gf_set_client_buf),
	FILL_CMD_WITHOUT_PRI_IN	(gf_get_client_buf),
	FILL_CMD_WITHOUT_PRI_IN (gf_get_plant_list),
	FILL_CMD_WITH_PRI_IN    (gf_set_plant_status),
	FILL_CMD_WITH_PRI_IN    (gf_add_effect_to_all_plants),
	FILL_CMD_WITH_PRI_IN	(gf_add_home_log),
	FILL_CMD_WITHOUT_PRI_IN	(gf_get_home_log),
	FILL_CMD_WITH_PRI_IN_GE (gf_pick_fruit),
	FILL_CMD_WITHOUT_PRI_IN (gf_get_summon_dragon_list),
	FILL_CMD_WITH_PRI_IN (gf_add_summon_dragon),

	

	FILL_CMD_WITH_PRI_IN (gf_add_shop_log),
	FILL_CMD_WITHOUT_PRI_IN (gf_shop_last_log),

	FILL_CMD_WITH_PRI_IN (gf_add_donate_count),
	FILL_CMD_WITHOUT_PRI_IN (gf_get_role_base_info_for_boss),
	FILL_CMD_WITHOUT_PRI_IN (gf_get_player_community_info),

	FILL_CMD_WITHOUT_PRI_IN (gf_get_ring_task_list),
	FILL_CMD_WITHOUT_PRI_IN (gf_get_ring_task_history_list),
	FILL_CMD_WITH_PRI_IN    (gf_replace_ring_task),
	FILL_CMD_WITH_PRI_IN    (gf_delete_ring_task),
	
	FILL_CMD_WITH_PRI_IN    (gf_set_role_vitality_point),
	FILL_CMD_WITH_PRI_IN    (gf_set_second_pro_exp),
	FILL_CMD_WITH_PRI_IN    (gf_replace_ring_task_history),

	FILL_CMD_WITHOUT_PRI_IN (gf_get_card_list),
	FILL_CMD_WITH_PRI_IN    (gf_insert_card),
	FILL_CMD_WITH_PRI_IN    (gf_erase_card),

	//for off line mode
	FILL_CMD_WITHOUT_PRI_IN (gf_get_offline_info),
	FILL_CMD_WITH_PRI_IN    (gf_set_offline_info),

	//for fight team
	FILL_CMD_WITH_PRI_IN    (gf_create_fight_team),
	FILL_CMD_WITH_PRI_IN    (gf_add_fight_team_member),
	FILL_CMD_WITH_PRI_IN    (gf_set_player_team_flg),
	FILL_CMD_WITH_PRI_IN    (gf_get_team_info),
	FILL_CMD_WITH_PRI_IN	(gf_del_team_member),
	FILL_CMD_WITH_PRI_IN	(gf_del_team),
	FILL_CMD_WITH_PRI_IN	(gf_team_info_changed),
	FILL_CMD_WITHOUT_PRI_IN	(gf_get_team_top10),

	FILL_CMD_WITH_PRI_IN    (gf_search_team_info),
	FILL_CMD_WITH_PRI_IN    (gf_gain_team_exp),
	FILL_CMD_WITH_PRI_IN	(gf_contribute_team),
	FILL_CMD_WITH_PRI_IN    (gf_team_reduce_tax),
	FILL_CMD_WITH_PRI_IN	(gf_fetch_team_coin),
	FILL_CMD_WITH_PRI_IN	(gf_team_active_enter),
	FILL_CMD_WITH_PRI_IN	(gf_get_team_active_top100),
	FILL_CMD_WITH_PRI_IN	(gf_team_active_score_change),
	FILL_CMD_WITH_PRI_IN    (gf_set_team_member_level),
	FILL_CMD_WITH_PRI_IN    (gf_change_team_name),
	FILL_CMD_WITH_PRI_IN    (gf_change_team_mcast), 

	FILL_CMD_WITHOUT_PRI_IN (gf_list_true_wusheng_info),
	FILL_CMD_WITH_PRI_IN    (gf_set_true_wusheng_info),

	FILL_CMD_WITHOUT_PRI_IN (gf_get_other_info_list),
	FILL_CMD_WITH_PRI_IN (gf_set_other_info),

	FILL_CMD_WITH_PRI_IN    (gf_list_ranker_info),
	FILL_CMD_WITH_PRI_IN    (gf_set_ranker_info),

	FILL_CMD_WITH_PRI_IN (gf_get_other_active),
	FILL_CMD_WITH_PRI_IN    (gf_set_other_active),
	FILL_CMD_WITHOUT_PRI_IN (gf_get_other_active_list),
	FILL_CMD_WITH_PRI_IN    (gf_get_single_other_active),
	FILL_CMD_WITH_PRI_IN    (gf_set_role_god_guard),

    FILL_CMD_WITH_PRI_IN    (gf_set_power_user),
    FILL_CMD_WITH_PRI_IN    (gf_set_player_fate),
    // //// 师徒 ////////
    FILL_CMD_WITHOUT_PRI_IN (gf_get_master),
    FILL_CMD_WITHOUT_PRI_IN (gf_get_apprentice),
    FILL_CMD_WITH_PRI_IN    (gf_master_add_apprentice),
    FILL_CMD_WITH_PRI_IN    (gf_master_del_apprentice),
    FILL_CMD_WITH_PRI_IN    (gf_set_prentice_grade),
    FILL_CMD_WITHOUT_PRI_IN (gf_get_prentice_cnt),

    FILL_CMD_WITH_PRI_IN    (gf_set_reward_player),
    FILL_CMD_WITHOUT_PRI_IN (gf_get_reward_player),
    FILL_CMD_WITH_PRI_IN    (gf_set_player_reward_flag),

    //------ for home decorate ---------
    FILL_CMD_WITHOUT_PRI_IN (gf_get_decorate_list),
    FILL_CMD_WITH_PRI_IN    (gf_set_decorate),
    FILL_CMD_WITH_PRI_IN_GE (gf_add_decorate_lv),

	FILL_CMD_WITH_PRI_IN	(gf_insert_tmp_info),
		
	FILL_CMD_WITH_PRI_IN_GE (gf_check_update_trade_info),
	FILL_CMD_WITH_PRI_IN_GE (gf_safe_trade_item),
	FILL_CMD_WITH_PRI_IN (gf_distract_clothes_strength),

	FILL_CMD_WITH_PRI_IN    (gf_list_simple_role_info)


};
#endif

#define DEALFUN_COUNT  (int(sizeof(g_cmdmap )/sizeof(g_cmdmap[0])))

#include "./gf_db.h"
stru_cmd_item_t g_new_cmd_map[]={
#include "./gf_db_bind.h"
};
#define NEW_DEALFUN_COUNT  (int(sizeof(g_new_cmd_map )/sizeof(g_new_cmd_map[0])))


/*
 *--------------------------------------------------------------------------------------
 *	   Class:  Croute_func
 *	  Method:  Croute_func
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */


Croute_func::Croute_func (mysql_interface * db, Cclientproto * cp):Cfunc_route_base(db),
	gongfu(db), gf_attire(db), gf_item(db, &gf_item_change_log),
	gf_friend(db), gf_role(db), gf_task(db),
	gf_skill(db), gf_invite_code(db), gf_killboss(db),gf_summon(db), 
    gf_daction(db), gf_waction(db), gf_maction(db), gf_warehouse(db), gf_clothes_warehouse(db),
    gf_amb(db), send_email(cp), gf_secondary_pro(db), gf_mail(db), gf_item_log(db), gf_get_shopitem_log(db), gf_market_log(config_get_strval("MARKET_LOG_UDP")),
    gf_buff(db),gf_material(db, &gf_item_change_log), gf_hero_top(db), gf_shop_log(db), gf_hunter_top(db),
	gf_donate(db),  gf_buy_item_limit(db),  gf_kill_boss(db), gf_achieve(db), gf_stat(db),  gf_title(db),
	gf_ap_toplist(db),  gf_ring_task(db), gf_ring_task_history(db),gf_contest(db), gf_taotai(db),
	gf_swap_action(db), gf_plant(db), gf_home(db), gf_home_log(db), gf_summon_dragon_list(db), gf_item_change_log(db, "GF"), gf_card(db), gf_summon_skill(db),
	gf_team(db), gf_team_detail(db), gf_wuseng_info(db), gf_other_info(db), gf_ranker(db), gf_master(db),
    gf_reward(db), gf_decorate(db), gf_tmp_info(db), gf_numen(db), gf_numen_skill(db), gf_other_active(db)
{
	this->db=db;
	this->initlist(g_cmdmap,DEALFUN_COUNT );	
	//this->cmdmaplist.show_item();
    this->vip_obj.init();

	//初始化命令列表
	uint32_t malloc_size=sizeof(stru_cmd_item_t )*64*1024;
	this->p_cmd_list=(stru_cmd_item_t* ) malloc (malloc_size );
	memset(this->p_cmd_list,0, malloc_size);
	this->set_cmd_list(g_new_cmd_map,NEW_DEALFUN_COUNT );



	printf("exp: %u %u %u %u\n", calc_exp(1), calc_exp(2), calc_old_exp(1), calc_old_exp(2));
}  /* -----  end of method Croute_func::Croute_func  (constructor)  ----- */

Croute_func::~Croute_func()
{
    this->vip_obj.final();
}
int Croute_func::do_sync_data(uint32_t userid, uint16_t cmdid )
{
       return SUCC;
}


int Croute_func::gf_get_xiaomee(DEAL_FUN_ARG)
{
    gf_get_xiaomee_out out = {0};
    ret = this->gf_role.get_coins(USERID_ROLETM, &(out.count));
    if (ret != SUCC) {
        return ret;
    }

	if (out.count > 1000000000) {
		out.count = 1000000000;
		this->gf_role.set_coins(USERID_ROLETM, out.count);
	}
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_fuck_version_check(DEAL_FUN_ARG)
{
    STD_RETURN(SUCC);
}

/**  
 * @fn 取得用户信息
 * @brief  cmd route interface
 * @param  
 * @return 
 * @note player
 */
int Croute_func::gf_get_user_info(DEAL_FUN_ARG)
{
	char out[PROTO_MAX_SIZE] = {0};
	int cpy_len = 0;
	gf_get_user_info_out_header* p_out_header = (gf_get_user_info_out_header*)out;
	gf_attire_id_duration_list* p_clothes_out_item = NULL;
//	gf_skill_bind_list* p_skill_out_item = NULL;
	stru_gf_info_out* p_user_info = &(p_out_header->user_info);
	stru_msg_list user_msg_list = {0};
	stru_msg_list role_msg_list = {0};
	stru_role_info role_info = {0};
	uint32_t count = 0;
	uint32_t len = 0;
	
		
	if (del_daily_loop(USERID_ROLETM) != SUCC)
	{
		return ret;
	}


	char* p_offset = &(out[0]) + sizeof(*p_out_header);

	//取得用户全局信息
    uint32_t forbid_flag = 0;
	ret = this->gongfu.get_user_info(USERID_ROLETM, &(p_out_header->user_info), &forbid_flag,&user_msg_list);
	if (ret != SUCC || forbid_flag != 0)
	{
		return ret;
	}
	//DEBUG_LOG("-----------1:%d  %d ",user_msg_list.len, user_msg_list.count);
	
	//取得角色信息
	ret = this->gf_role.get_role_info(USERID_ROLETM, &role_info, &role_msg_list);
	if (ret != SUCC)
	{
		return ret;
	}
	p_user_info->role_regtime = role_info.role_regtime;
	p_user_info->role_type = role_info.role_type;
	p_user_info->Ol_count = role_info.Ol_count;
	strncpy(p_user_info->nick, role_info.nick, sizeof(p_user_info->nick));
	p_user_info->skill_point = role_info.skill_point;
	p_user_info->map_id = role_info.map_id;
	p_user_info->xpos = role_info.xpos;
	p_user_info->ypos = role_info.ypos;
	p_user_info->xiaomee = role_info.xiaomee;
	p_user_info->level = role_info.level;
	p_user_info->exp = role_info.exp;
	p_user_info->hp = role_info.hp;
	p_user_info->mp = role_info.mp;
	p_user_info->honour = role_info.honour;
	p_user_info->fight = role_info.fight;
	p_user_info->win = role_info.win;
	p_user_info->fail = role_info.fail;
	p_user_info->winning_streak = role_info.winning_streak;
	p_user_info->fumo_points_today = role_info.fumo_points_today;
	p_user_info->fumo_points_total = role_info.fumo_points;
	memcpy (p_user_info->uniqueitem, role_info.uniqueitem, sizeof(p_user_info->uniqueitem));
	memcpy (p_user_info->itembind, role_info.itembind, sizeof(p_user_info->itembind));
	p_user_info->show_state = role_info.show_state;		
	p_user_info->open_box_times = role_info.open_box_times;
	p_user_info->achieve_point = role_info.achieve_point;
	p_user_info->last_update_tm = role_info.last_update_tm;
	p_user_info->achieve_title = role_info.achieve_title;
	p_user_info->forbiden_add_friend_flag = p_out_header->user_info.forbiden_add_friend_flag;
	p_user_info->vitality_point = role_info.vitality_point;
	//DEBUG_LOG("-----------2:%d  %d ", role_msg_list.len, role_msg_list.count);
	
	if (p_user_info->xiaomee > 1000000000) {
		DEBUG_LOG("USER　%u TOO MUCH xiaomee %u", RECVBUF_USERID, p_user_info->xiaomee);
		p_user_info->xiaomee = 1000000000;
		this->gf_role.set_coins(RECVBUF_USERID, RECVBUF_ROLETM, p_user_info->xiaomee);
	}
	
	//合成离线消息，存入out buff
	count = user_msg_list.count;
	len = user_msg_list.len;
	uint32_t total_len = user_msg_list.len + role_msg_list.len;
	user_msg_list.count = user_msg_list.count + role_msg_list.count;
	user_msg_list.len = total_len==0?msg_list_head_len:total_len ;
	p_out_header->msg_size = user_msg_list.len;
	
	//用户全局离线消息 存入out
	cpy_len = len==0 ? msg_list_head_len:len;
	memcpy(p_offset, &user_msg_list, cpy_len);
	p_offset += cpy_len;
	
	//角色离线消息 存入out
	cpy_len = role_msg_list.len>msg_list_head_len?(role_msg_list.len - msg_list_head_len):0;
	memcpy(p_offset, role_msg_list.buf, cpy_len);
	p_offset += cpy_len;
	
	//初始化数据库离线消息字段
	if ( count!=0 )
	{
		stru_msg_list t_msglist = {0};
		t_msglist.len = msg_list_head_len;
		t_msglist.count = 0;
		this->gongfu.update_msglist(RECVBUF_USERID, &t_msglist);		
    }	
	if ( role_msg_list.count!=0 )
	{
		stru_msg_list t_msglist = {0};
		t_msglist.len = msg_list_head_len;
		t_msglist.count = 0;
		this->gf_role.update_msglist(USERID_ROLETM, &t_msglist);		
  	}	
	
	//取得用户身上装备列表
	ret = this->gf_attire.get_used_clothes_list_with_duration(USERID_ROLETM, &(p_out_header->used_clothes_count), &p_clothes_out_item);
	if (ret != SUCC)
	{
		return ret;
	}

	if (p_clothes_out_item)
	{
		cpy_len = sizeof(*p_clothes_out_item) * p_out_header->used_clothes_count;
		memcpy(p_offset, p_clothes_out_item, cpy_len);
		free(p_clothes_out_item);
		p_offset += cpy_len;
	}

#if 1
	ret = gf_get_skill_bind_private(USERID_ROLETM,  &(p_out_header->skill_count),
		(gf_get_skill_bind_key_out_item*)p_offset);
	if (ret != SUCC)
	{
		return ret;
	}
	p_offset += sizeof(gf_get_skill_bind_key_out_item) * p_out_header->skill_count;
#else
	gf_skill_bind_list* p_skill_out_item = NULL;
	ret = this->gf_skill.get_skill_bind_key(USERID_ROLETM, &(p_out_header->skill_count),
		(gf_get_skill_bind_key_out_item**)&p_skill_out_item);
	if (p_skill_out_item)
	{
		cpy_len = sizeof(*p_skill_out_item) * p_out_header->skill_count;
		memcpy(p_offset, p_skill_out_item, cpy_len);
		free(p_skill_out_item);
		p_offset += cpy_len;
	}	
#endif
	DEBUG_LOG("get user info:uid=[%u],msg_len=[%d],msg_cnt=[%d],clothes_cnt=[%d],len=[%ld]",
			RECVBUF_USERID, user_msg_list.len, user_msg_list.count, p_out_header->used_clothes_count, p_offset-&(out[0]));
	STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0]));
}

int Croute_func::gf_special_set(userid_t userid, uint32_t role_regtime)
{
	uint32_t lv=0, exp=0, upd_flg=0;
	ret = this->gf_role.get_role_info_for_special_set(userid, role_regtime, &lv, &exp, &upd_flg);
	if (ret != SUCC)
	{
		return ret;
	}
	if (lv < 100 && !upd_flg) {
		ret = this->gf_role.set_int_value(userid, role_regtime, "upd_flg", 1);
		if (ret != SUCC)
		{
			return ret;
		}
		this->gf_task.del_old_daily_loop_task(userid, role_regtime);
		uint32_t exp_lv_from = calc_exp(lv);
		uint32_t exp_lv_to	= calc_exp(lv + 1);

		uint32_t old_exp_lv_from = calc_old_exp(lv);
		uint32_t old_exp_lv_to	= calc_old_exp(lv + 1);

		uint32_t add_exp = ((float)exp - (float)old_exp_lv_from) * ((float)exp_lv_to - (float)exp_lv_from) 
			/ ((float)old_exp_lv_to - (float)old_exp_lv_from);

		
		if (exp >= old_exp_lv_from && exp <= old_exp_lv_to && add_exp <= (exp_lv_to - exp_lv_from)) {

			DEBUG_LOG("special set nor lv%u exp:%u per:%f old:%u %u | new:%u %u | %u", lv, exp,
				((float)exp - (float)old_exp_lv_from) / ((float)old_exp_lv_to - (float)old_exp_lv_from), old_exp_lv_from, old_exp_lv_to, exp_lv_from, exp_lv_to, add_exp);
			uint32_t total_exp = exp_lv_from + add_exp;
			this->gf_role.set_int_value(userid, role_regtime, ROLE_EXP_COLUMN, total_exp);
		} else {
			DEBUG_LOG("special set unnor lv%u exp:%u per:%f old:%u %u | new:%u %u | %u", lv, exp,
				((float)exp - (float)old_exp_lv_from) / (old_exp_lv_to - old_exp_lv_from), old_exp_lv_from, old_exp_lv_to, exp_lv_from, exp_lv_to, add_exp);
			this->gf_role.set_int_value(userid, role_regtime, ROLE_EXP_COLUMN, exp_lv_from);
		}
	}
	return SUCC;
}

/**  
 * @fn 取得用户信息 创建角色以及相关信息
 * @brief  cmd route interface
 * @param  
 * @return 
 * @note player
 */
int Croute_func::gf_get_info_for_login(DEAL_FUN_ARG)
{
	char out[PROTO_MAX_SIZE] = {0};
	int cpy_len = 0;
	gf_get_info_for_login_out_header* p_out_header = (gf_get_info_for_login_out_header*)out;
    gf_attire_id_duration_list* p_clothes_out_item = NULL;
    gf_get_info_for_login_out_item9* pout9 = NULL;
	stru_gf_info_out* p_user_info = &(p_out_header->user_info);
	stru_msg_list user_msg_list = {0};
	stru_msg_list role_msg_list = {0};
	stru_role_info role_info = {0};
	uint32_t count = 0;
	uint32_t len = 0;

	this->gf_buff.del_buff_when_login(USERID_ROLETM);
	this->gf_buff.del_invitee_buff(USERID_ROLETM);

	other_process(USERID_ROLETM);
	if (del_daily_loop(USERID_ROLETM) != SUCC)
	{
		return ret;
	}

	gf_special_set(USERID_ROLETM);

	char* p_offset = &(out[0]) + sizeof(*p_out_header);

	//取得用户全局信息
    uint32_t forbid_flag = 0;
	ret = this->gongfu.get_user_info(USERID_ROLETM, &(p_out_header->user_info), &forbid_flag,&user_msg_list);
	if (ret != SUCC || forbid_flag != 0)
	{
		return ret;
	}
	//DEBUG_LOG("-----------1:%d  %d ",user_msg_list.len, user_msg_list.count);
	
	//取得角色信息
	ret = this->gf_role.get_role_info(USERID_ROLETM, &role_info, &role_msg_list);
	if (ret != SUCC)
	{
		return ret;
	}
    p_user_info->role_regtime = role_info.role_regtime;
	p_user_info->role_type = role_info.role_type;
	p_user_info->power_user = role_info.power_user;
	p_user_info->Ol_count = role_info.Ol_count;
	strncpy(p_user_info->nick, role_info.nick, sizeof(p_user_info->nick));
	p_user_info->nick_change_tm = role_info.nick_change_tm;
	p_user_info->skill_point = role_info.skill_point;
	p_user_info->map_id = role_info.map_id;
	p_user_info->xpos = role_info.xpos;
	p_user_info->ypos = role_info.ypos;
	p_user_info->xiaomee = role_info.xiaomee;
	p_user_info->level = role_info.level;
	p_user_info->exp = role_info.exp;
	p_user_info->alloter_exp = role_info.alloter_exp;
	p_user_info->hp = role_info.hp;
	p_user_info->mp = role_info.mp;
	p_user_info->honour = role_info.honour;
	p_user_info->fight = role_info.fight;
	p_user_info->win = role_info.win;
	p_user_info->fail = role_info.fail;
	p_user_info->winning_streak = role_info.winning_streak;
	p_user_info->fumo_points_today = role_info.fumo_points_today;
	p_user_info->fumo_points_total = role_info.fumo_points;
	p_user_info->fumo_tower_top = role_info.fumo_tower_top;
	p_user_info->fumo_tower_used_tm = role_info.fumo_tower_used_tm;
	memcpy (p_user_info->uniqueitem, role_info.uniqueitem, sizeof(p_user_info->uniqueitem));
	memcpy (p_user_info->itembind, role_info.itembind, sizeof(p_user_info->itembind));
	p_user_info->double_exp_time = role_info.double_exp_time;
	p_user_info->day_flag = role_info.day_flag;
	p_user_info->max_times_chapter = role_info.max_times_chapter;
	p_user_info->show_state = role_info.show_state;	
	p_user_info->open_box_times = role_info.open_box_times;
	p_user_info->strengthen_cnt = role_info.strengthen_cnt;	
	p_user_info->achieve_point = role_info.achieve_point;
	p_user_info->vitality_point = role_info.vitality_point;
	p_user_info->last_update_tm = role_info.last_update_tm;
	p_user_info->achieve_title = role_info.achieve_title;
	p_user_info->forbiden_add_friend_flag = p_out_header->user_info.forbiden_add_friend_flag;
	p_user_info->home_active_point = role_info.home_active_point;
	p_user_info->home_last_tm = role_info.home_last_tm;
	p_user_info->team_id = role_info.team_id;
    memcpy(p_user_info->god_guard, role_info.god_guard, sizeof(p_user_info->god_guard));

	if (p_user_info->xiaomee > 1000000000) {

		DEBUG_LOG("USER　%u TOO MUCH xiaomee %u", RECVBUF_USERID, p_user_info->xiaomee);
		p_user_info->xiaomee = 1000000000;
		this->gf_role.set_coins(RECVBUF_USERID, RECVBUF_ROLETM, p_user_info->xiaomee);
	}
	//合成离线消息，存入out buff
	count = user_msg_list.count;
	len = user_msg_list.len;
	uint32_t total_len = user_msg_list.len + role_msg_list.len;
	user_msg_list.count = user_msg_list.count + role_msg_list.count;
	user_msg_list.len = total_len==0?msg_list_head_len:total_len ;
	p_out_header->msg_size = user_msg_list.len;


	
	//用户全局离线消息 存入out
	cpy_len = len==0 ? msg_list_head_len:len;
	memcpy(p_offset, &user_msg_list, cpy_len);
	p_offset += cpy_len;
	
	//角色离线消息 存入out
	cpy_len = role_msg_list.len>msg_list_head_len?(role_msg_list.len - msg_list_head_len):0;
	memcpy(p_offset, role_msg_list.buf, cpy_len);
	p_offset += cpy_len;
	
	//初始化数据库离线消息字段
	if ( count!=0 ) {
		stru_msg_list t_msglist = {0};
		t_msglist.len = msg_list_head_len;
		t_msglist.count = 0;
		this->gongfu.update_msglist(RECVBUF_USERID, &t_msglist);		
    }	
	if ( role_msg_list.count!=0 ){
		stru_msg_list t_msglist = {0};
		t_msglist.len = msg_list_head_len;
		t_msglist.count = 0;
		this->gf_role.update_msglist(USERID_ROLETM, &t_msglist);		
  	}	
	
	//取得用户身上装备列表
	ret = this->gf_attire.get_used_clothes_list_with_duration(USERID_ROLETM, &(p_out_header->used_clothes_count), &p_clothes_out_item);
	if (ret != SUCC) {
		return ret;
	}

	if (p_clothes_out_item) {
		cpy_len = sizeof(*p_clothes_out_item) * p_out_header->used_clothes_count;
		memcpy(p_offset, p_clothes_out_item, cpy_len);
		free(p_clothes_out_item);
		p_offset += cpy_len;
	}

/*	
	ret = gf_get_skill_bind_private(USERID_ROLETM,  &(p_out_header->skill_count),
		(gf_get_skill_bind_key_out_item*)p_offset);
	if (ret != SUCC)
	{
		return ret;
	}
*/
	ret = gf_get_all_skills_info(USERID_ROLETM,  &(p_out_header->skill_count),
		(gf_get_skill_bind_key_out_item*)p_offset);
	if (ret != SUCC)
	{
		return ret;
	}	
	
	p_offset += sizeof(gf_get_skill_bind_key_out_item) * p_out_header->skill_count;

    ret = this->gf_get_task_classify_list(USERID_ROLETM, &(p_out_header->done_tasks_num),
        &(p_out_header->doing_tasks_num), &(p_out_header->cancel_tasks_num), p_offset, &cpy_len);
    if (ret != SUCC) {
        return ret;
    }
    p_offset += cpy_len;

    ret = this->gf_get_role_package_list(USERID_ROLETM, &(p_out_header->pkg_clothes_cnt),
        &(p_out_header->pkg_item_cnt), p_offset, &cpy_len);
    if (ret != SUCC) {
        return ret;
    }
    p_offset += cpy_len;

    ret = this->gf_killboss.get_killed_boss(USERID_ROLETM, &(p_out_header->fumo_stage_cnt),
        (gf_get_killed_boss_out_item**)&pout9);
    if (ret != SUCC) {
        return ret;
    }
    if (pout9) {
        cpy_len = sizeof(*pout9) * p_out_header->fumo_stage_cnt;
        memcpy(p_offset, pout9, cpy_len);
        free(pout9);
        p_offset += cpy_len;
    }

	gf_get_db_buff_list_out_item* p_buff = 0;
	ret = this->gf_buff.get_buff_list(USERID_ROLETM, &(p_out_header->db_buff_cnt), &p_buff);
   	if (p_buff) {
	    cpy_len = sizeof(*p_buff) * p_out_header->db_buff_cnt;
	    memcpy(p_offset, p_buff, cpy_len);
	    free(p_buff);
	    p_offset += cpy_len;
    }
	
	DEBUG_LOG("get user info:uid=[%u],msg_len=[%d],msg_cnt=[%d],clothes_cnt=[%d],len=[%ld]",
			RECVBUF_USERID, user_msg_list.len, user_msg_list.count, p_out_header->used_clothes_count, p_offset-&(out[0]));
	STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0]));
}

/**
 *@fn 设置角色昵称
 *@brief cmd route interface
 */
int Croute_func::gf_set_nick(DEAL_FUN_ARG)
{
	gf_set_nick_in* p_in = PRI_IN_POS;
	ret = this->gf_role.update_nick(RECVBUF_USERID, RECVBUF_ROLETM, p_in->nick);
	if (ret != SUCC) {
		return ret;
	}
	if (p_in->use_item) {
		ret = this->gf_item.reduce_item_int_value(USERID_ROLETM, p_in->use_item, "count", 1);
		if (ret != SUCC) {
			STD_ROLLBACK();
			return ret;
		}
	}
	STD_RETURN_WITH_STRUCT(ret, p_in->use_item);
}
/**
 *@fn 获取角色昵称
 *@brief cmd route interface
 */
int Croute_func::gf_get_nick(DEAL_FUN_ARG)
{
	gf_get_nick_out out = {{0}};
	ret = this->gf_role.get_last_login_nick(RECVBUF_USERID, out.nick);
    STD_RETURN_WITH_STRUCT(ret, out);
}

/** 
 * @fn 增加各类型的好友（朋友，师傅，徒弟....）
 * @brief  cmd route interface
 */
int Croute_func::gf_add_friend(DEAL_FUN_ARG)
{
    uint32_t vip_type = 0;
	gf_add_friend_in* p_in = PRI_IN_POS;

	ret = this->gongfu.get_vip_type(RECVBUF_USERID, &vip_type);
	if (ret != SUCC)
	{
		return ret;
	}

	ret = this->gf_friend.add(RECVBUF_USERID, p_in->id, vip_type, TYPE_FRIEND);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

    STD_RETURN_WITH_STRUCT(ret, *p_in);
}

int Croute_func::gf_forbiden_add_friend_flag(DEAL_FUN_ARG)
{
	gf_forbiden_add_friend_flag_in* p_in = PRI_IN_POS;

	ret = gongfu.set_int_value(RECVBUF_USERID, p_in->flag, "forbiden_add_friend_flag"); 
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return 0;
	}
	return 0;
}

int Croute_func::gf_query_forbiden_friend_flag(DEAL_FUN_ARG)
{
	gf_query_forbiden_friend_flag_out  out = { 0 };
	ret = gongfu.get_int_value(RECVBUF_USERID,  "forbiden_add_friend_flag",  &out.flag);
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		out.err_code = ret;
	}
	out.mimi_id = RECVBUF_USERID;
	STD_RETURN_WITH_STRUCT(0, out);	
}

/** 
 * @fn 添加黑名单
 * @brief  cmd route interface
 */
int Croute_func::gf_add_black(DEAL_FUN_ARG)
{
    uint32_t vip_type = 0;
	gf_add_black_out_header out_header={0};
	gf_add_black_in* p_in = PRI_IN_POS;
	
	ret = this->gongfu.get_vip_type(RECVBUF_USERID, &vip_type);
	if (ret != SUCC)
	{
		return ret;
	}

	ret = this->gf_friend.add(RECVBUF_USERID, p_in->id, vip_type, TYPE_BLACK);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	out_header.id = p_in->id;
	STD_RETURN_WITH_STRUCT(ret, out_header);

}


/** 
 * @fn 删除各类型的好友(朋友，师徒...)
 * @brief  cmd route interface
 */
int Croute_func::gf_del_friend(DEAL_FUN_ARG)
{
	gf_del_friend_in* p_in = PRI_IN_POS;
	ret = this->gf_friend.del(RECVBUF_USERID, p_in->id, TYPE_FRIEND);
	STD_RETURN(ret);
}


/** 
 * @fn 删除黑名单
 * @brief  cmd route interface
 */
int Croute_func::gf_del_black(DEAL_FUN_ARG)
{
	gf_del_black_in* p_in = PRI_IN_POS;
	ret = this->gf_friend.del(RECVBUF_USERID, p_in->id, TYPE_BLACK);
	STD_RETURN(ret);
}

int Croute_func::gf_del_friend_whatever(DEAL_FUN_ARG)
{
	gf_del_friend_in* p_in = PRI_IN_POS;
	ret = this->gf_friend.del(RECVBUF_USERID, p_in->id);
	STD_RETURN(ret);
}


/** 
 * @fn  取得黑名单列表
 * @brief  cmd route interface
 */
int Croute_func::gf_get_blacklist(DEAL_FUN_ARG)
{
	gf_get_friendlist_out_header out_header = {0};
	gf_get_friendlist_out_item* p_out_item = NULL;
	
	ret = this->gf_friend.get_friendid_list(RECVBUF_USERID, TYPE_BLACK, &(out_header.count), (stru_friedid_list**)&p_out_item);
	if (ret != SUCC)
	{
		return ret;
	}


	DEBUG_LOG("get friendlist:uid=[%u],clothes_cnt=[%d]",
			RECVBUF_USERID, out_header.count);
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);

}


/** 
 * @fn 取得好友列表
 * @brief  cmd route interface
 */
int Croute_func::gf_get_friendlist(DEAL_FUN_ARG)
{
	gf_get_friendlist_out_header out_header = {0};
	gf_get_friendlist_out_item* p_out_item = NULL;

	ret = this->gf_friend.get_friendid_list(RECVBUF_USERID, TYPE_FRIEND, &(out_header.count), (stru_friedid_list**)&p_out_item);
	if (ret != SUCC)
	{
		return ret;
	}


	DEBUG_LOG("get friendlist:uid=[%u],friend_cnt=[%d]",
			RECVBUF_USERID, out_header.count);
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

/** 
 * @fn 取得装备列表
 * @brief  cmd route interface
 */
int Croute_func::gf_get_clothes_list(DEAL_FUN_ARG)
{
	gf_get_clothes_list_out_header out_header = {0};
	gf_get_clothes_list_out_item* p_out_item = NULL;

	ret = this->gf_attire.get_clothes_list(USERID_ROLETM, &(out_header.count), &p_out_item);
	if (ret != SUCC) {
		return ret;
	}

	DEBUG_LOG("get clothes list:uid=[%u],clothes_cnt=[%d]",
			RECVBUF_USERID, out_header.count);
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

/** 
 * @fn 取得强化材料列表
 * @brief  cmd route interface
 */
int Croute_func::gf_get_strengthen_material_list(DEAL_FUN_ARG)
{
	gf_get_strengthen_material_list_out_header out_header = {0};
	gf_get_strengthen_material_list_out_item* p_out_item = NULL;

	ret = this->gf_material.get_user_material_list(USERID_ROLETM, &(out_header.count), &p_out_item);
	if (ret != SUCC) {
		return ret;
	}

	DEBUG_LOG("get strengthen material list:uid=[%u],material_cnt=[%d]",
			RECVBUF_USERID, out_header.count);
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}
/**
 * @brief strengthen attire
 */
int Croute_func::gf_strengthen_attire(DEAL_FUN_ARG)
{
    gf_strengthen_attire_in* p_in = PRI_IN_POS;
    gf_strengthen_attire_out out = {0};
    uint32_t left_coins = 0;
    ROLE_REDUCE_VALUE(XIAOMEE, RECVBUF_USERID, RECVBUF_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, p_in->coin, &left_coins);
    out.coin = left_coins;

    if (p_in->strengthen_fail_cnt == 0) {
        ret = this->gf_role.set_int_add_value(USERID_ROLETM, 1, "strengthen_count");
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }

    ret = this->gf_attire.set_attire_int_value(USERID_ROLETM,p_in->uniquekey,"attirelv",p_in->attirelv);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
    ret = this->gf_material.reduce_material_count(USERID_ROLETM, p_in->material_id, p_in->material_cnt);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
    if (p_in->stoneid_1 != 0) {
        ret = this->gf_material.reduce_material_count(USERID_ROLETM, p_in->stoneid_1, p_in->stone_1_cnt);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }
    if (p_in->stoneid_2 != 0) {
        ret = this->gf_material.reduce_material_count(USERID_ROLETM, p_in->stoneid_2, 1);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }
    out.attireid = p_in->attireid;
    out.uniquekey = p_in->uniquekey;
    out.attirelv = p_in->attirelv;
    out.material_id = p_in->material_id;
    out.material_cnt = p_in->material_cnt;
    out.stoneid_1 = p_in->stoneid_1;
    out.stone_1_cnt = p_in->stone_1_cnt;
    out.stoneid_2 = p_in->stoneid_2;
    STD_RETURN_WITH_STRUCT(ret, out);
}

//added by cws 0620
int Croute_func::gf_strengthen_attire_without_material(DEAL_FUN_ARG){
	gf_strengthen_attire_without_material_in *p_in = PRI_IN_POS;
	gf_strengthen_attire_without_material_out p_out = {0};
	ret = this->gf_attire.set_attire_int_value(USERID_ROLETM,p_in->uniquekey,"attirelv",p_in->attirelv);
	if (ret != SUCC) {
		STD_ROLLBACK();
		return ret;
		}
	p_out.attirelv = p_in->attirelv;
	p_out.uniquekey = p_in->uniquekey;
	STD_RETURN_WITH_STRUCT(ret, p_out);
}
	

int Croute_func::gf_get_clothes_list_ex(DEAL_FUN_ARG)
{
	gf_get_clothes_list_ex_out_header out_header = {0};
	gf_get_clothes_list_ex_out_item* p_out_item = NULL;

	ret = this->gf_attire.get_clothes_list_ex(USERID_ROLETM, &(out_header.count), &p_out_item);
	if (ret != SUCC)
	{
		return ret;
	}
	
	DEBUG_LOG("get clothes list:uid=[%u],clothes_cnt=[%d],len=[%ld]",
			RECVBUF_USERID, out_header.count, sizeof(gf_get_clothes_list_out_header) + (sizeof(*p_out_item)*out_header.count));

	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
//	STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0]));
}

/** 
 * @fn 使用装备
 * @brief  cmd route interface
 */
 int Croute_func::gf_wear_clothes(DEAL_FUN_ARG)
{
	gf_wear_clothes_in_header* p_in = PRI_IN_POS;
	gf_wear_clothes_out_header out_header;
	gf_wear_clothes_out_item* p_out_item;

	CHECK_PRI_IN_LEN_WITHOUT_HEADER( p_in->count * sizeof(gf_wear_clothes_in_item));

	uint32_t db_attire_cnt = 0;
	ret = this->gf_attire.get_attire_cnt(USERID_ROLETM, &db_attire_cnt, 0xffffffff);
	
	if (db_attire_cnt - p_in->count > p_in->max_bag_grid_count)
	{
		return GF_ATTIRE_MAX_ERR;
	}
	

	if (p_in->count == 0)
	{
		out_header.count = 0;
		ret = this->gf_attire.set_all_clothes_noused(USERID_ROLETM);
		STD_RETURN_WITH_STRUCT(ret, out_header);
	}
	else if (p_in->count>MAX_WEAR_CLOTHES_NUM)
	{
		DEBUG_LOG("gf_wear_clothes: number:%d > max:10", p_in->count);
		return GF_CLOTHES_MAX_ERR;
	}

	ret = this->gf_attire.set_all_clothes_noused(USERID_ROLETM);
	if(ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	
	gf_wear_clothes_in_item* p_in_item = (gf_wear_clothes_in_item*)((char*)PRI_IN_POS + sizeof(*p_in));

	ret = this->gf_attire.get_clothes_by_timelist(USERID_ROLETM, 
			p_in_item, p_in->count, &p_out_item, &(out_header.count));
	if(ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

	for (uint32_t i = 0; i < p_in->count; i++) {
		ret = this->gf_attire.set_clothes_flag_equip_part(USERID_ROLETM, p_in_item->id, p_in_item->equip_part);
		if(ret!=SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}
		p_in_item++;
	}
	
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::gf_take_off_clothes(DEAL_FUN_ARG)
{
    gf_take_off_clothes_in* p_in = PRI_IN_POS;
    ret = this->gf_attire.set_clothes_noused(USERID_ROLETM, p_in->unique_id);
    STD_RETURN(SUCC);
}

int  Croute_func::gf_move_item_warehouse_to_bag(DEAL_FUN_ARG)
{
	gf_move_item_warehouse_to_bag_in* p_in = PRI_IN_POS;
	gf_move_item_warehouse_to_bag_out out = {0};
	uint32_t left_coins = 0; 
	
	ROLE_REDUCE_VALUE(XIAOMEE, RECVBUF_USERID, RECVBUF_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, p_in->cost, &left_coins);

	ret = this->gf_increase_item(USERID_ROLETM, p_in->item_id, p_in->count, 9999, 
		p_in->max_item_bag_grid_count, item_change_log_opt_type_add, false);
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	ret = gf_warehouse.delete_warehouse_item(USERID_ROLETM, p_in->item_id, p_in->count);
	if( ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	out.coins_left = left_coins;
	out.count =   p_in->count;
	out.item_id =  p_in->item_id;
	STD_RETURN_WITH_STRUCT(ret,out); 
}

int Croute_func::gf_move_item_bag_to_warehouse(DEAL_FUN_ARG)
{
	gf_move_item_bag_to_warehouse_in* p_in = PRI_IN_POS;
	gf_move_item_bag_to_warehouse_out  out = {0};
	uint32_t  left_coins = 0;
	uint32_t  count = 0;

    ret = gf_warehouse.get_warehouse_used_count(RECVBUF_USERID, &count);
    if(ret != SUCC)
    {
        return ret;
    }
    if(  count >= p_in->max_warehouse_grid_count)
    {
        return GF_WAREHOUSE_MAX_COUNT_ERROR;
    }
    ROLE_REDUCE_VALUE(XIAOMEE, RECVBUF_USERID, RECVBUF_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, p_in->cost, &left_coins);

	ret = gf_item.reduce_item_int_value(USERID_ROLETM, p_in->item_id, "count", p_in->count, false);
	if (ret!=SUCC)
    {
    	STD_ROLLBACK();
        return ret;
    }
    ret = gf_warehouse.add_warehouse_item(USERID_ROLETM, p_in->item_id, p_in->count);
    if (ret!=SUCC)
    {
        STD_ROLLBACK();
        return ret;
    }

    out.coins_left = left_coins;
    out.count =   p_in->count;
    out.item_id =  p_in->item_id;
    STD_RETURN_WITH_STRUCT(ret,out);
}

int  Croute_func::get_warehouse_item_list(DEAL_FUN_ARG)
{
	gf_get_warehouse_item_list_out_header out_header = {0};
	gf_get_warehouse_item_list_out_item * p_out_item = NULL;
	
	ret = gf_warehouse.get_warehouse_item_list(USERID_ROLETM, &p_out_item, &(out_header.count));
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::get_warehouse_clothes_item_list(DEAL_FUN_ARG)
{
	gf_get_warehouse_clothes_item_list_out_header out_header  = {0};
	gf_get_warehouse_clothes_item_list_out_item *p_out_item = NULL;

	ret = gf_clothes_warehouse.get_clothes_warehouse_item_list(USERID_ROLETM, &p_out_item, &(out_header.count));
	if(ret != SUCC)
    {
    	STD_ROLLBACK();
        return ret;
    }
    STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::gf_move_clothes_item_bag_to_warehouse(DEAL_FUN_ARG)
{
	gf_move_clothes_item_bag_to_warehouse_in* p_in = PRI_IN_POS;
	gf_move_clothes_item_bag_to_warehouse_out out = {0};
	uint32_t  left_coins = 0;
	uint32_t  count = 0;

	ret = gf_warehouse.get_warehouse_used_count(RECVBUF_USERID, &count);
	if(ret != SUCC){
		return ret;
	}
	if(  count >= p_in->max_warehouse_grid_count){
		return GF_WAREHOUSE_MAX_COUNT_ERROR;
	}


	ROLE_REDUCE_VALUE(XIAOMEE, RECVBUF_USERID, RECVBUF_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, p_in->cost, &left_coins);
	
	add_attire_t attire = {0};	
	ret = gf_attire.get_attire_by_index(USERID_ROLETM, p_in->id, p_in->item_id,  &attire);	
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	//delete item from clothes bag
	ret = gf_attire.del_attire_with_index(USERID_ROLETM, p_in->id);	
	if(ret != SUCC)
    {
    	STD_ROLLBACK();
        return ret;
    }
	//add item to warehouse
	uint32_t  new_id = 0;
	ret = gf_clothes_warehouse.add_clothes_warehouse_item(USERID_ROLETM, attire.attireid,  attire.gettime, attire.attire_rank, attire.duration, attire.endtime, attire.attire_lv, &new_id);
	if(ret != SUCC)
    {
    	STD_ROLLBACK();
        return ret;
    }

	out.old_id = p_in->id;
	out.coins_left = left_coins;
    out.id =   new_id;
    out.item_id =  p_in->item_id;
	out.get_time =  attire.gettime;
	out.attire_rank = attire.attire_rank;
	out.duration = attire.duration;
	out.end_time = attire.endtime;
	out.attire_lv = attire.attire_lv;
    STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::gf_move_clothes_item_warehouse_to_bag(DEAL_FUN_ARG)
{
	gf_move_clothes_item_warehouse_to_bag_in* p_in = PRI_IN_POS;
    gf_move_clothes_item_warehouse_to_bag_out out = {0};
    uint32_t  left_coins = 0;

	ROLE_REDUCE_VALUE(XIAOMEE, RECVBUF_USERID, RECVBUF_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, p_in->cost, &left_coins);

	uint32_t attireid = 0;
	uint32_t get_time = 0;
	uint32_t attire_rank = 0;
	uint32_t duration = 0;
	uint32_t end_time = 0;
	uint32_t attire_lv = 0;

	ret = gf_clothes_warehouse.get_clothes_warehouse_item_by_id(USERID_ROLETM, p_in->id, &attireid, &get_time, &attire_rank, &duration, &end_time, &attire_lv);
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	//delete item from warehouse
	ret = gf_clothes_warehouse.delete_clothes_warehouse_item(USERID_ROLETM, p_in->item_id, p_in->id);
	if(ret != SUCC)
    {
    	STD_ROLLBACK();
        return ret;
    }
	//add item to clothes bag
	uint32_t new_id = 0;	
	add_attire_t attire = {0};
	attire.attireid = attireid;
	attire.gettime = get_time;
	attire.attire_rank = attire_rank;
	attire.duration = duration;	
	attire.usedflag  = 0;
	attire.endtime = end_time;
	attire.attire_lv = attire_lv;
	ret = gf_attire.add_attire(USERID_ROLETM,  &attire, p_in->max_bag_grid_count, &new_id);
	if(ret != SUCC)
    {
    	STD_ROLLBACK();
        return ret;
    }

	out.old_id = p_in->id;
	out.coins_left = left_coins;
    out.id =   new_id;
    out.item_id =  p_in->item_id;
    out.get_time =  attire.gettime;
    out.attire_rank = attire.attire_rank;
    out.duration = attire.duration;
	out.end_time = attire.endtime;
	out.attire_lv = attire.attire_lv;
    STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::gf_set_buy_item_limit_data(DEAL_FUN_ARG)
{
	gf_set_buy_item_limit_data_in *p_in = PRI_IN_POS;
	ret = gf_buy_item_limit.replace_buy_item_limit_data(USERID_ROLETM, 
														p_in->item_id,
														p_in->item_limit_type,
														p_in->item_count,
														p_in->last_time	
														);	

	return SUCC;
}

int Croute_func::gf_get_buy_item_limit_list(DEAL_FUN_ARG)
{
	gf_buy_item_limit_list_out_header out_header = { 0 };
	gf_buy_item_limit_list_out_element* p_out_element = NULL;
	ret = gf_buy_item_limit.get_buy_item_limit_list(USERID_ROLETM, &p_out_element, &out_header.count);
	if( ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_element, out_header.count);
}

int  Croute_func::gf_get_ap_toplist(DEAL_FUN_ARG)
{
	gf_get_ap_toplist_in* p_in = PRI_IN_POS;
	
	gf_get_ap_toplist_out_header out_header = { 0 };
	gf_get_ap_toplist_out_element* p_out_element = NULL;
	out_header.begin_index = p_in->begin_index;
	out_header.end_index = p_in->end_index;
	
	ret = gf_ap_toplist.get_ap_toplist( p_in->begin_index, p_in->end_index, &p_out_element, &out_header.count);
	if(ret != SUCC){
		STD_ROLLBACK();
		return ret;
	}
	
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_element, out_header.count);
}

int  Croute_func::gf_get_achievement_data_list(DEAL_FUN_ARG)
{
	gf_get_achievement_data_list_out_header out_header = { 0 };
	gf_get_achievement_data_list_out_element* p_out_element = NULL;

	ret = gf_achieve.get_achievement_data_list(USERID_ROLETM, &p_out_element, &out_header.count);
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_element, out_header.count);
}


int  Croute_func::gf_replace_achievement_data(DEAL_FUN_ARG)
{
	gf_replace_achievement_data_in* p_in = PRI_IN_POS;
	gf_achieve.replace_achievement_data(USERID_ROLETM, p_in->achieve_type, p_in->get_time);
//	gf_role.set_int_add_value(USERID_ROLETM, p_in->add_point, "achieve_point");
//	gf_role.set_int_value(USERID_ROLETM, "last_add_ap_time", p_in->get_time);
	return SUCC;
}
/**
 * @brief achieve title
 */
int  Croute_func::gf_get_achieve_title(DEAL_FUN_ARG)
{
	gf_get_achieve_title_out_header out_header = { 0 };
	gf_get_achieve_title_out_item* p_out_item = NULL;

	ret = gf_title.get_achieve_title(USERID_ROLETM, &p_out_item, &out_header.count);
	if(ret != SUCC) {
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int  Croute_func::gf_add_achieve_title(DEAL_FUN_ARG)
{
    gf_add_achieve_title_in_header* p_in = PRI_IN_POS; 
    CHECK_PRI_IN_LEN_WITHOUT_HEADER( p_in->cnt * sizeof(gf_add_achieve_title_in_item) );
    //gf_add_achieve_title_out out = {0};

    gf_add_achieve_title_in_item* p_in_item = (gf_add_achieve_title_in_item*)(p_in + 1);
    for (uint32_t i = 0; i < p_in->cnt; i++) {
        ret = gf_title.add_achieve_title(USERID_ROLETM, p_in->type, p_in_item->titleid, p_in->get_time);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
        p_in_item += 1;
    }
    //STD_RETURN_WITH_STRUCT(ret,out);
    STD_RETURN(ret);
}

int Croute_func::gf_set_cur_achieve_title(DEAL_FUN_ARG)
{
    gf_set_cur_achieve_title_in* p_in = PRI_IN_POS;
    gf_set_cur_achieve_title_out out = {0};
    ret = gf_role.set_int_value(USERID_ROLETM, "achieve_title", p_in->count);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
    out.count = p_in->count;
    STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::gf_get_stat_info(DEAL_FUN_ARG)
{
	gf_get_stat_info_out_header out_header = {0};
	gf_get_stat_info_out_item * p_out_items = NULL;
	ret = gf_stat.list_player_stat_val(USERID_ROLETM, &p_out_items, &out_header.stat_cnt); 
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_items, out_header.stat_cnt);
}

int Croute_func::gf_set_stat_info(DEAL_FUN_ARG)
{
	gf_set_stat_info_in * p_in = PRI_IN_POS;
	ret = gf_stat.add_player_stat_val(USERID_ROLETM, p_in->stat_id, p_in->add_cnt);
	return 0;
}

int  Croute_func::gf_get_kill_boss_list(DEAL_FUN_ARG)
{
	gf_get_kill_boss_list_out_header out_header = { 0 };
	gf_get_kill_boss_list_out_element* p_out_element = NULL;
	
	ret = gf_kill_boss.get_kill_boss_list(USERID_ROLETM,  &p_out_element, &out_header.count);
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;	
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_element, out_header.count);
}

int  Croute_func::gf_replace_kill_boss(DEAL_FUN_ARG)
{
	gf_replace_kill_boss_in* p_in = PRI_IN_POS;
	ret = gf_kill_boss.replace_kill_boss(USERID_ROLETM, p_in->stage_id, p_in->boss_id, p_in->kill_time, p_in->pass_cnt);
	if (ret != SUCC) {
		STD_ROLLBACK();
		return ret;
	}
	return SUCC;	
}

//int Croute_func::gf_update_home_active_point(DEAL_FUN_ARG)
//{
//	gf_update_home_active_point_in* p_in = PRI_IN_POS;
//	ret = gf_role.update_home_active_point(USERID_ROLETM, p_in->active_point, p_in->last_tm);	
//	return ret;
//}
//
int  Croute_func::gf_get_home_data(DEAL_FUN_ARG)
{
	gf_get_home_data_out out = {0};
	ret = gf_home.get_player_home_data(USERID_ROLETM, &out);	
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

	if (out.home_level == 0) {
		gf_home.insert_player_home_data(USERID_ROLETM);
	}

	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::gf_set_home_data(DEAL_FUN_ARG)
{
	gf_set_home_data_in * p_in = PRI_IN_POS;
	gf_set_home_data_out out = {0};
	if (p_in->dec_fumo_point) {
		ROLE_REDUCE_VALUE(FUMO_POINTS,RECVBUF_USERID, RECVBUF_ROLETM, GF_FUMO_NOENOUGH_ERR, p_in->dec_fumo_point, &(out.left_fumo));
	}
	ret = gf_home.set_player_home_data(USERID_ROLETM, p_in); 
	if (ret != SUCC) {
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_set_home_update_tm(DEAL_FUN_ARG)
{
	gf_set_home_update_tm_in * p_in = PRI_IN_POS;
	ret = gf_home.set_player_home_update_tm(USERID_ROLETM, p_in->last_randseed_tm);
	return ret;
}

//int Croute_func::gf_replace_home_exp_level(DEAL_FUN_ARG)
//{
//	gf_replace_home_exp_level_in* p_in = PRI_IN_POS;
//	gf_replace_home_exp_level_out out = { 0 };
//
//	uint32_t left_fumo_point = 0;
//	if(p_in->fumo_point > 0)
//	{
//		ret = gf_role.reduce_int_value(USERID_ROLETM, 10199, "fumo_points", p_in->fumo_point, &left_fumo_point);
//		if(ret != SUCC)
//		{
//			STD_ROLLBACK();
//			return ret;
//		}	
//	}
//
//	ret = gf_home.replace_player_home_exp_level_data(USERID_ROLETM, p_in->exp, p_in->level);
//	if(ret != SUCC)
//	{
//		STD_ROLLBACK();
//		return ret;
//	}
//
//	out.exp = p_in->exp;
//	out.level =  p_in->level;
//	out.fumo_point = p_in->fumo_point;
//
//	STD_RETURN_WITH_STRUCT(ret,out);
//}

int Croute_func::gf_mail_head_list(DEAL_FUN_ARG)
{
	gf_mail_head_list_out_header out_header = { 0 };
	gf_mail_head_list_out_element* p_out_element = NULL;

	ret = gf_mail.get_mail_head_list(USERID_ROLETM,  &p_out_element, &out_header.count);
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_element, out_header.count);
}

int Croute_func::gf_mail_body(DEAL_FUN_ARG)
{
	gf_mail_body_in* p_in = PRI_IN_POS;
	gf_mail_body_out out = {0};
	ret = gf_mail.get_mail_body(RECVBUF_USERID,  p_in->mail_id, &out);
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	ret = gf_mail.set_mail_read_flag(RECVBUF_USERID, p_in->mail_id, 1);
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::gf_delete_mail(DEAL_FUN_ARG)
{
	gf_delete_mail_in* p_in = PRI_IN_POS;
	gf_delete_mail_out out = {0};

	ret = gf_mail.delete_mail(RECVBUF_USERID,  p_in->mail_id);
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	out.mail_id = p_in->mail_id;
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_take_mail_enclosure(DEAL_FUN_ARG)
{	
	char buf[PROTO_MAX_SIZE] = {0};
	int idx = 0;

	gf_take_mail_enclosure_in_header* p_in = PRI_IN_POS;
	gf_mail_nums_enclosure* p_nums = NULL;
	gf_mail_items_enclosure* p_items = NULL;
	gf_mail_equips_enclosure* p_equips = NULL;

	char* p_begin_pos = (char*)(p_in);

	if(p_in->mail_nums_count > 0){
		p_nums =(gf_mail_nums_enclosure*)(p_begin_pos + sizeof(gf_take_mail_enclosure_in_header));
	}
	if(p_in->mail_items_count > 0){
		p_items =(gf_mail_items_enclosure*) (p_begin_pos + sizeof(gf_take_mail_enclosure_in_header) +
			   	(p_in->mail_nums_count)*sizeof(gf_mail_nums_enclosure));
	}
	if(p_in->mail_equips_count > 0){
		p_equips =(gf_mail_equips_enclosure*)(p_begin_pos + sizeof(gf_take_mail_enclosure_in_header) + (p_in->mail_nums_count)*sizeof(gf_mail_nums_enclosure) + (p_in->mail_items_count)*sizeof(gf_mail_items_enclosure));
	}

	
	pack_h(buf, p_in->mail_id, idx);
	pack_h(buf, p_in->mail_nums_count, idx);
	for(uint32_t i = 0; i < p_in->mail_nums_count; i++)
	{
		if( p_nums[i].type == 2 ){//EXP
			ret = gf_role.increase_int_value(USERID_ROLETM, ROLE_EXP_COLUMN, p_nums[i].number);
			if(ret != SUCC)
			{
				STD_ROLLBACK();
				return ret;
			}
		}
		else if( p_nums[i].type == 1){//功夫豆
			ret = gf_role.increase_int_value(USERID_ROLETM, ROLE_XIAOMEE_COLUMN, p_nums[i].number);
			if(ret != SUCC)
			{
				STD_ROLLBACK();
				return ret;
			}
		}
		else if( p_nums[i].type == 4){//伏魔点
			ret = gf_role.increase_int_value(USERID_ROLETM, ROLE_FUMO_POINTS_COLUMN, p_nums[i].number);
			if(ret != SUCC)
			{
				STD_ROLLBACK();
				return ret;
			}
		}
		else if( p_nums[i].type == 3){//技能点
			ret = gf_role.increase_int_value(USERID_ROLETM, ROLE_SKILL_POINT_COLUMN, p_nums[i].number);
			if(ret != SUCC)
			{
				STD_ROLLBACK();
				return ret;
			}
		}
		pack_h(buf, p_nums[i].type, idx);
		pack_h(buf, p_nums[i].number, idx);
	}	
	//物品	
	pack_h(buf, p_in->mail_items_count, idx);	
	for(uint32_t i = 0; i< p_in->mail_items_count; i++)
	{
        ret = gf_increase_item(USERID_ROLETM,p_items[i].item_id,p_items[i].item_count,set_item_limit(p_items[i].item_id),p_in->max_item_bag_grid);
		if(ret != SUCC)
		{
			STD_ROLLBACK();
			return ret;									            
		}
		pack_h(buf, p_items[i].item_id, idx);
		pack_h(buf, p_items[i].item_count, idx);
	}

	pack_h(buf, p_in->mail_equips_count, idx);
	//装备
	for(uint32_t i = 0; i< p_in->mail_equips_count; i++)
	{
		add_attire_t equip = { 0 };
		equip.attireid = p_equips[i].equip_id;
		equip.attire_rank = p_equips[i].equip_rank;
		//equip.gettime = p_equips[i].equip_get_time;
        equip.gettime = time(NULL);
		equip.duration = p_equips[i].equip_duration;
        if (p_equips[i].equip_lifetime == 0) {
            equip.endtime = 0;
        } else {
            equip.endtime = equip.gettime + ((p_equips[i].equip_lifetime + 1) * 3600 * 24);
        }
		equip.usedflag = 0;
		uint32_t last_insert_id = 0;
		ret = gf_attire.add_attire(USERID_ROLETM, &equip, p_in->max_item_bag_grid, &last_insert_id); 
		if(ret != SUCC){
			STD_ROLLBACK();
			return ret;
		}

		pack_h(buf, p_equips[i].equip_id, idx);
		pack_h(buf, p_equips[i].equip_rank, idx);
		pack_h(buf, equip.gettime, idx);
		pack_h(buf, p_equips[i].equip_duration, idx);
		pack_h(buf, equip.endtime, idx);
		pack_h(buf, last_insert_id, idx);
	}
	//清空邮件附件
	ret = gf_mail.clear_mail_enclosures(RECVBUF_USERID,  p_in->mail_id);
	if(ret != SUCC){
		STD_ROLLBACK();
		return ret;						        
	}

	STD_RETURN_WITH_BUF(ret, buf, idx);	
}

int  Croute_func::gf_send_mail(DEAL_FUN_ARG)
{	
	gf_send_mail_in* p_in = PRI_IN_POS;	
	gf_send_mail_out out = {0};
	//check the mail count
	uint32_t cur_count = 0;
	ret = gf_mail.get_mail_count(RECVBUF_USERID, p_in->receive_id, &cur_count);
	if(ret != SUCC){
		STD_ROLLBACK();
		return ret;
	}
	if(cur_count >= p_in->max_mail_limit){
		STD_ROLLBACK();
		return GF_MAIL_FULL;			    
	}
	ret = gf_mail.insert_user_mail(RECVBUF_USERID, p_in, &out);
	if(ret != SUCC){
		STD_ROLLBACK();
		return ret;
	}	
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::gf_send_system_mail(DEAL_FUN_ARG)
{
	gf_send_system_mail_in* p_in = PRI_IN_POS;
	gf_send_mail_out out = {0};
	//check the mail count
	uint32_t cur_count = 0;
	ret = gf_mail.get_mail_count(RECVBUF_USERID, p_in->receive_id, &cur_count);
	if(ret != SUCC){
		STD_ROLLBACK();
		return ret;
	}
	if(cur_count >= p_in->max_mail_limit){
		STD_ROLLBACK();
		return GF_MAIL_FULL;
	}
	ret = gf_mail.insert_system_mail(USERID_ROLETM, p_in, &out);
	if(ret != SUCC){
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT(ret,out);
}


int Croute_func::gf_reduce_money(DEAL_FUN_ARG)
{
	gf_reduce_money_in* p_in = PRI_IN_POS;
	uint32_t left_money = 0;
	ROLE_REDUCE_VALUE(XIAOMEE, RECVBUF_USERID, RECVBUF_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, p_in->money, &left_money);
	return SUCC;
}


int  Croute_func::gf_get_secondary_pro_list(DEAL_FUN_ARG)
{
	gf_get_secondary_pro_list_out_header out_header = {0};
	gf_get_secondary_pro_list_out_type* p_out_type = NULL;

	ret = gf_secondary_pro.get_player_secondary_pro_list(USERID_ROLETM,  &p_out_type, &out_header.count);
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}	
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_type, out_header.count);
}

int  Croute_func::gf_learn_secondary_pro(DEAL_FUN_ARG)
{
	gf_learn_secondary_pro_in* p_in = PRI_IN_POS;
	gf_learn_secondary_pro_out out = {0};

	uint32_t  left_coins = 0;
    ROLE_REDUCE_VALUE(XIAOMEE, RECVBUF_USERID, RECVBUF_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, p_in->money, &left_coins);

	ret = gf_secondary_pro.insert_player_secondary_pro(USERID_ROLETM, p_in->pro_type);
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	out.pro_type = p_in->pro_type;
	out.left_coin = left_coins;
	STD_RETURN_WITH_STRUCT(ret,out);
}

int  Croute_func::gf_secondary_pro_fuse(DEAL_FUN_ARG)
{
	gf_secondary_pro_fuse_in_header* p_in = PRI_IN_POS;
	gf_secondary_pro_fuse_out_header out_header = {0};
	gf_secondary_pro_fuse_out_material *p_out_material = NULL;


	for(uint32_t i =0; i< p_in->material_count; i++)
	{
		ret = gf_item.reduce_item_int_value(USERID_ROLETM, p_in->materials[i].item_id, "count", p_in->materials[i].item_count);
   		if(ret != SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}		
	}

	gf_random_data* p_random_data = (gf_random_data*)(p_in->materials + p_in->material_count);
	for(uint32_t i =0; i< p_in->random_count; i++)
	{
		ret = this->gf_increase_item(USERID_ROLETM, p_random_data[i].item_id, p_random_data[i].item_count, 9999, p_in->max_item_bag_count);
		if(ret != SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}	
	}	

//	ret = this->gf_increase_item(USERID_ROLETM, p_in->create_item_id, p_in->create_item_count, 9999, p_in->max_item_bag_count);
//	if(ret != SUCC)
//	{
//		STD_ROLLBACK();
//		return ret;
//	}

	ret = gf_secondary_pro.add_player_secondary_pro_exp(USERID_ROLETM, p_in->pro_type, p_in->add_exp, p_in->max_exp_limit);
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}	
	out_header.pro_type   = p_in->pro_type;
	out_header.add_exp    = p_in->add_exp;
	out_header.id         = p_in->id;
//	out_header.create_item_id  = p_in->create_item_id;
//	out_header.create_item_count = p_in->create_item_count;
	out_header.material_count = p_in->material_count;
	out_header.random_count = p_in->random_count;

	p_out_material =  (gf_secondary_pro_fuse_out_material*)malloc((p_in->material_count + p_in->random_count)*sizeof(gf_secondary_pro_fuse_out_material));
	gf_secondary_pro_fuse_out_material *p_tmp =  p_out_material;
	for(uint32_t i=0; i < p_in->material_count; i++)
	{
		p_tmp->item_id    = p_in->materials[i].item_id; 
		p_tmp->item_count = p_in->materials[i].item_count;
		p_tmp++;
	}
	for(uint32_t i=0; i < p_in->random_count; i++)
	{
		p_tmp->item_id    = p_random_data[i].item_id; 
		p_tmp->item_count = p_random_data[i].item_count;
		p_tmp++;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_material, out_header.material_count + p_in->random_count);	
}

/** 
 * @fn  取得角色物品列表
 * @brief  cmd route interface
 */
int Croute_func::gf_get_user_item_list(DEAL_FUN_ARG)
{
	gf_get_user_item_list_out_header out_header = {0};
	gf_get_user_item_list_out_item* p_out_item = NULL;

	ret = this->gf_item.get_user_item_list(USERID_ROLETM, &(out_header.count), &p_out_item);
	if (ret != SUCC)
	{
		return ret;
	}

	DEBUG_LOG("get user itemlist:uid=[%u],item_cnt=[%d]",
			RECVBUF_USERID, out_header.count);
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}
/** 
 * @fn  取得角色背包中物品/装备列表
 * @brief  private function
 */
int Croute_func::gf_get_role_package_list(uint32_t userid, uint32_t roletm, 
    uint32_t* cnt1, uint32_t* cnt2, char* outbuf, int* outlen)
{
    int cpy_len = 0;
	gf_attire_id_duration_list * p_item_1 = NULL;
    gf_get_user_item_list_out_item* p_item_2 = NULL;

    char* p_offset = outbuf;
    ret = this->gf_attire.get_package_clothes_list(userid, roletm, cnt1, &p_item_1);
    if (ret != SUCC) {
        return ret;
    }
    if (p_item_1) {
        cpy_len = sizeof(*p_item_1) * (*cnt1);
        memcpy(p_offset, p_item_1, cpy_len);
        free(p_item_1);
        p_offset += cpy_len;
    }

	ret = this->gf_item.get_user_item_list(userid, roletm, cnt2, &p_item_2);
	if (ret != SUCC){
		return ret;
	}
    if (p_item_2) {
        cpy_len = sizeof(*p_item_2) * (*cnt2);
        memcpy(p_offset, p_item_2, cpy_len);
        free(p_item_2);
        p_offset += cpy_len;
    }
/*
    ret = this->gf_material.get_user_material_list(userid, roletm, cnt3, &p_item_3);
    if (ret != SUCC) {
        return ret;
    }
    if (p_item_3) {
        cpy_len = sizeof(*p_item_3) * (*cnt3);
        memcpy(p_offset, p_item_3, cpy_len);
        free(p_item_3);
        p_offset += cpy_len;
    }*/
    *outlen = p_offset-outbuf;
	DEBUG_LOG("role packagelist:uid=[%u],clothes_cnt=[%u],item_cnt=[%u]", 
        userid,*cnt1,*cnt2);
    return SUCC;
}

/** 
 * @fn  取得角色背包中物品列表
 * @brief  cmd route interface
 */
int Croute_func::gf_get_package_iclist(DEAL_FUN_ARG)
{
    char out[PROTO_MAX_SIZE] = {0};
    int cpy_len = 0;
	gf_get_package_iclist_out_header* p_out = (gf_get_package_iclist_out_header*)out;
	gf_get_package_iclist_out_item_1* p_item_1 = NULL;
	gf_get_package_iclist_out_item_2* p_item_2 = NULL;

    char* p_offset = &(out[0]) + sizeof(*p_out);
    ret = this->gf_attire.get_package_clothes_list(USERID_ROLETM, &(p_out->clothes_count), 
        (gf_attire_id_duration_list**)&p_item_1);
    if (ret != SUCC) {
        return ret;
    }
    if (p_item_1) {
        cpy_len = sizeof(*p_item_1) * p_out->clothes_count;
        memcpy(p_offset, p_item_1, cpy_len);
        free(p_item_1);
        p_offset += cpy_len;
    }

	ret = this->gf_item.get_user_item_list(USERID_ROLETM, &(p_out->item_count), 
        (gf_get_user_item_list_out_item**)&p_item_2);
	if (ret != SUCC){
		return ret;
	}
    if (p_item_2) {
        cpy_len = sizeof(*p_item_2) * p_out->item_count;
        memcpy(p_offset, p_item_2, cpy_len);
        free(p_item_2);
        p_offset += cpy_len;
    }

	DEBUG_LOG("get user itemlist:uid=[%u],clothes_count=[%u],item_count=[%u]",
			RECVBUF_USERID, p_out->clothes_count, p_out->item_count);
    STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0]));
}

int Croute_func::gf_set_item_int_value(DEAL_FUN_ARG)
{
	gf_set_item_int_value_in* p_in = PRI_IN_POS;

	if (!is_strengthen_material(p_in->itemid)) {
		uint32_t item_cnt = 0;
		ret = this->gf_item.get_item_int_value(USERID_ROLETM, p_in->itemid, item_count_column, &item_cnt);
		if (ret != SUCC)
		{
			return ret;
		}

		if (item_cnt != p_in->value) {
			if (item_cnt < p_in->value) {
				ret = this->gf_increase_item(USERID_ROLETM, p_in->itemid, p_in->value - item_cnt, 9999, 9999);
				if (ret != SUCC)
				{
					STD_ROLLBACK();
					return ret;
				}
			} else {
				ret = this->gf_item.reduce_item_int_value(USERID_ROLETM , p_in->itemid, item_count_column, item_cnt - p_in->value);
				if (ret != SUCC)
				{
					STD_ROLLBACK();
					return ret;
				}
			}
		}
	} else {
		if (p_in->value)
		{
			ret = this->gf_material.reduce_material_count(USERID_ROLETM, p_in->itemid, 0xffffffff);
			if (ret != SUCC)
			{
				STD_ROLLBACK();
				return ret;
			}

			ret = this->gf_increase_item(USERID_ROLETM, p_in->itemid, p_in->value, 9999, 9999);
			if (ret != SUCC)
			{
				STD_ROLLBACK();
				return ret;
			}
		}
		else
		{
			ret = this->gf_material.reduce_material_count(USERID_ROLETM, p_in->itemid, 0xffffffff);
			if (ret != SUCC)
			{
				STD_ROLLBACK();
				return ret;
			}
		}
	}
	DEBUG_LOG("gf_set_item_int_value:uid=[%u],role_tm=[%d]",
			USERID_ROLETM);
	STD_RETURN(ret);
}

int Croute_func::gf_add_item(DEAL_FUN_ARG)
{
	gf_add_item_in* p_in = PRI_IN_POS;	
    ret = gf_increase_item(USERID_ROLETM,p_in->itemid,p_in->add_num,set_item_limit(p_in->itemid),
        p_in->max_item_bag_grid_count);
	if (ret != SUCC)
	{	
		STD_ROLLBACK();
		return ret;
	}

	DEBUG_LOG("gf_add_item:uid=[%u],role_tm=[%d]",
			USERID_ROLETM);
	STD_RETURN(ret);
}

int Croute_func::gf_add_item_attire(DEAL_FUN_ARG)
{
    gf_add_item_attire_in * p_in = PRI_IN_POS;
    gf_add_item_attire_out out = {0};
    //DEBUG_LOG("gain item %u %u %u %u", p_in->type, p_in->item_id, p_in->extra_info,p_in->max_item_bag_grid_count);
    switch (p_in->type)
    {
    case TYPE_ATTIRE:
        m_attire_elem.attireid = p_in->item_id;
        m_attire_elem.gettime = time(NULL);
        ret = this->gf_attire.add_attire(USERID_ROLETM, &m_attire_elem, 250, &(p_in->extra_info));
        if (ret != SUCC) {
            STD_ROLLBACK();
        }
        break;
    case TYPE_ITEM:
        ret = gf_increase_item(USERID_ROLETM,p_in->item_id,p_in->extra_info,set_item_limit(p_in->item_id),DEFAULT_MAX_ITEM_GRID_CNT);
        if (ret != SUCC) {
            STD_ROLLBACK();
        }
        break;
    case TYPE_ROLE:
        if (p_in->item_id==TYPE_ROLE_XIAOMEE)
            ret = this->gf_role.increase_int_value(USERID_ROLETM,
                ROLE_XIAOMEE_COLUMN, p_in->extra_info);
        else if (p_in->item_id==TYPE_ROLE_EXP)
            ret = this->gf_role.increase_int_value(USERID_ROLETM, 
                ROLE_EXP_COLUMN, p_in->extra_info);
        else if (p_in->item_id==TYPE_ROLE_SKILLPOINT)
            ret = this->gf_role.increase_int_value(USERID_ROLETM, 
                ROLE_SKILL_POINT_COLUMN, p_in->extra_info);
        else if (p_in->item_id==TYPE_ROLE_FUMOPOINT)
            ret = this->gf_role.increase_int_value(USERID_ROLETM, 
                ROLE_FUMO_POINTS_COLUMN, p_in->extra_info);
        else if (p_in->item_id==TYPE_ROLE_PVP_FIGHT)
            ret = this->gf_role.increase_int_value(USERID_ROLETM,
                ROLE_EXPLOIT_COLUMN, p_in->extra_info);
        else if (p_in->item_id==TYPE_ROLE_HONOR)
            ret = this->gf_role.increase_int_value(USERID_ROLETM,
                ROLE_HONOR_COLUMN, p_in->extra_info);
        else if (p_in->item_id==TYPE_ROLE_YAOSHI_EXP)
            ret = this->gf_secondary_pro.add_player_secondary_pro_exp(USERID_ROLETM,
                1, p_in->extra_info, DEFAULT_MAX_SECONDARY_PRO_EXP);
        else if (p_in->item_id == TYPE_ROLE_ALLOCATOR_EXP)
            ret = this->gf_role.increase_int_value(USERID_ROLETM, ROLE_ALLOCATOR_EXP_COLUMN, p_in->extra_info);
        else if (p_in->item_id == TYPE_ROLE_SWORD_VALUE)
            ret = this->gongfu.set_int_add_value(RECVBUF_USERID, p_in->extra_info, "sword_value");
        else if (p_in->item_id == TYPE_ROLE_VIP_QUALIFY)
            ret = SUCC;
        else
        {
            STD_ROLLBACK();
            return ENUM_OUT_OF_RANGE_ERR;
        }
        break;
    default:
        STD_ROLLBACK();
        DEBUG_LOG("there %u ", p_in->type);
        return ENUM_OUT_OF_RANGE_ERR;
        break;
    }
	out.error_ret = ret;
	out.type = p_in->type;
	out.item_id = p_in->item_id;
	out.extra_info = p_in->extra_info;
	STD_RETURN_WITH_STRUCT(0, out);
}

int Croute_func::gf_del_item(DEAL_FUN_ARG)
{
	gf_del_item_in* p_in = PRI_IN_POS;	
	gf_del_item_out out = {0};
	
	//ret = this->gf_item.reduce_item_int_value(USERID_ROLETM, p_in->itemid, item_count_column, p_in->del_num);
    ret = gf_reduce_item_count(USERID_ROLETM,p_in->itemid,p_in->del_num);
	if (ret != SUCC)
	{

		STD_ROLLBACK();
		return ret;
	}

	DEBUG_LOG("gf_del_item:uid=[%u],role_tm=[%d]",
			USERID_ROLETM);
	out.itemid = p_in->itemid;
	out.del_num = p_in->del_num;
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_del_items(DEAL_FUN_ARG)
{

    gf_del_items_in_header* p_in = PRI_IN_POS; 
    CHECK_PRI_IN_LEN_WITHOUT_HEADER( p_in->count * sizeof(gf_del_items_in_item) );
    //gf_add_achieve_title_out out = {0};

    gf_del_items_in_item* p_in_item = (gf_del_items_in_item*)(p_in + 1);

	for (uint32_t i = 0; i < p_in->count; i++) {
		ret = gf_reduce_item_count(USERID_ROLETM, p_in_item->itemid, p_in_item->count);
		if (ret != SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}
		p_in_item++;
	}
	STD_RETURN(ret);
}

int Croute_func::gf_bitch_sell_items(DEAL_FUN_ARG)
{
    gf_bitch_sell_items_in_header* p_in = PRI_IN_POS; 
    DEBUG_LOG(" batch sell items [%u %u %u]", p_in->type, p_in->coins, p_in->cnt);
    
    CHECK_PRI_IN_LEN_WITHOUT_HEADER( p_in->cnt * sizeof(gf_bitch_sell_items_in_item) );

    gf_bitch_sell_items_in_item* p_in_item = (gf_bitch_sell_items_in_item*)(p_in + 1);
	for (uint32_t i = 0; i < p_in->cnt; i++) {
		ret = gf_reduce_item_count(USERID_ROLETM, p_in_item->itemid, p_in_item->count);
		if (ret != SUCC) {
			STD_ROLLBACK();
			return ret;
		}
		p_in_item++;
	}

    uint32_t left_coins = 0;
    if (p_in->type == 1) {
		ret = gf_increase_item(USERID_ROLETM, gold_coin_item_id, p_in->coins, 
				set_item_limit(gold_coin_item_id), 500);
        if ((ret != SUCC)) {
            STD_ROLLBACK();
            return ret;
        }
        left_coins = p_in->coins;
        this->gf_get_item_count(USERID_ROLETM, gold_coin_item_id, &left_coins);
    } else if (p_in->type == 2) {
        ret = gf_role.increase_int_value_with_ret(USERID_ROLETM, ROLE_XIAOMEE_COLUMN, p_in->coins, &left_coins);
        if ((ret != SUCC)) {
            STD_ROLLBACK();
            return ret;
        }
    }

    p_in->coins = left_coins;

    STD_RETURN_WITH_BUF(ret, p_in, sizeof(*p_in) + p_in->cnt * sizeof(*p_in_item));
}
/** 
 * @fn 取得用户部分信息
 * @brief  cmd route interface
 */
int Croute_func::gf_get_user_partial_info(DEAL_FUN_ARG)
{
	char out[PROTO_MAX_SIZE] = {0};
	gf_get_user_partial_info_in* p_in = PRI_IN_POS;	
	gf_get_user_partial_info_out_header* p_out_header = (gf_get_user_partial_info_out_header*)out;
	gf_get_user_partial_info_out_item* p_out_item = NULL;
	int cpy_len = 0;
	char* p_offset = &(out[0]) + sizeof(*p_out_header);

	
	ret = this->gf_role.get_role_partial_info(p_in->userid, p_in->role_time, &(p_out_header->userpart));
	if (ret != SUCC)
	{
		return ret;
	}
   gf_get_user_vip_out vip_obj = {0};
    ret = this->gongfu.get_vip_info(p_in->userid, &vip_obj);
    if (ret != SUCC) {
        return ret;
    }
    p_out_header->userpart.vip = vip_obj.vip_is;
 	//added by cws20120510
    gf_get_other_active_out active = {0};
    active.active_id = 2;
    ret = this->gf_other_active.get_player_other_active_info(p_in->userid, p_out_header->userpart.role_regtime, &active);
    if (ret != SUCC) {
        return ret;
    }

    memcpy(p_out_header->userpart.datas, active.info_buf, ACTIVE_BUF_LEN);
	DEBUG_LOG("gf_get_role_hp_mp_info");
    //# 
    p_out_header->x_value = vip_obj.x_value;
    p_out_header->start_time = vip_obj.start_tm;
    p_out_header->end_time = vip_obj.end_tm;
    /*
	ret = this->gongfu.get_vip_type(p_in->userid, &(p_out_header->userpart.vip));
	if (ret != SUCC)
	{
		return ret;
	}
*/

 ret = this->gf_attire.get_user_attireid_by_usedflg(p_in->userid, p_out_header->userpart.role_regtime
		, &(p_out_header->attirecnt.attire_cnt), &p_out_item, ATTIRE_USED);
	if (ret != SUCC)
	{
		return ret;
	}

	if (p_out_item)
	{
		cpy_len = sizeof(*p_out_item) * p_out_header->attirecnt.attire_cnt;
		memcpy(p_offset, p_out_item, cpy_len);
		free(p_out_item);
		p_offset += cpy_len;
	}

	DEBUG_LOG("get user_partial_info:uid=[%u],clothes_cnt=[%d],len=[%ld]",
			p_in->userid, p_out_header->attirecnt.attire_cnt, p_offset-&(out[0]));
	STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0]));

}

int Croute_func::gf_list_simple_role_info(DEAL_FUN_ARG)
{
	gf_list_simple_role_info_in * p_in = PRI_IN_POS;
	gf_list_simple_role_info_out_header  header = {0};
	gf_list_simple_role_info_out_item * p_out_item = NULL;

	ret = gf_role.get_simple_role_info(p_in->userid, &(header.role_cnt), &p_out_item);

	STD_RETURN_WITH_STRUCT_LIST(ret, header, p_out_item, header.role_cnt);	

//	gf_search_team_info_out_header out_header = {0};
//	gf_search_team_info_out_item * p_list = 0;
//	ret = gf_team.search_team_info(p_in->team_id, p_in->captain_id, &(out_header.team_cnt), &p_list);
//	if (ret != SUCC) {
//		return ret;
//	}
//	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_list, out_header.team_cnt);	

}

/** 
 * @fn 取得用户部分信息
 * @brief  cmd route interface
 */
int Croute_func::gf_get_role_detail_info(DEAL_FUN_ARG)
{
	gf_get_role_detail_info_in* p_in = PRI_IN_POS;	
	gf_get_role_detail_info_out out = {0};

	ret = this->gf_role.get_role_detail_info(p_in->userid, p_in->role_time, &out);
	if (ret != SUCC) {
		return ret;
	}

    gf_get_other_active_out active = {0};
    active.active_id = 2;
    ret = this->gf_other_active.get_player_other_active_info(p_in->userid, p_in->role_time, &active);
    if (ret != SUCC) {
        return ret;
    }

    memcpy(out.datas, active.info_buf, ACTIVE_BUF_LEN);
	DEBUG_LOG("gf_get_role_detail_info");
	STD_RETURN_WITH_STRUCT(ret, out);
}


/** 
 * @fn 增加离线消息
 * @brief  cmd route interface
 */
int Croute_func::gf_add_offline_msg(DEAL_FUN_ARG)
{
	gf_add_offline_msg_in_header *p_in= PRI_IN_POS;

	CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->msglen-4);

	if (RECVBUF_ROLETM == 0)
	{
		ret= this->gongfu.add_offline_msg(RECVBUF_USERID, (stru_msg_item *)p_in);
	}
	else
	{
		ret= this->gf_role.add_offline_msg(RECVBUF_USERID, RECVBUF_ROLETM, (stru_msg_item *)p_in);		
	}
	
	STD_RETURN(ret);
}

int Croute_func::gf_role_login(DEAL_FUN_ARG)
{
	char out[PROTO_MAX_SIZE] = {0};
	int cpy_len = 0;
	gf_role_login_out_header* p_out_header = (gf_role_login_out_header*)out;
	gf_role_login_out_item_1* p_out_item_1 = NULL;
	gf_role_login_out_item_2* p_out_item_2 = NULL;

	char* p_offset = &(out[0]) + sizeof(*p_out_header);

	ret = this->gongfu.get_vip_type(RECVBUF_USERID, &(p_out_header->vip));
	if (ret != SUCC)
	{
		return ret;
	}

	ret = this->gf_role.is_role_status_nor(RECVBUF_USERID, RECVBUF_ROLETM );
	if (ret != SUCC)
	{
		return ret;
	}
	ret = this->gf_friend.get_friendid_list(RECVBUF_USERID, TYPE_FRIEND, &(p_out_header->friend_count), (stru_friedid_list**)&p_out_item_1);
	if (ret != SUCC)
	{
		return ret;
	}	

	if (p_out_item_1)
	{
		cpy_len = sizeof(*p_out_item_1) * p_out_header->friend_count;
		memcpy(p_offset, p_out_item_1, cpy_len);
		free(p_out_item_1);
		p_offset += cpy_len;
	}

	ret = this->gf_friend.get_friendid_list(RECVBUF_USERID, TYPE_BLACK, &(p_out_header->black_count), (stru_friedid_list**)&p_out_item_2);
	if (ret != SUCC)
	{
		return ret;
	}	

	if (p_out_item_2)
	{
		cpy_len = sizeof(*p_out_item_2) * p_out_header->black_count;
		memcpy(p_offset, p_out_item_2, cpy_len);
		free(p_out_item_2);
		p_offset += cpy_len;
	}


	DEBUG_LOG("gf_user_login:uid=[%u],vip=[%u],friend_cnt=[%u],black_cnt=[%u],len=[%ld]"
			,RECVBUF_USERID, p_out_header->vip, p_out_header->friend_count, p_out_header->black_count, p_offset - &(out[0]));
	STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0]));
}

int Croute_func::gf_get_user_base_info(DEAL_FUN_ARG)
{
	gf_get_user_base_info_out_header out = {0};

	ret = this->gongfu.get_user_base_info(RECVBUF_USERID, &out);
	if (ret != SUCC)
	{
		return ret;
	}

    out.max_bag = 50;
    if (out.vip != 0) {
        uint32_t tail_time = time(NULL);
        if ( out.vip == 2 ) {
            tail_time = out.end_time;
        }
        uint32_t vip_lv = 0;
        //vip_config_data_mgr::getInstance()->init();
        vip_lv = calc_player_vip_level( &vip_obj, out.start_time, tail_time, out.x_value, out.vip);
        out.max_bag += vip_obj.get_add_item_bag_count_by_level(vip_lv);
        DEBUG_LOG("calc_player_vip_level: vip_lv=%u max_bag=%u", vip_lv, out.max_bag);
        //vip_config_data_mgr::getInstance()->final();
    }

    DEBUG_LOG("get user info:uid=[%u],max_bag=[%u],len=[%ld]",RECVBUF_USERID, out.max_bag, sizeof(gf_get_user_base_info_out_header));
	STD_RETURN_WITH_STRUCT(ret, out);
}

uint32_t Croute_func::gf_get_player_vip_level(uint32_t userid)
{
	gf_get_user_base_info_out_header out = {0};
	
	ret = this->gongfu.get_user_base_info(userid, &out);
	if (ret != SUCC)
	{
		return ret;
	}

	out.max_bag = 50;
	if (out.vip != 0) {
		uint32_t tail_time = time(NULL);
		if ( out.vip == 2 ) {
			tail_time = out.end_time;
		}
		uint32_t vip_lv = 0;
		//vip_config_data_mgr::getInstance()->init();
		vip_lv = calc_player_vip_level( &vip_obj, out.start_time, tail_time, out.x_value, out.vip);
		out.max_bag += vip_obj.get_add_item_bag_count_by_level(vip_lv);
		DEBUG_LOG("calc_player_vip_level: vip_lv=%u max_bag=%u", vip_lv, out.max_bag);
		return vip_lv;
	}
	return 0;
}
/**
 *
 */
int Croute_func::gf_set_user_base_info(DEAL_FUN_ARG)
{
	gf_set_user_base_info_in* p_in = PRI_IN_POS;
	ret = this->gongfu.update_user_base_info(RECVBUF_USERID, p_in);
	STD_RETURN(ret);
}

int Croute_func::gf_get_role_list(DEAL_FUN_ARG)
{
	char out[PROTO_MAX_SIZE] = {0};
	gf_get_role_list_out_header* p_out_header = (gf_get_role_list_out_header*)out;
	char* p_offset = &(out[0]) + sizeof(*p_out_header);
	
	gf_get_role_list_out_item_1* p_out_item_1 = NULL;
	gf_get_role_list_out_item_1* p_cur_out_item_1 = NULL;
	gf_get_role_list_out_item_2* p_out_item_2 = NULL;


	this->gf_role.set_role_phy_del(RECVBUF_USERID);
    //getback outdate attire     
    uint32_t outdate = time(NULL);
	uint32_t* p_role_tm_list = 0;
	uint32_t role_cnt = 0;
	this->gf_attire.get_clothes_outdated(USERID_ROLETM, outdate, &p_role_tm_list, &role_cnt);
	if (p_role_tm_list) {
		for (uint32_t i = 0; i < role_cnt; i++) {
#ifdef VERSION_TAIWAN
			this->gf_mail.insert_sys_nor_mail(RECVBUF_USERID, p_role_tm_list[i], 999, "商城道具過期通知", "親愛的小俠士！你的部分商城道具已經過期，可在商城\"我的過期物品\"中找到並續費使用。");
#else
            gf_mail.insert_sys_nor_mail(RECVBUF_USERID, p_role_tm_list[i], 999, "商场道具过期通知", "小侠士!您部分商场道具已到期.可在商城\"我的过期物品\"中找到并续费!" );
#endif
		}
		free(p_role_tm_list);
	}
	STD_COMMIT(ret);
    this->gf_attire.set_clothes_outdated(USERID_ROLETM, outdate);
	STD_COMMIT(ret);
	int cpy_len = 0;
    gf_user_self_define self_out = {0};
    ret = this->gongfu.get_necessary_info_for_login(RECVBUF_USERID, &self_out);
    if (ret != SUCC && ret != USER_ID_NOFIND_ERR) {
        return ret;
    }
    if (self_out.deadline == 4) {
        p_out_header->deadline = 0xffffffff;
    } else {
        p_out_header->deadline = self_out.forbid_time +((self_out.deadline - 1) * 7 + 1) * 3600 * 24;
        if (p_out_header->deadline < time(NULL)) {
            ret = this->gongfu.set_int_value(RECVBUF_USERID, 0, "forbid_flag");
            self_out.forbid_flag = 0;
        }
    }
    DEBUG_LOG("role_list: forbid_flag=[%u] deadline=[%u]",self_out.forbid_flag, p_out_header->deadline);
    p_out_header->forbid_flag = self_out.forbid_flag;
    //p_out_header->deadline = self_out.deadline;

    uint32_t val = self_out.x_value;
    uint32_t cur_time = time(NULL);
    if (self_out.start_time != 0 && cur_time > self_out.start_time) {
        uint32_t interval = ( cur_time - self_out.start_time ) / 86400;
        if (self_out.vip_is == 2) {
            interval = (self_out.end_time - self_out.start_time) / 86400;
        }
        if ( self_out.vip_is == VIP_YEAR_USER_FLAG) {
            interval = interval * 3; //+ 100
        }
        val = interval + self_out.x_value;
    }

    p_out_header->vip_is = self_out.vip_is;
    p_out_header->x_value = val;
    p_out_header->end_time = self_out.end_time;

    ret = this->gf_role.get_role_list(RECVBUF_USERID, &(p_out_header->count), &p_out_item_1);
    if (ret != SUCC)
    {
        return ret;
    }
    for (uint32_t i=0;i<p_out_header->count;i++)
	{
		p_cur_out_item_1 = p_out_item_1 + i;
		ret = this->gf_attire.get_used_clothes_list(RECVBUF_USERID, p_cur_out_item_1->role_time
			, &(p_cur_out_item_1->clothes_count), &p_out_item_2);
		if (ret != SUCC)
		{
			return ret;
		}
		// copy item1 to out buf
		cpy_len = sizeof(*p_cur_out_item_1);
		memcpy(p_offset, p_cur_out_item_1, cpy_len);
		p_offset += cpy_len;

		DEBUG_LOG("get role clothes list:uid=[%u],role_no=[%d],clothes_count=[%d]",
				RECVBUF_USERID, i, p_cur_out_item_1->clothes_count);

		// copy item2 to out buf
		if (p_out_item_2)
		{
			cpy_len = sizeof(*p_out_item_2) * (p_cur_out_item_1->clothes_count);
			memcpy(p_offset, p_out_item_2, cpy_len);
			p_offset += cpy_len;	

			free(p_out_item_2);
			p_out_item_2 = NULL;
		}
	}
	if (p_out_item_1)
		free(p_out_item_1);

	DEBUG_LOG("get role list:uid=[%u],role_cnt=[%d]",
			RECVBUF_USERID, p_out_header->count);
	STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0]));
}


int Croute_func::gf_get_role_list_ex(DEAL_FUN_ARG)
{
	gf_get_role_list_ex_out_header out_header = {0};
	
	gf_get_role_list_ex_out_item_1* p_out_item_1 = NULL;
//	gf_get_role_list_ex_out_item_1* p_cur_out_item_1 = NULL;
//	gf_get_role_list_ex_out_item_2* p_out_item_2 = NULL;

//	int cpy_len = 0;

	ret = this->gf_role.get_all_role_list(RECVBUF_USERID, &(out_header.count), &p_out_item_1);
	if (ret != SUCC)
	{
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item_1, out_header.count);
}


int Croute_func::gf_del_attire(DEAL_FUN_ARG)
{
	gf_del_attire_in* p_in = (gf_del_attire_in*)PRI_IN_POS;
	gf_del_attire_out out = {0};

	ret = this->gf_attire.del_attire(USERID_ROLETM, p_in);
	if (ret != SUCC)
	{
		return ret;
	}

	DEBUG_LOG("ggf_del_attire:uid=[%u],attireid=[%d],id=[%d]",
			RECVBUF_USERID, p_in->attireid, p_in->index_id);
	out.attireid = p_in->attireid;
	out.index_id = p_in->index_id;
	STD_RETURN_WITH_STRUCT(ret, out)
}


int Croute_func::gf_add_attire(DEAL_FUN_ARG)
{
	gf_add_attire_in* p_in = (gf_add_attire_in*)PRI_IN_POS;

	m_attire_elem.attireid = p_in->attireid;
	m_attire_elem.gettime = p_in->gettime;
	m_attire_elem.attire_rank = 1;
	m_attire_elem.duration = p_in->duration;
    m_attire_elem.endtime = 0;
	m_attire_elem.usedflag= p_in->usedflag;
	ret = this->gf_attire.add_attire(USERID_ROLETM, &m_attire_elem, p_in->max_bag_grid_count);
	if (ret != SUCC)
	{
		return ret;
	}

	DEBUG_LOG("ggf_del_attire:uid=[%u],attireid=[%d],gettime=[%d]",
			RECVBUF_USERID, p_in->attireid, p_in->gettime);
	STD_RETURN(ret);
}

int Croute_func::gf_add_role(DEAL_FUN_ARG)
{
	gf_add_role_in* p_in = (gf_add_role_in*)PRI_IN_POS;
	gf_add_role_out out = {0};
	bool is_existed = true;
	uint32_t regtime = 0;
	time_t now = time (NULL);
	DEBUG_LOG("in gf_add_role");
	if (p_in->role_type > 4 || p_in->role_type == 0)
	{
		return ROLE_TYPE_NOFIND_ERR;
	}
	out.parentid = p_in->parentid;
	out.parent_role_tm = p_in->parent_role_tm;
	
	ret = this->gongfu.is_user_existed(RECVBUF_USERID, &is_existed, &regtime);

	if (!is_existed)
	{
		ret = this->gongfu.add_user(RECVBUF_USERID, p_in->parentid);
		
		if (ret != SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}
	}
	else if (!regtime)
	{
		ret = this->gongfu.set_int_value(RECVBUF_USERID, (uint32_t)now, "regtime");	
		if (ret != SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}
		if (p_in->parentid) 
		{
			ret = this->gongfu.set_int_value(RECVBUF_USERID, p_in->parentid, "parentid"); 
			if (ret != SUCC)
			{
				STD_ROLLBACK();
				return ret;
			}
		}
	}
	else 
	{
		out.user_existed = 1;
	}

	ret = this->gf_role.is_role_num_max(RECVBUF_USERID);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

	ret = this->gf_role.add_role(RECVBUF_USERID, (uint32_t)now, p_in);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	RECVBUF_ROLETM = (uint32_t)now;
	//uint32_t skill_id = 0;
	add_attire_t m_attire_elem2 = {0};
	memset(&m_attire_elem, 0, sizeof(m_attire_elem));
	//donate towards
	if (p_in->role_type == 1)
	{
		m_attire_elem.attireid = 100001;
		m_attire_elem.duration = 32 * 50;

		m_attire_elem2.attireid = 100585;
		m_attire_elem2.duration = 116 * 50;
		//skill_id = 100204;
	}
	else if (p_in->role_type == 2)
	{
		m_attire_elem.attireid = 200001;
		m_attire_elem.duration = 35 * 50;

		m_attire_elem2.attireid = 200585;
		m_attire_elem2.duration = 116 * 50;
		//skill_id = 200201;
	}
	else if (p_in->role_type == 3)
	{
		m_attire_elem.attireid = 300001;
		m_attire_elem.duration = 29 * 50;

		m_attire_elem2.attireid = 300585;
		m_attire_elem2.duration = 116 * 50;
		//skill_id = 300213;
	}
    else if (p_in->role_type == 4) {
        m_attire_elem.attireid = 400014;
        m_attire_elem.duration = 20 * 50;

		m_attire_elem2.attireid = 400585;
		m_attire_elem2.duration = 116 * 50;
        //skill_id = 300213;
    }
		
	// insert default attires
	m_attire_elem.attire_rank = 1;
	m_attire_elem.gettime = time(NULL);
    m_attire_elem.endtime = 0;
	m_attire_elem.usedflag = 0;
	ret = this->gf_attire.add_attire(USERID_ROLETM, &m_attire_elem, MAX_ATTIRE_CNT);
	//if (ret != SUCC)
	{
	//	STD_ROLLBACK();
	//	return ret;
	}

	m_attire_elem2.attire_rank = 1;
	m_attire_elem2.gettime = time(NULL);
    m_attire_elem2.endtime = 0;
	m_attire_elem2.usedflag = 1;
	ret = this->gf_attire.add_attire(USERID_ROLETM, &m_attire_elem2, MAX_ATTIRE_CNT);
	
	
	if ((240273282 <= RECVBUF_USERID && RECVBUF_USERID <= 240323464) ||
		(240223021 <= RECVBUF_USERID && RECVBUF_USERID <= 240273281) ||
		(240001000 <= RECVBUF_USERID && RECVBUF_USERID <= 240101443) ||
		(240101444 <= RECVBUF_USERID && RECVBUF_USERID <= 240223020) ||
		(241000100 <= RECVBUF_USERID && RECVBUF_USERID <= 241201966) ||
		(241400010 <= RECVBUF_USERID && RECVBUF_USERID <= 241601554) ||
		(240323465 <= RECVBUF_USERID && RECVBUF_USERID <= 240976689)) {
		this->gf_mail.insert_sys_nor_mail(USERID_ROLETM, 1014, "", "");
		this->gf_role.increase_int_value(USERID_ROLETM, ROLE_XIAOMEE_COLUMN, 4000);
	}
	
	//insert amb reward of invitee
	uint32_t parent_id = 0;
	this->gongfu.get_parent_id(RECVBUF_USERID, &parent_id);
	if (parent_id) {
		//insert
		this->gf_increase_item(USERID_ROLETM, 1700050, 1, 999, 999);
	}
	
	this->gf_increase_item(USERID_ROLETM, 1700009, 1, 999, 999);
	this->gf_increase_item(USERID_ROLETM, 1300001, 10, 999, 999);
	this->gf_increase_item(USERID_ROLETM, 1300101, 10, 999, 999);
	this->gf_increase_item(USERID_ROLETM, 1302001, 8, 999, 999);
	//
	//this->gf_mail.insert_sys_nor_mail(USERID_ROLETM, 1004, "", "");

	// insert default skills
/*	
	add_skill_t skill_in = {0};
	skill_in.skill_id = skill_id;
	skill_in.skill_lv = 1;
	skill_in.bind_key = 10;
	ret = this->gf_skill.add_skill(USERID_ROLETM, &skill_in);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
*/
	
	ret = this->gf_role.get_role_num(RECVBUF_USERID, ROLE_STATUS_ALL, &(out.role_num));
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

    if (p_in->parentid != 0 && p_in->parent_role_tm != 0) {
        this->gf_friend.add(RECVBUF_USERID, p_in->parentid, 1, TYPE_FRIEND);
        this->gf_master.add_master(USERID_ROLETM, p_in->parentid, p_in->parent_role_tm, time(NULL));
    }

	DEBUG_LOG("gf_add_role:uid=[%u],role_tm=[%d],role_type=[%d],allrolenum=[%u] parent=[%u %u]",
			RECVBUF_USERID,RECVBUF_ROLETM, p_in->role_type, out.role_num, p_in->parentid, p_in->parent_role_tm );
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_gray_delete_role(DEAL_FUN_ARG)
{
	ret = this->gf_role.gray_del_role(USERID_ROLETM);
	if (ret != SUCC)
	{
		return ret;
	}

	DEBUG_LOG("gray delete:uid=[%u],role_tm=[%d]",
			USERID_ROLETM);
	STD_RETURN(ret);
}

int Croute_func::gf_logic_delete_role(DEAL_FUN_ARG)
{

    time_t now = time (NULL);
	/*
    uint32_t del_tm = 0;
    uint32_t deadtime = (uint32_t)now - 7 * 24 * 60 * 60;
    ret = this->gf_role.get_int_value(USERID_ROLETM, "del_tm", &del_tm);
	if (ret != SUCC)
	{
		return ret;
	}
    if (del_tm > deadtime)
    {
        return SYS_ERR;
    }
    */
	ret = this->gf_role.logic_del_role(USERID_ROLETM);
	if (ret != SUCC)
	{
		return ret;
	}
	//delete role phy 1month ago
	gf_get_logic_del_role_list* p_logic_del = 0;
	gf_get_logic_del_role_list* p_cur = 0;

	uint32_t count = 0;
	int ret1 = this->gf_role.get_logic_del_role_list(RECVBUF_USERID, &count, &p_logic_del);
	if (ret1 == SUCC)
	{
        uint32_t linetime = (uint32_t)now - 30 * 24 * 60 * 60;
        p_cur = p_logic_del;
		for (uint32_t i = 0; i < count; i++)
		{
            if (p_cur->del_tm < linetime)
            {
			    DEBUG_LOG("phy del role %u:uid=[%u],role_tm=[%d]", count, RECVBUF_USERID, p_cur->del_tm);
			    this->del_role_phy(RECVBUF_USERID, p_cur->role_time);
            }
            p_cur++;
		}
		if (p_logic_del) 
		{
			free(p_logic_del);
		}
	}

	
	DEBUG_LOG("logic delete:uid=[%u],role_tm=[%d] ret %u", USERID_ROLETM, ret);
	STD_RETURN(ret);
}

int Croute_func::gf_resume_gray_role(DEAL_FUN_ARG)
{
	ret = this->gf_role.resume_gray_role(USERID_ROLETM);
	if (ret != SUCC)
	{
		return ret;
	}

	DEBUG_LOG("resume gray:uid=[%u],role_tm=[%d]",
			USERID_ROLETM);
	STD_RETURN(ret);
}

int Croute_func::gf_del_role(DEAL_FUN_ARG)
{
	uint32_t status = 0;
	ret = this->gf_role.get_int_value(USERID_ROLETM, "status", &status);
	if (ret != SUCC)
	{
		return ret;
	}	
	if (status != ROLE_STATUS_DEL)
	{
		return SYS_ERR;
	}
	return del_role_phy(USERID_ROLETM);

}

/*
int Croute_func::gf_set_role_delflg(DEAL_FUN_ARG)
{
	uint32_t role_num = 0;
	ret = this->gf_role.get_role_num(RECVBUF_USERID, ROLE_STATUS_NOR_BLOCK, &role_num);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	if (role_num <= 1) 
	{
		STD_ROLLBACK();
		return ROLE_NUM_MIN_ERR;
	}

	ret = this->gf_role.set_role_delflg(USERID_ROLETM);
	if (ret != SUCC)
	{
		return ret;
	}

	DEBUG_LOG("gf_set_role_delflg:uid=[%u],role_tm=[%d]",
			USERID_ROLETM);
	STD_RETURN(ret);
}
*/
int Croute_func::gf_set_role_int_value(DEAL_FUN_ARG)
{
	gf_set_role_int_value_in* p_in = (gf_set_role_int_value_in*)PRI_IN_POS;	
	ret = this->gf_role.set_int_value(USERID_ROLETM, p_in->column_name, p_in->value);
	if (ret != SUCC)
	{
		return ret;
	}

	DEBUG_LOG("gf_set_role_int_value:uid=[%u],role_tm=[%d]",
			USERID_ROLETM);
	STD_RETURN(ret);

}

int Croute_func::gf_exchange_coins_fight_value(DEAL_FUN_ARG)
{
	gf_exchange_coins_fight_value_in* p_in = (gf_exchange_coins_fight_value_in*)PRI_IN_POS;	

	gf_exchange_coins_fight_value_out out = { 0 };
	if (p_in->type == 1) {
		ret = gf_role.reduce_int_value(USERID_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, ROLE_XIAOMEE_COLUMN, p_in->coins, &(out.coins));
		if (ret != SUCC) 
        {
            STD_ROLLBACK();
            return ret;
        }
		ret = gf_role.increase_int_value(USERID_ROLETM, ROLE_EXPLOIT_COLUMN, p_in->fight_value);
		if (ret != SUCC) 
        {
            STD_ROLLBACK();
            return ret;
        }
		gf_role.reduce_int_value(USERID_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, ROLE_EXPLOIT_COLUMN, 0, &(out.fight_value));
	} else if (p_in->type == 2) {
		ret = gf_role.reduce_int_value(USERID_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, ROLE_EXPLOIT_COLUMN, p_in->fight_value, &(out.fight_value));
		if (ret != SUCC) 
        {
            STD_ROLLBACK();
            return ret;
        }
		ret = gf_role.increase_int_value(USERID_ROLETM, ROLE_XIAOMEE_COLUMN, p_in->coins);
		if (ret != SUCC) 
        {
            STD_ROLLBACK();
            return ret;
        }
		gf_role.reduce_int_value(USERID_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, ROLE_XIAOMEE_COLUMN, 0, &(out.coins));
	}
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_set_role_itembind(DEAL_FUN_ARG)
{
	gf_set_role_itembind_in* p_in = (gf_set_role_itembind_in*)PRI_IN_POS;	
	gf_set_role_itembind_out out = {{0}};

	memcpy(&out,p_in,sizeof(gf_set_role_itembind_out));
	ret = this->gf_role.update_itembind(USERID_ROLETM, p_in);
//	if (ret == SUCC)
//		ret = this->gf_role.get_itembind(USERID_ROLETM, &out);
	STD_RETURN_WITH_STRUCT(ret, out);
}


int Croute_func::gf_get_role_itembind(DEAL_FUN_ARG)
{
	gf_get_role_itembind_out out = {{0}};

	ret = this->gf_role.get_itembind(USERID_ROLETM, (gf_set_role_itembind_out*)&out);
	STD_RETURN_WITH_STRUCT(0, out);
}
//add for double experience time
int Croute_func::gf_get_double_exp_data(DEAL_FUN_ARG)
{
    gf_get_double_exp_data_out out = {0};
    ret = this->gf_role.get_role_double_exp_data(USERID_ROLETM, &(out.day_flag), &(out.dexp_tm));
    STD_RETURN_WITH_STRUCT(0, out);
}

int Croute_func::gf_set_double_exp_data(DEAL_FUN_ARG)
{
    gf_set_double_exp_data_in* p_in = (gf_set_double_exp_data_in*)PRI_IN_POS;
    gf_set_double_exp_data_in out = {0};
    ret = this->gf_role.set_role_double_exp_data(USERID_ROLETM, p_in->day_flag, p_in->dexp_tm);
    if (ret != SUCC) {
        return ret;
    }
    out.day_flag = p_in->day_flag;
    out.dexp_tm  = p_in->dexp_tm;
    STD_RETURN_WITH_STRUCT(0, out);
}
//add for times chapter
int Croute_func::gf_get_max_times_chapter(DEAL_FUN_ARG)
{
    uint32_t out = 0;
    ret = this->gf_role.get_int_value(USERID_ROLETM, "max_times_chapter", &out);
    if (ret != SUCC) {
        return ret;
    }
    STD_RETURN_WITH_STRUCT(0, out);
}

int Croute_func::gf_set_max_times_chapter(DEAL_FUN_ARG)
{
    gf_set_max_times_chapter_in *p_in = (gf_set_max_times_chapter_in*)PRI_IN_POS;
    uint32_t now_chapter = 0;
    ret = this->gf_role.get_int_value(USERID_ROLETM, "max_times_chapter", &now_chapter);
    if (ret != SUCC) {
        return ret;
    }
    if (p_in->now_chapter > now_chapter) {
        ret = this->gf_role.set_int_value(USERID_ROLETM, "max_times_chapter", p_in->now_chapter);
        if (ret != SUCC) {
            return ret;
        }
    }
    STD_RETURN(SUCC);
}

int Croute_func::gf_get_friendlist_type(DEAL_FUN_ARG)
{
	gf_get_friendlist_type_in* p_in = (gf_get_friendlist_type_in*)PRI_IN_POS;
	gf_get_friendlist_type_out_header out_header = {0};
	gf_get_friendlist_type_out_item* p_out_item = NULL;

	ret = this->gf_friend.get_friendid_list_type(RECVBUF_USERID, (FRIEND_TYPE)(p_in->friend_type), 
		&(out_header.count), &p_out_item);
	if (ret != SUCC)
	{
		return ret;
	}

	DEBUG_LOG("get friendlist:uid=[%u],friend_cnt=[%d]",
			RECVBUF_USERID, out_header.count);
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::gf_buy_attire(DEAL_FUN_ARG)
{
	gf_buy_attire_in_header* p_in = PRI_IN_POS;
	gf_buy_attire_out_header out_header = {0};
	gf_buy_attire_out_item* p_out_item=NULL;
	uint32_t left_coins = 0;

	CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->count * sizeof(gf_buy_attire_in_item)); 
	//reduce coins
    if (p_in->flag == 0) {
        ROLE_REDUCE_VALUE(XIAOMEE, RECVBUF_USERID, RECVBUF_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, p_in->cost, &left_coins);
    } else if (p_in->flag == 1) {
        ROLE_REDUCE_VALUE(EXPLOIT, RECVBUF_USERID, RECVBUF_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, p_in->cost, &left_coins);
    } else if (p_in->flag == 2) {
        ROLE_REDUCE_VALUE(FUMO_POINTS, RECVBUF_USERID, RECVBUF_ROLETM, GF_FUMO_NOENOUGH_ERR, p_in->cost, &left_coins);
    }
//	ret = this->gf_role.reduce_coins(RECVBUF_USERID,RECVBUF_ROLETM,p_in->cost,&left_coins);
	
	//insert table
	gf_buy_attire_in_item* pcur=(gf_buy_attire_in_item*)((char*)p_in+sizeof(gf_buy_attire_in_header));
	for (uint32_t iter=0;iter<p_in->count;iter++)
	{
		m_attire_elem.attireid = pcur->attireid;
		m_attire_elem.gettime = pcur->gettime;
		m_attire_elem.attire_rank = pcur->attire_rank;
		m_attire_elem.duration = pcur->duration;
        m_attire_elem.endtime = 0;
		m_attire_elem.usedflag = 0;

		ret = this->gf_attire.add_attire(USERID_ROLETM, &m_attire_elem, p_in->max_bag_grid_count);
		if (ret!=SUCC)
		{ 
			STD_ROLLBACK();
			return ret;
		}
		pcur++;
	}
	//get return info : auto increament id of table
	ret = this->gf_attire.get_max_id_list(USERID_ROLETM, p_in->count, &out_header.count, &p_out_item);
	if (ret!=SUCC)
	{ 
		STD_ROLLBACK();
		return ret;
	}

    out_header.flag = p_in->flag;
	out_header.coins_left = left_coins;
	out_header.count = p_in->count;
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::gf_buy_item(DEAL_FUN_ARG)
{
	gf_buy_item_in* p_in = PRI_IN_POS;
	gf_buy_item_out out = {0};

	uint32_t left_coins = 0;

	//reduce coins
	ROLE_REDUCE_VALUE(XIAOMEE, RECVBUF_USERID, RECVBUF_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, p_in->cost, &left_coins);

	//insert table
	ret = gf_increase_item(USERID_ROLETM,p_in->itemid,p_in->count,p_in->max,p_in->max_item_bag_grid_count);
	if (ret!=SUCC)
	{ 
		STD_ROLLBACK();
		return ret;
	}
	//get return info : auto increament id of table
	
	out.coins_left = left_coins;
	out.count = p_in->count;
	out.itemid = p_in->itemid;
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::gf_sell_item(DEAL_FUN_ARG)
{
	gf_sell_item_in* p_in = PRI_IN_POS;
	gf_sell_item_out out = {0};

	ret = this->gf_item.reduce_item_int_value(USERID_ROLETM, p_in->item_id, item_count_column, p_in->item_cnt);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	ret = this->gf_role.increase_int_value_with_ret(USERID_ROLETM, ROLE_XIAOMEE_COLUMN, p_in->price, &(out.left_coins));
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	out.item_id = p_in->item_id;
	out.item_cnt = p_in->item_cnt;
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_sell_attire(DEAL_FUN_ARG)
{
	gf_sell_attire_in* p_in = PRI_IN_POS;
	gf_sell_attire_out out = {0};

	ret = this->gf_attire.del_attire_with_index(USERID_ROLETM, p_in->index_id);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	ret = this->gf_role.increase_int_value_with_ret(USERID_ROLETM, ROLE_XIAOMEE_COLUMN, p_in->price, &(out.left_coins));
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	out.attire_id = p_in->attire_id;
	out.index_id = p_in->index_id;
	STD_RETURN_WITH_STRUCT(ret, out);

}

int Croute_func::gf_buy_goods(DEAL_FUN_ARG)
{
    gf_buy_goods_in* p_in = PRI_IN_POS;

	uint32_t left_coins = 0;
	//reduce coins
	ROLE_REDUCE_VALUE(XIAOMEE, RECVBUF_USERID, RECVBUF_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, p_in->coins * p_in->cnt, &left_coins);

    if (p_in->type == 1) {
        add_attire_t attire = {0};
        attire.attireid = p_in->itemid;
        attire.duration = p_in->duration;
        attire.attire_lv = p_in->attirelv;
        attire.gettime = p_in->gettime;
        attire.endtime = p_in->endtime;
        ret = gf_attire.add_attire(USERID_ROLETM, &attire, 99999);
        if ( ret != SUCC ){
            STD_ROLLBACK();
            return ret;
        }
        ret = this->gf_attire.get_max_id(USERID_ROLETM, &(p_in->uniqueid));
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    } else {
        ret = gf_increase_item(USERID_ROLETM,p_in->itemid,p_in->cnt,set_item_limit(p_in->itemid),99999);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }
    p_in->coins = left_coins;
    STD_RETURN_WITH_BUF(ret, p_in, sizeof(gf_buy_goods_in));
}

int Croute_func::gf_sell_goods(DEAL_FUN_ARG)
{
    gf_sell_goods_in* p_in = PRI_IN_POS;
    gf_sell_goods_out out = {0};

   // DEBUG_LOG("uid=[%u],item=[%u %u %u %u %u]",RECVBUF_USERID, p_in->itemid, p_in->uniqueid, p_in->cnt, p_in->shop_start_tm, p_in->attire_lv, p_in->ype);
    uint32_t left_coins = 0;
	ret = gf_role.increase_int_value_with_ret(USERID_ROLETM, ROLE_XIAOMEE_COLUMN, p_in->coins * p_in->cnt, &(left_coins));
    if (p_in->type == 1) {
        ret = gf_attire.del_attire_with_index(USERID_ROLETM, p_in->uniqueid);
        if (ret != SUCC) {
            STD_ROLLBACK();
            out.err_code = ret;
            //return ret;
        }
    } else {
        ret = gf_reduce_item_count(USERID_ROLETM, p_in->itemid, p_in->cnt);
        if (ret != SUCC) {
            STD_ROLLBACK();
            out.err_code = ret;
            //return ret;
        }
    }
	if (ret == SUCC) {
		gf_shop_log.add_shop_log_indb(USERID_ROLETM, p_in);
	}
    out.type    = p_in->type;
    out.shop_id = p_in->shop_id;
    out.goodsid = p_in->goodsid;
    out.itemid  = p_in->itemid;
    out.uniqueid= p_in->uniqueid;
    out.cnt     = p_in->cnt;
    out.coins   = left_coins;

    if (out.err_code != SUCC) {
        ret = this->gf_role.get_coins(USERID_ROLETM, &(out.coins));
    }
    STD_RETURN_WITH_STRUCT(SUCC,out);
}
#if 0
int Croute_func::gf_repair_attire(DEAL_FUN_ARG)
{
	gf_repair_attire_in* p_in = PRI_IN_POS;
	gf_repair_attire_out out = {0};
	uint32_t duration = 0;
	uint32_t t_cost = 0;

	ret = this->gf_attire.get_duration_by_index(USERID_ROLETM, p_in->index_id, &duration);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

	if (p_in->max_duration < duration/50)
	{
		ERROR_LOG("db duration[%u] is more than duration in max[%u]", duration/50, p_in->max_duration);
		STD_ROLLBACK();
		return DB_DATA_ERR;
	}
	t_cost = (p_in->max_duration - duration/50) * p_in->per_price;
	//DEBUG_LOG("%u %u %u %u %u %u", p_in->attire_id, p_in->index_id, p_in->per_price, duration, p_in->max_duration, t_cost);
	ret = this->gf_role.reduce_int_value(USERID_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, 
		ROLE_XIAOMEE_COLUMN, t_cost, &(out.left_coins));
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

	ret = this->gf_attire.update_duration(USERID_ROLETM, p_in->index_id, p_in->max_duration);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	out.attire_id = p_in->attire_id;
	out.index_id = p_in->index_id;
	out.duration = MAX_DURATION_VALUE;
	STD_RETURN_WITH_STRUCT(ret,out);

}
#endif
int Croute_func::gf_repair_all_attire(DEAL_FUN_ARG)
{
	gf_repair_all_attire_in_header* p_in = PRI_IN_POS;
	gf_repair_all_attire_in_item* p_item = (gf_repair_all_attire_in_item*)(p_in + 1);
	gf_repair_all_attire_out_header out_head = {0};
	gf_repair_all_attire_out_item* p_out_item = 0;
	gf_repair_all_attire_out_item* p_out_item_base = 0;
	uint32_t duration = 0;
	uint32_t t_cost = 0;

	//check proto len
	CHECK_PRI_IN_LEN_WITHOUT_HEADER (p_in->attire_cnt * sizeof(gf_repair_all_attire_in_item));

	if (p_in->attire_cnt)
	{
		p_out_item_base =(gf_repair_all_attire_out_item*)malloc(sizeof(gf_repair_all_attire_out_item) *(p_in->attire_cnt));
		DEBUG_LOG("cnt:%u  cost:%u %p", p_in->attire_cnt, t_cost, p_out_item);
		if (!p_out_item_base)
		{
			return SYS_ERR;
		}
		memset(p_out_item_base, 0, sizeof(gf_repair_all_attire_out_item) * (p_in->attire_cnt));
		p_out_item = p_out_item_base;
	}
	for (uint32_t i = 0; i < p_in->attire_cnt; i++)
	{
		//get duration
		ret = this->gf_attire.get_duration_by_index(USERID_ROLETM, p_item->index_id, &duration);
		if (ret != SUCC)
		{
			STD_ROLLBACK();
			free(p_out_item_base);
			return ret;
		}
		//check duration
		if (p_item->max_duration < duration/50)
		{
			ERROR_LOG("db duration[%u] is more than duration in max[%u]", duration/50, p_item->max_duration);
			STD_ROLLBACK();
			free(p_out_item_base);
			return DB_DATA_ERR;
		}
		//update duration
		ret = this->gf_attire.update_duration(USERID_ROLETM, p_item->index_id, p_item->max_duration * 50);
		if (ret != SUCC)
		{
			STD_ROLLBACK();
			free(p_out_item_base);
			return ret;
		}	
		//add up cost
		t_cost += (p_item->max_duration - (duration + 49) / 50) * p_item->per_price;
		p_out_item->attire_id = p_item->attire_id;
		p_out_item->index_id = p_item->index_id;
		p_out_item->duration = p_item->max_duration;
		p_item ++;
		p_out_item ++;
	}
	//deduce coins
	DEBUG_LOG("cnt:%u  cost:%u ", p_in->attire_cnt, t_cost);
	ret = this->gf_role.reduce_int_value(USERID_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, 
		ROLE_XIAOMEE_COLUMN, t_cost, &(out_head.left_coins));
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		if (p_out_item)
		{
			free(p_out_item_base);
		}
		return ret;
	}
	
	out_head.attire_cnt = p_in->attire_cnt;
	STD_RETURN_WITH_STRUCT_LIST(ret, out_head, p_out_item_base, out_head.attire_cnt);

}


int Croute_func::gf_attire_set_int_value(DEAL_FUN_ARG)
{
	gf_attire_set_int_value_in* p_in = PRI_IN_POS;

	ret = this->gf_attire.set_attire_int_value(USERID_ROLETM, p_in->indexid, p_in->column_name, p_in->value);

	STD_RETURN(ret);
}

int Croute_func::gf_compose_attire(DEAL_FUN_ARG)
{
    char out[PROTO_MAX_SIZE] = {0};
    int cpy_len = 0;
    gf_compose_attire_in_header* p_in = PRI_IN_POS;
    gf_compose_attire_in_item* p_item = (gf_compose_attire_in_item*)(p_in + 1);
    gf_compose_attire_out *p_out = (gf_compose_attire_out*)out;
    add_attire_t p_attire_add = {0};

    char* p_offset = &(out[0]) + sizeof(*p_out);

    CHECK_PRI_IN_LEN_WITHOUT_HEADER (p_in->item_cnt * sizeof(gf_compose_attire_in_item));
    p_out->formula = p_in->method_id;
    p_out->item_cnt = p_in->item_cnt;
	p_out->is_clothes = p_in->is_clothes;
	p_out->duration = p_in->duration;
    //deduce coins
    ret = this->gf_role.reduce_int_value(USERID_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, 
        ROLE_XIAOMEE_COLUMN, p_in->coins, &(p_out->left_coins));
    if (ret != SUCC) {
        return ret;
    }
    for (uint32_t i = 0; i < p_in->item_cnt; i++) {
		if (p_item->is_clothes) {
		//delete attire
			for (uint32_t sub_it = 0; sub_it < p_item->cnt; sub_it++) {
				ret = this->gf_attire.del_attire_with_index(USERID_ROLETM, p_item->item_id);
		        if (ret != SUCC) {
		            STD_ROLLBACK();
		            return ret;
		        }
			}
		}
		else 
		{
	    //deduce items
	        ret = this->gf_reduce_item_count(USERID_ROLETM, p_item->item_id, p_item->cnt);
	        if (ret != SUCC) {
	            STD_ROLLBACK();
	            return ret;
	        }
		}
        p_item ++;
    }
	if (p_in->is_clothes) {
	    p_attire_add.attireid    = p_in->attire_id;
	    p_attire_add.gettime     =   p_in->gettime;
	    p_attire_add.duration    =  p_in->duration;
	    p_attire_add.attire_rank =      p_in->rank;
	    DEBUG_LOG("ATTIRE: id=%u, rank=%u, duration=%u",p_in->attire_id, p_in->rank, p_in->duration);
	    ret = this->gf_attire.add_attire(USERID_ROLETM, &(p_attire_add), p_in->max_bag_grid_count);
	    if (ret!=SUCC){
	        STD_ROLLBACK();
	        return ret;
	    }		
	    ret = this->gf_attire.get_max_id(USERID_ROLETM, &(p_out->unique_id));
	    if (ret!=SUCC){
	        STD_ROLLBACK();
	        return ret;
	    }	
	} else {
		ret = this->gf_increase_item(USERID_ROLETM, p_in->attire_id, p_in->duration, p_in->max_num, p_in->max_bag_grid_count);
        //gf_increase_item(USERID_ROLETM,p_in->attire_id,p_in->duration,p_in->max_num,p_in->max_bag_grid_count);
		if (ret!=SUCC){
	        STD_ROLLBACK();
	        return ret;
	    }	
	}

	if (p_in->del_item) {
		ret = gf_reduce_item_count(USERID_ROLETM, p_in->del_item, 1);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
	}

	if (p_in->random_item) {
		ret = this->gf_increase_item(USERID_ROLETM, p_in->random_item, 1, 9999, p_in->max_bag_grid_count);
		if (ret!=SUCC){
	        STD_ROLLBACK();
	        return ret;
	    }
	}
    p_out->attire_id = p_in->attire_id;
	p_out->random_item = p_in->random_item;
	p_out->del_item = p_in->del_item;
    cpy_len = sizeof(*p_item) * p_in->item_cnt;
    memcpy(p_offset, p_in + 1, cpy_len);
    p_offset += cpy_len;
    STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0]));
}

int Croute_func::gf_decompose_attire(DEAL_FUN_ARG)
{
    gf_decompose_attire_in_header* p_in = PRI_IN_POS;
    gf_decompose_attire_in_item* p_item = (gf_decompose_attire_in_item*)(p_in + 1);

    CHECK_PRI_IN_LEN_WITHOUT_HEADER (p_in->item_cnt * sizeof(gf_decompose_attire_in_item));
    //reduce attire
    ret = this->gf_attire.del_attire_with_index(USERID_ROLETM, p_in->attire_id);
    if (ret != SUCC) {
        return ret;
    }
    for (uint32_t i = 0; i < p_in->item_cnt; i++) {
        //increase items
        ret = this->gf_increase_item(USERID_ROLETM, p_item->item_id, p_item->cnt, 9999, p_in->max_item_bag_grid_count);
    //    ret = this->gf_item.increase_item_int_value(USERID_ROLETM, p_item->item_id,
    //        item_count_column, p_item->cnt, p_in->max_item_bag_grid_count);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
		p_item++;
    }
    STD_RETURN_WITH_BUF(ret, p_in, sizeof(*p_in) + p_in->item_cnt * sizeof(*p_item));
}

int Croute_func::gf_battle_use_item(DEAL_FUN_ARG)
{
	gf_battle_use_item_in* p_in = PRI_IN_POS;
	gf_battle_use_item_out out = {0};

	if (p_in->itemid == 4) {
	//reduce fumo points
		ROLE_REDUCE_VALUE(FUMO_POINTS,RECVBUF_USERID, RECVBUF_ROLETM, GF_FUMO_NOENOUGH_ERR, p_in->count, NULL);
	} else {
		//alter table
		ret = this->gf_item.reduce_item_int_value(USERID_ROLETM, p_in->itemid, item_count_column, p_in->count);
		if (ret!=SUCC)
		{ 
			STD_ROLLBACK();
			return ret;
		}
		if (p_in->market_type) 
		{
			if (!gf_market_log.notify_use_item(USERID_ROLETM, p_in->itemid, p_in->count))
			{
				ERROR_LOG("f_market_log.notify_use_item send error %u %u %u", USERID_ROLETM, p_in->itemid);
			}
		}
	}
	//get return info
	out.itemid = p_in->itemid;
	out.count = p_in->count;
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_pick_up_roll_item(DEAL_FUN_ARG)
{
	gf_pick_up_roll_item_in_header* p_in = PRI_IN_POS;
	gf_pick_up_roll_item_out out = {0};
	out.err_code = 0;
	out.roll_id = p_in->roll_id;
	out.itemid =  p_in->itemid;
	out.index_id = 0;
	if (p_in->type == 2)
	{
		ret = this->gf_increase_item(USERID_ROLETM, p_in->itemid, 1, p_in->max_num, p_in->max_item_bag_grid_count);
		if(ret != SUCC)
		{
			STD_ROLLBACK();
			goto ERR_LOGIC;
		}
	}
	else if(p_in->type == 1)
	{
		m_attire_elem.attireid = p_in->itemid;
		m_attire_elem.gettime = (uint32_t)time(NULL);
		m_attire_elem.duration = p_in->duration;
		m_attire_elem.attire_rank = 1;
        m_attire_elem.endtime = 0;
		m_attire_elem.usedflag = 0;

		ret = this->gf_attire.add_attire(USERID_ROLETM, &m_attire_elem, p_in->max_item_bag_grid_count);
		if (ret!=SUCC)
		{
			STD_ROLLBACK();
			goto ERR_LOGIC;
		}
		ret = this->gf_attire.get_max_id(USERID_ROLETM, &(out.index_id));
		if (ret!=SUCC)
		{
			STD_ROLLBACK();
			goto ERR_LOGIC;
		}
		out.itemid = p_in->itemid;
	}
	else
	{
		ret = ENUM_OUT_OF_RANGE_ERR;
		goto ERR_LOGIC;
	}
	if (!ret && p_in->unique_pick_cnt)
	{
		ret = this->gf_role.update_unique_itembit(USERID_ROLETM, p_in->uniqueitem);
		if (ret != SUCC) {
			STD_ROLLBACK();
			goto ERR_LOGIC;
		}
	}

ERR_LOGIC:
	out.err_code = ret;
	STD_RETURN_WITH_STRUCT(0, out);
}

int Croute_func::gf_pick_up_item(DEAL_FUN_ARG)
{
	gf_pick_up_item_in_header* p_in = PRI_IN_POS;
	gf_pick_up_item_out out = {0};

#define TYPE_ATTIRE 1
#define TYPE_ITEM   2
#define TYPE_MATERIAL   3
	if (p_in->type == TYPE_ITEM || p_in->type == TYPE_MATERIAL)
	{
		//add item table
		//ret = gf_item.pick_up_item(USERID_ROLETM, p_in->itemid, p_in->max_num, p_in->max_item_bag_grid_count);
        uint32_t max_limit = p_in->max_num;
        if ( p_in->itemid == 1700061 || p_in->itemid == gold_coin_item_id ) {
            max_limit = this->set_item_limit(p_in->itemid);
        }
        ret = gf_increase_item(USERID_ROLETM,p_in->itemid,1,max_limit,p_in->max_item_bag_grid_count);
		if (ret!=SUCC)
		{ 
			STD_ROLLBACK();
			return ret;
		}
        out.itemid = p_in->itemid;
		out.index_id = 0;
	}
	else if (p_in->type == TYPE_ATTIRE)
	{
		//add attire table
		m_attire_elem.attireid = p_in->itemid;
		m_attire_elem.gettime = (uint32_t)time(NULL);
		m_attire_elem.duration = p_in->duration;
		m_attire_elem.attire_rank = 1;
        m_attire_elem.endtime = 0;
		m_attire_elem.usedflag = 0;
		
		ret = this->gf_attire.add_attire(USERID_ROLETM, &m_attire_elem, p_in->max_item_bag_grid_count);
		if (ret!=SUCC)
		{ 
			return ret;
		}		
		ret = this->gf_attire.get_max_id(USERID_ROLETM, &(out.index_id));
		if (ret!=SUCC)
		{ 
			STD_ROLLBACK();
			return ret;
        }
        out.itemid = p_in->itemid;
	} else
		return ENUM_OUT_OF_RANGE_ERR;
	if (!ret && p_in->unique_pick_cnt) 
	{
		ret = this->gf_role.update_unique_itembit(USERID_ROLETM, p_in->uniqueitem);
	    if (ret != SUCC) {
			STD_ROLLBACK();
	        return ret;
	    }
	}
	//get return info
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_set_role_uinque_itembit(DEAL_FUN_ARG)
{
    gf_set_role_uinque_itembit_in* p_in = (gf_set_role_uinque_itembit_in*)PRI_IN_POS;

    ret = this->gf_role.update_unique_itembit(USERID_ROLETM, p_in->uniqueitem);
    if (ret != SUCC) {
        return ret;
    }
    STD_RETURN(SUCC);
}

int Croute_func::gf_set_role_base_info(DEAL_FUN_ARG)
{
	gf_set_role_base_info_in_header* p_in = PRI_IN_POS;
	gf_set_role_base_info_in_item* p_item = (gf_set_role_base_info_in_item*)(p_in + 1);

	CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->attire_cnt * sizeof(*p_item));
	
	ret = this->gf_role.set_role_base_info(USERID_ROLETM, p_in);
	if (ret != SUCC)
	{
		return ret;
	}
	ret = this->gf_attire.set_attire_list_duration(USERID_ROLETM, p_in->attire_cnt, p_item);
	if (ret != SUCC)
	{
		//STD_ROLLBACK(); what ever ,not need to roll back
		ERROR_LOG("set attire duration error:%u,%u attire_cnt=%u", USERID_ROLETM, p_in->attire_cnt);
		return ret;
	}	
	STD_RETURN(ret);
}

int Croute_func::gf_set_role_base_info_2(DEAL_FUN_ARG)
{
	gf_set_role_base_info_2_in_header* p_in = PRI_IN_POS;

	ret = this->gf_role.set_role_base_info_2(USERID_ROLETM, p_in);
	if (ret != SUCC)
	{  
		return ret;
	}  
	   
	STD_RETURN(ret);
}

int Croute_func::gf_set_role_pvp_info(DEAL_FUN_ARG)
{
	gf_set_role_pvp_info_in* p_in = PRI_IN_POS;
	ret = this->gf_role.set_role_pvp_info(USERID_ROLETM, p_in);
    if (ret != SUCC)
    {
        return ret;
    }
	STD_RETURN(ret);
}

int Croute_func::gf_set_role_stage_info(DEAL_FUN_ARG)
{
	gf_set_role_stage_info_in* p_in = PRI_IN_POS;

	ret = this->gf_killboss.set_stage_info(USERID_ROLETM, p_in);
	if (ret != SUCC)
	{
		return ret;
	}
	STD_RETURN(ret);
}

// task handler
int Croute_func::gf_get_task_finished(DEAL_FUN_ARG)
{
	gf_get_task_finished_out_header out_header = {0};
	gf_get_task_finished_out_item *p_out_item = NULL;
	ret = this->gf_task.get_task_finished(USERID_ROLETM, &(out_header.count), 
		&p_out_item);
	if (ret != SUCC)
	{
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}
	
int Croute_func::gf_del_outmoded_task(DEAL_FUN_ARG)
{
	gf_del_outmoded_task_in_header* p_in = PRI_IN_POS;
    gf_del_outmoded_task_in_item* p_in_item = (gf_del_outmoded_task_in_item*)(p_in+1);

	
    for (uint32_t i = 0; i < p_in->task_num; i++) {
        this->gf_task.del_task(USERID_ROLETM, p_in_item->task_id);
    }
	STD_RETURN(SUCC);
}

int Croute_func::gf_get_task_in_hand(DEAL_FUN_ARG)
{
	gf_get_task_in_hand_out_header out_header = {0};
	gf_get_task_in_hand_out_item *p_out_item = NULL;
	ret = this->gf_task.get_task_in_hand(USERID_ROLETM, &(out_header.count), 
		&p_out_item);
	if (ret != SUCC)
	{
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);

}

int Croute_func::gf_get_all_task(DEAL_FUN_ARG)
{
	gf_get_all_task_out_header out_header = {0};
	gf_get_all_task_out_item *p_out_item = NULL;
	ret = this->gf_task.get_all_task(USERID_ROLETM, &(out_header.count), 
		&p_out_item);
	if (ret != SUCC)
	{
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);

}
/**
 * @fn 获取角色的任务分类(完成/进行/放弃)列表
 * @brief private function
 */
int Croute_func::gf_get_task_classify_list(uint32_t userid, uint32_t roletm,
    uint32_t* done_task_num, uint32_t* doing_task_num, uint32_t* cancel_task_num,
    char* outbuf, int* outlen)
{
	char* p_offset = outbuf;
	int cpy_len = 0;

	gf_get_task_finished_out_item *p_done_out_item = NULL;
	ret = this->gf_task.get_task_finished(userid, roletm, done_task_num, &p_done_out_item);
	if (ret != SUCC) {
		return ret;
	}
	// copy finish task id to out buf
    if (p_done_out_item) {
        cpy_len = sizeof(*p_done_out_item) * (*done_task_num);
        memcpy(p_offset, p_done_out_item, cpy_len);
        //DEBUG_LOG("get finish task list:uid=[%u],task_id=[%d]", RECVBUF_USERID, p_done_out_item->taskid);
        p_offset += cpy_len;
        free(p_done_out_item);
    }

	gf_get_task_in_hand_out_item *p_doing_out_item = NULL;
	ret = this->gf_task.get_task_in_hand(userid, roletm, doing_task_num, &p_doing_out_item);
	if (ret != SUCC) {
		return ret;
	}
	// copy doing task information to out buf
    if (p_doing_out_item) {
        cpy_len = sizeof(*p_doing_out_item) * (*doing_task_num);
        memcpy(p_offset, p_doing_out_item, cpy_len);
        //DEBUG_LOG("get doing task list:uid=[%u],task_id=[%d]", RECVBUF_USERID, p_doing_out_item->taskid);
        p_offset += cpy_len;
        free(p_doing_out_item);
    }

    gf_get_task_cancel_out_item *p_cancel_out = NULL;
    ret = this->gf_task.get_task_cancel(userid, roletm, cancel_task_num, &p_cancel_out);
	if (ret != SUCC) {
		return ret;
	}
	// copy cancel task information to out buf
    if (p_cancel_out) {
        cpy_len = sizeof(*p_cancel_out) * (*cancel_task_num);
        memcpy(p_offset, p_cancel_out, cpy_len);
        //DEBUG_LOG("get cancel task list:uid=[%u],task_id=[%u]", RECVBUF_USERID, p_cancel_out->taskid);
        p_offset += cpy_len;
        free(p_cancel_out);
    }
    DEBUG_LOG("task classify:uid=[%u],done_num=[%u],doing_num=[%u],cancel_num=[%u]",userid,
        *done_task_num, *doing_task_num, *cancel_task_num);
    *outlen = p_offset - outbuf;
    return SUCC;
}

int Croute_func::gf_get_task_list(DEAL_FUN_ARG)
{
	char out[PROTO_MAX_SIZE] = {0};
	gf_get_task_list_out_header* p_out_header = (gf_get_task_list_out_header*)out;
	char* p_offset = &(out[0]) + sizeof(*p_out_header);

	int cpy_len = 0;

	gf_get_task_finished_out_item *p_done_out_item = NULL;
	ret = this->gf_task.get_task_finished(USERID_ROLETM, &(p_out_header->done_task_num), 
			&p_done_out_item);
	if (ret != SUCC) {
		return ret;
	}
	// copy finish task id to out buf
    if (p_done_out_item) {
        cpy_len = sizeof(*p_done_out_item) * (p_out_header->done_task_num);
        memcpy(p_offset, p_done_out_item, cpy_len);
        DEBUG_LOG("get finish task list:uid=[%u],task_id=[%d]", RECVBUF_USERID, p_done_out_item->taskid);
        p_offset += cpy_len;
        free(p_done_out_item);
    }

	gf_get_task_in_hand_out_item *p_doing_out_item = NULL;
	ret = this->gf_task.get_task_in_hand(USERID_ROLETM, &(p_out_header->doing_task_num), 
			&p_doing_out_item);
	if (ret != SUCC) {
		return ret;
	}
	// copy doing task information to out buf
    if (p_doing_out_item) {
        cpy_len = sizeof(*p_doing_out_item) * (p_out_header->doing_task_num);
        memcpy(p_offset, p_doing_out_item, cpy_len);
        DEBUG_LOG("get doing task list:uid=[%u],task_id=[%d]", RECVBUF_USERID, p_doing_out_item->taskid);
        p_offset += cpy_len;
        free(p_doing_out_item);
    }

    gf_get_task_cancel_out_item *p_cancel_out = NULL;
    ret = this->gf_task.get_task_cancel(USERID_ROLETM, &(p_out_header->cancel_task_num), 
			&p_cancel_out);
	if (ret != SUCC) {
		return ret;
	}
	// copy cancel task information to out buf
    if (p_cancel_out) {
        cpy_len = sizeof(*p_cancel_out) * (p_out_header->cancel_task_num);
        memcpy(p_offset, p_cancel_out, cpy_len);
        DEBUG_LOG("get cancel task list:uid=[%u],task_id=[%u]", RECVBUF_USERID, p_cancel_out->taskid);
        p_offset += cpy_len;
        free(p_cancel_out);
    }

	STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0]));

}

int Croute_func::gf_set_task_flg(DEAL_FUN_ARG)
{
	gf_set_task_flg_in *p_in = PRI_IN_POS;
	gf_set_task_flg_out out={0};

	if (p_in->flag==SET_TASK_IN_HAND)
	{
		ret = this->gf_task.set_task_in_hand(USERID_ROLETM, p_in->taskid);
	}
	else if (p_in->flag==SET_TASK_FINISHED)
	{
		ret = this->gf_task.set_task_finished(USERID_ROLETM, p_in->taskid);
	}
	else if (p_in->flag==SET_TASK_NULL)
	{
		ret = this->gf_task.del_task(USERID_ROLETM, p_in->taskid);
    }
    else if (p_in->flag==SET_TASK_CANCEL)
    {
        ret =this->gf_task.set_task_cancel(USERID_ROLETM, p_in->taskid);
    }
	else
		ret = ENUM_OUT_OF_RANGE_ERR;
	out.id = p_in->taskid;
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_set_task_value(DEAL_FUN_ARG)
{
	gf_set_task_value_in_header *p_in = PRI_IN_POS;
	gf_set_task_value_out out;

	if (p_in->value_type==SET_TASK_AS_VALUE)
	{
		CHECK_PRI_IN_LEN_WITHOUT_HEADER(TASK_AS_VALUE_LEN);
		
		ret = this->gf_task.set_task_as_value(USERID_ROLETM,
			p_in->taskid, p_in->binary_arr, TASK_AS_VALUE_LEN);
	}
	else if (p_in->value_type==SET_TASK_SER_VALUE)
	{
		CHECK_PRI_IN_LEN_WITHOUT_HEADER(TASK_SER_VALUE_LEN);
		
		ret = this->gf_task.set_task_ser_value(USERID_ROLETM,
			p_in->taskid, p_in->binary_arr, TASK_SER_VALUE_LEN);	
	}
	else
		ret = ENUM_OUT_OF_RANGE_ERR;
	
	out.value_type = p_in->value_type;
	STD_RETURN_WITH_STRUCT(ret, out);
}
/**************************************************************************
 *  add the follow tow function for deal item (item/strengthen material)  *
 **************************************************************************/
int Croute_func::gf_reduce_item_count(uint32_t userid, uint32_t roletm, uint32_t id, uint32_t cnt)
{
    int ret = 0;
    if (is_strengthen_material(id)) {
        ret = this->gf_material.reduce_material_count(userid, roletm, id, cnt);
    } else {
        ret = this->gf_item.reduce_item_int_value(userid, roletm, id, item_count_column, cnt);
    }
    if (ret != SUCC) {
	STD_ROLLBACK();
	return ret;
    }
    return ret;
}
int Croute_func::gf_increase_item(uint32_t userid, uint32_t roletm, uint32_t id, uint32_t cnt, uint32_t pile, uint32_t max_bag, 
	item_change_log_opt_type is_vip_opt, bool log_flg)
{
    int ret = 0;
    if (is_strengthen_material(id)) {
        ret = this->gf_material.add_material(userid, roletm, id, cnt, pile, max_bag, is_vip_opt, log_flg);
    } else {
        ret = this->gf_item.add_item(userid, roletm, id, cnt, pile, max_bag, is_vip_opt, log_flg);
    }
    return ret;
}
int Croute_func::gf_get_item_count(uint32_t userid, uint32_t roletm, uint32_t id, uint32_t* cnt)
{
    int ret = 0;
    if (is_strengthen_material(id)) {
        ret = this->gf_material.get_material_cnt_by_id(userid, roletm, cnt, id);
    } else {
        ret = this->gf_item.get_item_int_value(userid, roletm, id,"count", cnt);
    }
    return ret;
}

int Croute_func::gf_task_swap_item(DEAL_FUN_ARG)
{
	gf_task_swap_item_in_header *p_in = PRI_IN_POS;
	gf_task_swap_item_in_item_1 *p_item1 = (gf_task_swap_item_in_item_1*)(p_in + 1);
	gf_task_swap_item_in_item_2* p_item2 = NULL;
	gf_task_swap_item_out_header out_header = {0};
	gf_task_swap_item_out_item *p_out_item = 0;
	uint32_t attire_cnt = 0;

	//check proto len
	CHECK_PRI_IN_LEN_WITHOUT_HEADER(
		(p_in->del_count * sizeof(gf_task_swap_item_in_item_1) + 
		p_in->add_count * sizeof(gf_task_swap_item_in_item_2)));
	
	//del items when finishing task
	for (uint32_t i=0;i<p_in->del_count;i++)
	{
		switch (p_item1->type)
		{
			case TYPE_ATTIRE:
				ret = this->gf_attire.del_attire_by_attireid(USERID_ROLETM, p_item1->id);
				break;
			case TYPE_ITEM:
				//ret = gf_item.reduce_item_int_value(USERID_ROLETM,p_item1->id,item_count_column,p_item1->count);
                ret = gf_reduce_item_count(USERID_ROLETM, p_item1->id, p_item1->count);
				if (ret == GF_ITEM_NOFIND_ERR)
					ret = GF_ITEM_NOTENOUGH_ERR;
				break;
			case TYPE_ROLE:
				if (p_item1->id==TYPE_ROLE_XIAOMEE)
					ROLE_REDUCE_VALUE(XIAOMEE,RECVBUF_USERID, RECVBUF_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, p_item1->count, NULL);
				else if (p_item1->id==TYPE_ROLE_EXP)
					ROLE_REDUCE_VALUE(EXP,RECVBUF_USERID, RECVBUF_ROLETM, GF_EXP_NOENOUGH_ERR, p_item1->count, NULL);
                else if (p_item1->id==TYPE_ROLE_FUMOPOINT)
					ROLE_REDUCE_VALUE(FUMO_POINTS,RECVBUF_USERID, RECVBUF_ROLETM, GF_FUMO_NOENOUGH_ERR, p_item1->count, NULL);
				else if (p_item1->id==TYPE_ROLE_PVP_FIGHT)
									ROLE_REDUCE_VALUE(EXPLOIT,RECVBUF_USERID,RECVBUF_ROLETM,GF_NUM_NOENOUGH_ERR, p_item1->count, NULL);

				else
				{
					STD_ROLLBACK();
					return ENUM_OUT_OF_RANGE_ERR;
				}
				break;
			default:
				STD_ROLLBACK();
				return ENUM_OUT_OF_RANGE_ERR;
				break;
		}
		
		if (ret!=SUCC)
		{
			STD_ROLLBACK();
			return ret;			
		}
		p_item1 ++;
	}

    uint32_t max_limit = 0;
	//add items when finishing task
	p_item2 = (gf_task_swap_item_in_item_2*)p_item1;
	for (uint32_t i = 0; i < p_in->add_count; i++)
	{
		switch (p_item2->type)
		{
			case TYPE_ATTIRE:
				
				for (uint32_t j=0;j<p_item2->count;j++)
				{
					m_attire_elem.attireid = p_item2->id;
					m_attire_elem.gettime = time(NULL);
					m_attire_elem.attire_rank = 1;
					m_attire_elem.duration = p_item2->duration;
                    if (p_item2->lifetime == 0) {
                        m_attire_elem.endtime = 0;
                    } else {
                        m_attire_elem.endtime = m_attire_elem.gettime + ((p_item2->lifetime+1) * 3600 * 24);
                    }
					m_attire_elem.usedflag = 0;
					ret = this->gf_attire.add_attire(USERID_ROLETM, &m_attire_elem, p_in->max_item_bag_grid_count);
					if (ret!=SUCC)
					{
						STD_ROLLBACK();
						return ret;			
					}
				}
				attire_cnt ++;
				break;
			case TYPE_ITEM:
                max_limit = p_item2->max;
                if (p_item2->id == 1700061 || p_item2->id == gold_coin_item_id) {
                    max_limit = this->set_item_limit(p_item2->id);
                }
                ret = gf_increase_item(USERID_ROLETM,p_item2->id,p_item2->count,max_limit,p_in->max_item_bag_grid_count);
				break;
			case TYPE_ROLE:
                
				if (p_item2->id==TYPE_ROLE_XIAOMEE)
					ret = this->gf_role.increase_int_value(USERID_ROLETM,
						ROLE_XIAOMEE_COLUMN, p_item2->count);
				else if (p_item2->id==TYPE_ROLE_EXP)
					ret = this->gf_role.increase_int_value(USERID_ROLETM, 
						ROLE_EXP_COLUMN, p_item2->count);
				else if (p_item2->id==TYPE_ROLE_SKILLPOINT)
					ret = this->gf_role.increase_int_value(USERID_ROLETM, 
						ROLE_SKILL_POINT_COLUMN, p_item2->count);
				else if (p_item2->id==TYPE_ROLE_FUMOPOINT)
					ret = this->gf_role.increase_int_value(USERID_ROLETM, 
						ROLE_FUMO_POINTS_COLUMN, p_item2->count);
				else if (p_item2->id==TYPE_ROLE_PVP_FIGHT)
                    ret = this->gf_role.increase_int_value(USERID_ROLETM,
                        ROLE_EXPLOIT_COLUMN, p_item2->count);
				else if (p_item2->id==TYPE_ROLE_HONOR)
					ret = this->gf_role.increase_int_value(USERID_ROLETM,
                        ROLE_HONOR_COLUMN, p_item2->count);
                else if (p_item2->id==TYPE_ROLE_YAOSHI_EXP)
                    ret = this->gf_secondary_pro.add_player_secondary_pro_exp(USERID_ROLETM,
                        1, p_item2->count, p_item2->max);
				else if (p_item2->id == TYPE_ROLE_ALLOCATOR_EXP)
					ret = this->gf_role.increase_int_value(USERID_ROLETM, ROLE_ALLOCATOR_EXP_COLUMN, p_item2->count);
				else
				{
					STD_ROLLBACK();
					return ENUM_OUT_OF_RANGE_ERR;
				}
				break;
			case TYPE_SKILL:
				ret = this->gf_skill.add_skill(USERID_ROLETM, p_item2->id, 1);
				if (ret != SUCC) {
					STD_ROLLBACK();
					return ret;	
				}
			default:
				STD_ROLLBACK();
                DEBUG_LOG("there %u ", p_item2->type);
				return ENUM_OUT_OF_RANGE_ERR;
				break;
		}
		
		if (ret!=SUCC)
		{
			STD_ROLLBACK();
			return ret;			
		}
		p_item2 ++;

	}
	
	ret = this->gf_role.get_xiaomee_exp_skpt(USERID_ROLETM, &(out_header.left_coins), 
		&(out_header.exp), &(out_header.allocator_exp),  &(out_header.skill_pt_left), &(out_header.fumo_pt_left), 
		&(out_header.honor));
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;			
	}	

	if (attire_cnt)
	{
		ret = this->gf_attire.get_max_id_list(USERID_ROLETM, attire_cnt, &(out_header.attire_cnt), &(p_out_item));
		if (ret != SUCC || out_header.attire_cnt != attire_cnt)
		{
			STD_ROLLBACK();
			return ret;			
		}
	}
	
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.attire_cnt);
}


//skill handler 
int Croute_func::gf_add_skill_points(DEAL_FUN_ARG)
{
	gf_add_skill_points_in * p_in = PRI_IN_POS;
	gf_add_skill_points_out  out = {0};

	ret = this->gf_role.increase_int_value_with_ret(USERID_ROLETM,
		"skill_point", p_in->skill_points_add, &(out.left_points));
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	ret = this->gf_item.reduce_item_int_value(USERID_ROLETM, p_in->skill_book_id, item_count_column, 1);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	out.skill_book_id = p_in->skill_book_id;
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_add_skill_with_no_book(DEAL_FUN_ARG)
{
	gf_add_skill_with_no_book_in * p_in = PRI_IN_POS;
	gf_add_skill_with_no_book_out  out = {0};

	ret = this->gf_skill.add_skill(USERID_ROLETM, p_in->skill_id, p_in->skill_lv);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	out.skill_id = p_in->skill_id;
	STD_RETURN_WITH_STRUCT(ret, out);
}


int Croute_func::gf_del_skill(DEAL_FUN_ARG)
{
	gf_del_skill_in * p_in = PRI_IN_POS;
	ret = this->gf_skill.del_skill(USERID_ROLETM, p_in);
	STD_RETURN(ret);
}

int Croute_func::gf_learn_new_skill(DEAL_FUN_ARG)
{
	gf_learn_new_skill_in* p_in = PRI_IN_POS;
	gf_learn_new_skill_out out = { 0 };

	ret = this->gf_role.reduce_skill_point(USERID_ROLETM, p_in->skill_point, &(out.left_point));
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	
	ret = this->gf_skill.add_skill(USERID_ROLETM, p_in->skill_id, p_in->skill_lv);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	if (p_in->item_id) {
    	ret = this->gf_item.reduce_item_int_value(USERID_ROLETM, p_in->item_id, "count", 1);
		if (ret != SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}
	}
	out.skill_id = p_in->skill_id;
	out.skill_lv = p_in->skill_lv;
	out.item_id = p_in->item_id;
	STD_RETURN_WITH_STRUCT(ret, out);
}

/*
uint32_t skill_id;
uint32_t skill_lv; //skill cur lv
uint32_t to_point; //which added to skill
uint32_t skill_point; // needed by upgrade
uint32_t coins; // needed by upgrade
uint8_t role_lv_reach;// needed by upgrade
*/
/*
uint32_t skill_id;
uint32_t skill_lv;
uint32_t skill_point; // the skill have
uint32_t left_point; // the role have
uint32_t left_coins;
*/
int Croute_func::gf_upgrade_skill(DEAL_FUN_ARG)
{
	gf_upgrade_skill_in * p_in = PRI_IN_POS;
	gf_upgrade_skill_out out = {0};

	//get point and lv of skill
	skill_info_t skill_info = {0};
	ret = this->gf_skill.get_skill_info(USERID_ROLETM, p_in->skill_id, &skill_info);
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}	
	if (p_in->skill_lv != skill_info.skill_lv + 1)
	{
		STD_ROLLBACK();
		return DB_ERR;
	}

	//reduce role's skill point
	ret = this->gf_role.reduce_skill_point(USERID_ROLETM, p_in->skill_point, &(out.left_point));
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	//reduce role's coins
	ret = this->gf_role.reduce_int_value(USERID_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, ROLE_XIAOMEE_COLUMN, 
	p_in->coins, &(out.left_coins));
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

	//upgrade skill level
	ret = this->gf_skill.skill_level_up(USERID_ROLETM, p_in->skill_id);
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	out.skill_id = p_in->skill_id;
	out.skill_lv = p_in->skill_lv;
	out.use_point = p_in->skill_point;
	STD_RETURN_WITH_STRUCT(ret, out);
}

/*
int Croute_func::gf_upgrade_skill(DEAL_FUN_ARG)
{
	gf_upgrade_skill_in * p_in = PRI_IN_POS;
	gf_upgrade_skill_out out = {0};

	//get point and lv of skill
	skill_info_t skill_info = {0};
	ret = this->gf_skill.get_skill_info(USERID_ROLETM, p_in->skill_id, &skill_info);
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}	
	if (p_in->skill_lv != skill_info.skill_lv)
	{
		STD_ROLLBACK();
		return DB_ERR;
	}

	//reduce role's skill point
	ret = this->gf_role.reduce_skill_point(USERID_ROLETM, p_in->to_point, &(out.left_point));
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

	
	//if skill point + to point > lv point :   point enough to upgrade lv
	DEBUG_LOG("%u %u %u", p_in->to_point, p_in->skill_point, skill_info.skill_point);
	if (skill_info.skill_point + p_in->to_point >= p_in->skill_point)
	{
		if (!p_in->role_lv_reach)
		{
			STD_ROLLBACK();
			return ROLE_LV_NOT_REACH_ERR;
		}
	 	//upgrade skill and update skill's point
	 	skill_info_t update_info(skill_info);
		update_info.skill_point = skill_info.skill_point + p_in->to_point - p_in->skill_point;
		update_info.skill_lv ++;
	  	ret = this->gf_skill.update_skill_info(USERID_ROLETM, p_in->skill_id, &update_info);
		if (ret!=SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}	
	  	//reduce coins
	  	ret = this->gf_role.reduce_int_value(USERID_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, ROLE_XIAOMEE_COLUMN, 
		p_in->coins, &(out.left_coins));
		if (ret!=SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}
		
		out.skill_lv = p_in->skill_lv + 1;	
		out.skill_point = skill_info.skill_point + p_in->to_point - p_in->skill_point;
	}
	else
	{
		//update skill's point: skill point + to point 
		ret = this->gf_skill.update_skill_point(USERID_ROLETM, p_in->skill_id, skill_info.skill_point + p_in->to_point);
	 	if (ret!=SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}
		out.skill_lv = p_in->skill_lv;	
		out.skill_point = skill_info.skill_point + p_in->to_point;

		//get coins
		ret = this->gf_role.get_coins(USERID_ROLETM, &(out.left_coins));
		if (ret!=SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}
	}
	out.skill_id = p_in->skill_id;
	
	STD_RETURN_WITH_STRUCT(ret, out);




	// reduce role's skill points
	ret = this->gf_role.reduce_skill_point(USERID_ROLETM, p_in->skill_point, &(out.left_point));
	if (ret!=SUCC)
	{
		return ret;
	}
	
	// reduce role's coins
	ret = this->gf_role.reduce_int_value(USERID_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, ROLE_XIAOMEE_COLUMN, 
		p_in->coins, &(out.left_coins));
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

	//update skill level to specified value 
	ret = this->gf_skill.upgrade_skill(USERID_ROLETM, p_in);
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

	//construct return info
	out.skill_id = p_in->skill_id;
	out.skill_lv = p_in->skill_lv + 1;	

	STD_RETURN_WITH_STRUCT(ret, out);
}
*/

/*
int Croute_func::gf_reset_skill(DEAL_FUN_ARG)
{
	gf_reset_skill_in* p_in = PRI_IN_POS;
	gf_reset_skill_out_header out_header = {0};
	uint32_t all_use_sp = p_in->used_sp;
//	ret = this->gf_skill.get_skill_points(USERID_ROLETM, &all_use_sp);
//	if (ret!=SUCC)
//	{
//		STD_ROLLBACK();
//		return ret;
//	}
	ret = this->gf_skill.reset_skills(USERID_ROLETM);
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	ret = this->gf_role.increase_int_value_with_ret(USERID_ROLETM, ROLE_SKILL_POINT_COLUMN, all_use_sp, &(out_header.left_skill_points));
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT(ret, out_header);
}
*/
int Croute_func::gf_reset_skill(DEAL_FUN_ARG)
{
	gf_reset_skill_in* p_in = PRI_IN_POS;
	gf_reset_skill_out_header out_header = {0};
	uint32_t all_use_sp = p_in->used_sp;
//	ret = this->gf_skill.get_skill_points(USERID_ROLETM, &all_use_sp);
//	if (ret!=SUCC)
//	{
//		STD_ROLLBACK();
//		return ret;
//	}
	ret = this->gf_skill.clear_skills(USERID_ROLETM);
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	ret = this->gf_role.update_skillbind(USERID_ROLETM, "", 0);
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	ret = this->gf_role.increase_int_value_with_ret(USERID_ROLETM, ROLE_SKILL_POINT_COLUMN, all_use_sp, &(out_header.left_skill_points));
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT(ret, out_header);
}


int Croute_func::gf_get_skill_list(DEAL_FUN_ARG)
{
	gf_get_skill_list_out_header out_header = {0};
	gf_get_skill_list_out_item *p_out_item = NULL;
	ret = this->gf_role.get_int_value(USERID_ROLETM, ROLE_SKILL_POINT_COLUMN, &(out_header.left_sp));
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	ret = this->gf_skill.get_skill_list(USERID_ROLETM, &(out_header.count), 
		&p_out_item);
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}


int Croute_func::gf_set_skill_bind_key(DEAL_FUN_ARG)
{
	gf_set_skill_bind_key_in_header* p_in = PRI_IN_POS;
	gf_set_skill_bind_key_in_item* p_in_item = (gf_set_skill_bind_key_in_item*)(p_in + 1);
	
	gf_get_skill_bind_key_out_header out_header = {0};
	gf_get_skill_bind_key_out_item* p_out_item = (gf_get_skill_bind_key_out_item*)m_tmpinfo;


	//check proto len
	CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->count * sizeof(gf_set_skill_bind_key_in_item));

#if 1
	ret = this->gf_role.update_skillbind(USERID_ROLETM, (char*)p_in, sizeof(*p_in) + p_in->count * sizeof(gf_set_skill_bind_key_in_item));
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	//get skill lv for each
	gf_get_skill_bind_key_out_item* p_cur_out = p_out_item;
	for (uint32_t iter=0; iter < p_in->count; iter++)
	{	
		p_cur_out->skill_id = p_in_item->skill_id;
		p_cur_out->bind_key = p_in_item->bind_key;
		
		ret = this->gf_skill.get_skill_lv(USERID_ROLETM, p_in_item->skill_id, &(p_cur_out->skill_lv));
	//	DEBUG_LOG("set key[%u] : %u  %u  %u", p_in->count, p_in_item->skill_id, p_in_item->bind_key, p_cur_out->skill_lv);
		if (ret!=SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}		
		p_in_item++;
		p_cur_out++;
	}

	out_header.count = p_in->count;
	STD_RETURN_WITH_STRUCT_BUF(ret, out_header, m_tmpinfo, p_in->count*sizeof(*p_out_item));

#else
	//clear all bind key first
	ret = this->gf_skill.clear_skill_bind_key(USERID_ROLETM);
	if (ret!=SUCC)
	{
		return ret;
	}

	//set bind key for each
	for (uint32_t iter=0;iter<p_in->count;iter++)
	{
		ret = this->gf_skill.set_bind_key(USERID_ROLETM, p_in_item->skill_id,
			p_in_item->bind_key);
		if (ret!=SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}		
		p_in_item++;
	}

	//get skill info  for return
	ret = this->gf_skill.get_skill_bind_key(USERID_ROLETM,
		&(out_header.count), &p_out_item);
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
#endif
}
/**
 * @brief private function
 */
int Croute_func::gf_get_skill_bind_private(uint32_t userid, uint32_t role_tm, uint32_t* p_count, gf_get_skill_bind_key_out_item* p_out_item)
{
	ret = this->gf_role.get_skillbind(userid, role_tm, m_tmpinfo);
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	*p_count = *(uint32_t*)m_tmpinfo;
	gf_skill_bind_stru* p_bind = (gf_skill_bind_stru*)(m_tmpinfo + 4);

	gf_get_skill_bind_key_out_item* p_cur_out = p_out_item;
	for (uint32_t i = 0; i < *p_count; i++)
	{
		p_cur_out->skill_id = p_bind->skill_id;
		p_cur_out->bind_key = p_bind->bind_key;
		//DEBUG_LOG("bind key[%u] : %u   %u", *p_count, p_bind->skill_id, p_bind->bind_key);
		ret = this->gf_skill.get_skill_lv(userid, role_tm, p_bind->skill_id, &(p_cur_out->skill_lv));
		if (ret!=SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}		
		p_bind++;
		p_cur_out++;
	}
	return ret;
}

int Croute_func::gf_get_all_skills_info(uint32_t userid, uint32_t role_tm, 
	uint32_t* p_count, gf_get_skill_bind_key_out_item* p_out_item)
{
	ret = this->gf_role.get_skillbind(userid, role_tm, m_tmpinfo);
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	uint32_t bind_cnt = *(uint32_t*)m_tmpinfo;
	gf_skill_bind_stru* p_bind = (gf_skill_bind_stru*)(m_tmpinfo + 4);

	gf_get_skill_bind_key_out_item* p_list = 0;
	ret = this->gf_skill.get_login_skills_info(userid, role_tm, p_count, &p_list);
	if (p_list) 
	{
		memcpy(p_out_item, p_list, (*p_count) * sizeof(*p_list));
		for (uint32_t i = 0; i < *p_count; i++)
		{
			gf_get_skill_bind_key_out_item* p_cur_out = p_out_item + i;
			for (uint32_t j = 0; j < bind_cnt; j++)
			{
				gf_skill_bind_stru* p_bind_cur = p_bind + j;
				if (p_bind_cur->skill_id == p_cur_out->skill_id) {
					p_cur_out->bind_key = p_bind_cur->bind_key;
					break;
				}
			}
		}
		
		free(p_list);
		p_list = 0;
	}
/*	
	gf_get_skill_bind_key_out_item* p_cur_out = p_out_item;
	for (uint32_t i = 0; i < *p_count; i++)
	{
		p_cur_out->skill_id = p_bind->skill_id;
		p_cur_out->bind_key = p_bind->bind_key;
		//DEBUG_LOG("bind key[%u] : %u   %u", *p_count, p_bind->skill_id, p_bind->bind_key);
		ret = this->gf_skill.get_skill_lv(userid, role_tm, p_bind->skill_id, &(p_cur_out->skill_lv));
		if (ret!=SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}		
		p_bind++;
		p_cur_out++;
	}
*/
	return ret;

}
/**
 * @fn php manage
 * @brief cmd:0x06C5
 */
int Croute_func::gf_get_skill_bind_key(DEAL_FUN_ARG)
{
	gf_get_skill_bind_key_out_header out_header = {0};
	gf_get_skill_bind_key_out_item* p_out_item = (gf_get_skill_bind_key_out_item*)m_tmpinfo1;

#if 1
	gf_get_skill_bind_private(USERID_ROLETM, &(out_header.count), (gf_get_skill_bind_key_out_item*)m_tmpinfo1);
	STD_RETURN_WITH_STRUCT_BUF(ret, out_header, m_tmpinfo1, out_header.count * sizeof(*p_out_item));
#else
	ret = this->gf_skill.get_skill_bind_key(USERID_ROLETM,
		&(out_header.count),&p_out_item);
	
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
#endif
}

int Croute_func::gf_game_input(DEAL_FUN_ARG)
{
	gf_game_input_in_header* p_in = PRI_IN_POS;
//	gf_game_input_in_item* p_in_item = (gf_game_input_in_item*)(p_in+1);

	gf_game_input_out_header out_header = {0};
	gf_game_input_out_item* p_out_item=NULL;
	
	ret = this->gf_role.increase_int_value(USERID_ROLETM, ROLE_XIAOMEE_COLUMN, p_in->coins);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;	
	}

	ret = this->gf_role.increase_int_value(USERID_ROLETM, ROLE_EXP_COLUMN, p_in->exp);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;	
	}	

	ret = this->gf_role.get_coin_exp(USERID_ROLETM, &(out_header.coins_left), &(out_header.exp));
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;	
	}	
	out_header.coins_adj = p_in->coins;
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.item_cnt);
}

int Croute_func::gf_logout(DEAL_FUN_ARG)
{
	gf_logout_in* p_in = PRI_IN_POS;
    DEBUG_LOG("LOGOUT: mapid=%u xpos=%u ypos=%u",p_in->mapid,p_in->xpos,p_in->ypos);

	//must first do 
	ret = this->del_daily_loop(USERID_ROLETM);
	ret = this->gongfu.update_onlinetime(RECVBUF_USERID, p_in->timediff);

	ret = this->gf_role.logout(USERID_ROLETM, p_in);

	STD_RETURN(SUCC);
}

int Croute_func::gf_add_killed_boss(DEAL_FUN_ARG)
{
	gf_add_killed_boss_in* p_in = PRI_IN_POS;

	ret = this->gf_killboss.add_killed_boss(USERID_ROLETM, p_in);
	STD_RETURN(ret);

}

int Croute_func::gf_get_killed_boss(DEAL_FUN_ARG)
{
	gf_get_killed_boss_out_header out_header = {0};

	gf_get_killed_boss_out_item *p_out_item = NULL;
	ret = this->gf_killboss.get_killed_boss(USERID_ROLETM, &(out_header.cnt), 
		&p_out_item);
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.cnt);
}

int Croute_func::other_process(userid_t userid, uint32_t role_regtime)
{
	this->gf_task.del_task(userid, role_regtime, 1158);
	this->gf_task.del_task(userid, role_regtime, 1159);
	this->gf_task.del_task(userid, role_regtime, 1160);
	this->gf_role.reset_pvp_fight(userid, role_regtime);
	uint32_t count = 0;
	ret = this->gf_task.get_task_list_count(userid, role_regtime, "4,45", &count);
	
	if (count) {
	//finish task
		uint32_t summon_cnt = 0;
		ret = this->gf_summon.get_summon_cnt(userid, role_regtime, "1001,1002,1003,1004,1005,1006,1007", &summon_cnt);
		
		if (!summon_cnt) {
		//and not have summon
			uint32_t item_cnt = 0;
			ret = this->gf_item.get_item_cnt_by_itemid(userid, role_regtime, &item_cnt, 1400047);
			if (!item_cnt) {
			//and not have this item
				gf_increase_item(userid, role_regtime, 1400047, 1, 99, 9999);
			}
		}
	}
	
	return 0;
}

int Croute_func::del_daily_loop(userid_t userid, uint32_t role_regtime)
{
    time_t timep = time(NULL);
	uint32_t tm_now = timep;
    struct tm *p_tm_cur = localtime( &timep );
    p_tm_cur->tm_hour = 0;
    p_tm_cur->tm_min  = 0;
    p_tm_cur->tm_sec  = 0;
    int cur_year  = p_tm_cur->tm_year;
    int cur_month = p_tm_cur->tm_mon;
//    int cur_mday   = p_tm_cur->tm_mday;
    int cur_wday = p_tm_cur->tm_wday;
    uint32_t day_time = mktime(p_tm_cur);

	uint32_t user_ol = 0;
    this->gongfu.get_int_value(userid, "Ol_last", &user_ol);
	time_t time_last = user_ol;
	struct tm *p_tm_last = localtime( &time_last );
	int last_year  = p_tm_last->tm_year;
    int last_month = p_tm_last->tm_mon;
 //   int last_mday   = p_tm_last->tm_mday;
    int last_wday = p_tm_last->tm_wday;


	this->gf_other_active.clear_player_out_tm_active_info(userid, role_regtime);

	DEBUG_LOG("%u == %u | %u == %u ", last_wday, cur_wday, tm_now, user_ol + 7 * 24 * 3600);
	if (day_time >= user_ol) {
		uint32_t tmp_userid = 0;
		uint32_t tmp_role_tm = 0;
		this->gf_role.get_role_need_physics_del(userid, &tmp_userid, &tmp_role_tm);
		if (tmp_userid && tmp_role_tm) {
			del_role_phy(tmp_userid, tmp_role_tm);
		}

		
		ret = this->gf_swap_action.clear_action(userid, role_regtime,0);//daily

		ret = this->gf_other_active.clear_player_out_date_active_info(userid, role_regtime, reset_daily);


		if (last_year!= cur_year) {
			ret = this->gf_swap_action.clear_action(userid, role_regtime,2);//monthly
			ret = this->gf_swap_action.clear_action(userid, role_regtime,3);//yearly
			ret = this->gf_other_info.replace_other_info(userid, role_regtime, 9, 0);//monthly
		} 
		if (last_month != cur_month) {
			ret = this->gf_swap_action.clear_action(userid, role_regtime,2);//monthly
			ret = this->gf_other_info.replace_other_info(userid, role_regtime, 9, 0);//monthly
			ret = this->gf_other_active.clear_player_out_date_active_info(userid, role_regtime, reset_month);
		} 
		if ((cur_wday < last_wday && (last_wday < 5 || (tm_now >= user_ol + 7 * 24 * 3600))) ||
					(cur_wday > last_wday && last_wday < 5 && cur_wday >= 5) ||
					(cur_wday >= last_wday && (tm_now >= user_ol + 7 * 24 * 3600)) ||
					(cur_wday == last_wday && (tm_now >= user_ol + 6 * 24 * 3600)) ||
					(last_wday < 5 && cur_wday >= 5)) {
			ret = this->gf_swap_action.clear_action(userid, role_regtime,1);//weekly

			ret = this->gf_other_active.clear_player_out_date_active_info(userid, role_regtime, reset_weekly);
		} 
		
		ret = this->gongfu.set_int_value(userid, 0, "Ol_today");
		if (ret != SUCC) {
            return ret;
        }
		this->gongfu.set_int_value(userid, time(NULL), "Ol_last");
	}
	
	
    uint32_t Ol_last = 0;
    ret = this->gf_role.get_role_Ol_last(userid, role_regtime, &Ol_last);
    if (ret != SUCC) {
        return ret;
    }
    DEBUG_LOG("daily loop task:uid=[%u] day_time=[%u] Ol_last=[%u]",userid, day_time, Ol_last);
    if (day_time >= Ol_last) {

		
		ret = this->gf_role.set_role_Ol_last(userid, role_regtime);
		if (ret != SUCC) {
            return ret;
        }
		
        ret = this->gf_task.del_daily_loop_task(userid, role_regtime);
        if (ret != SUCC) {
            return ret;
        }
		ret = this->gf_role.del_daily_loop(userid, role_regtime);
		if (ret != SUCC) {
            return ret;
        }

		ret = this->gf_buff.daily_del_buff(userid, role_regtime);

		ret = this->gf_attire.del_timeout_attire_by_attireid(userid, role_regtime, 1190001);

		ret = this->gf_attire.del_timeout_attire_by_attireid(userid, role_regtime, 1190003);

		ret = this->gf_attire.del_timeout_attire_by_attireid(userid, role_regtime, 1190004);
/*
#ifndef VERSION_TAIWAN
        uint32_t coupons_cnt = 0;
        ret = this->gf_item.get_item_cnt_by_itemid(userid, role_regtime, &coupons_cnt, 1700061);
        if (coupons_cnt == 1) {
            time_t mail_up_tm = day_time + 7 * 3600 * 24;
            struct tm *p_mail_up_tm = localtime( &mail_up_tm );
            int up_year  = p_mail_up_tm->tm_year;
            int up_month = p_mail_up_tm->tm_mon;
            int up_day   = p_mail_up_tm->tm_mday;
            time_t mail_low_tm = Ol_last + 7 * 3600 * 24;
            struct tm *p_mail_low_tm = localtime( &mail_low_tm );
            int low_year  = p_mail_low_tm->tm_year;
            int low_month = p_mail_low_tm->tm_mon;
            int low_day   = p_mail_low_tm->tm_mday;
            DEBUG_LOG("COUPONS up: %d-%d-%d low: %d-%d-%d",up_year,up_month,up_day,low_year,low_month,low_day);
            if ( ((cur_month < up_month) && (cur_month >= low_month)) ||
                ((cur_year < up_year) && (low_year <= cur_year)) ) {
                this->gf_mail.insert_sys_nor_mail(userid, role_regtime, 1010, "", "");
            }
        }
#endif
*/
    }
	return SUCC;
}


int Croute_func::gf_user_login(DEAL_FUN_ARG)
{
	STD_RETURN(SUCC);
}

int Croute_func::del_role_phy(userid_t userid, uint32_t role_regtime)
{
	//1   role	1
	int ret1 = this->gf_role.set_role_delflg(userid, role_regtime, 3);

	//1 attire 2
	ret1 = this->gf_attire.clear_role_attire(userid, role_regtime);


	//1 item 3
	uint32_t item_cnt = 0;
	gf_get_user_item_list_out_item* p_list = 0;
	ret1 = this->gf_item.get_user_item_list(userid, role_regtime, &item_cnt, &p_list);
	gf_get_user_item_list_out_item* p_tmp = p_list;
	if (item_cnt && p_list) {
		for (uint32_t i = 0; i < item_cnt; i++) {
			gf_item_change_log.add(time(NULL), item_change_log_opt_type_del ,userid, p_tmp->itemid, p_tmp->count);
			p_tmp++;
		}
		free (p_list);
		p_list = 0;
	}
	ret1 = this->gf_item.clear_role_item(userid, role_regtime);


	//1 skill 4
	ret1 = this->gf_skill.clear_role_skill(userid, role_regtime);


	//1 task 5
	ret1 = this->gf_task.clear_role_task(userid, role_regtime);


	//1 killboss 6
	ret1 = this->gf_killboss.clear_role_killboss(userid, role_regtime);


	//1 summon7
	ret1 = this->gf_summon.clear_role_summon(userid, role_regtime);

	//1 seconday pro8
	ret1 = this->gf_secondary_pro.clear_player_secondary_pro(userid, role_regtime);

	//1 achievement9
	ret1 = this->gf_achieve.clear_role_achievement(userid, role_regtime);

	//1 buff10
	ret1 = this->gf_buff.clear_role_buff(userid, role_regtime);

	gf_mail.clear_role_info(userid, role_regtime);

	uint32_t material_cnt = 0;
	gf_get_strengthen_material_list_out_item* p_list2 = 0;
	ret1 = this->gf_material.get_user_material_list(userid, role_regtime, &material_cnt, &p_list2);
	gf_get_strengthen_material_list_out_item* p_tmp2 = p_list2;
	if (material_cnt && p_list2) {
		for (uint32_t i = 0; i < material_cnt; i++) {
			gf_item_change_log.add(time(NULL), item_change_log_opt_type_del ,userid, p_tmp2->material_id, p_tmp2->cnt);
			p_tmp2++;
		}
		free (p_list2);
		p_list2 = 0;
	}
	
	gf_material.clear_role_info(userid, role_regtime);
	gf_shop_log.clear_role_info(userid, role_regtime);
	gf_buy_item_limit.clear_role_info(userid, role_regtime);
	gf_kill_boss.clear_role_info(userid, role_regtime);
	gf_title.clear_role_info(userid, role_regtime);
	gf_ring_task.clear_role_info(userid, role_regtime);
	gf_ring_task_history.clear_role_info(userid, role_regtime);
	gf_swap_action.clear_role_info(userid, role_regtime);
	gf_plant.clear_role_info(userid, role_regtime);
	gf_home.clear_role_info(userid, role_regtime);
	gf_home_log.clear_role_info(userid, role_regtime);
	gf_card.clear_role_info(userid, role_regtime);
	gf_summon_skill.clear_role_info(userid, role_regtime);
	gf_other_info.clear_role_info(userid, role_regtime);


	DEBUG_LOG("gf_del_role_cmd:uid=[%u],role_tm=[%d]",
			userid, role_regtime);
	return SUCC;

}


// GF_OTHER   DB


int Croute_func::gf_check_user_invited(DEAL_FUN_ARG)
{
	uint32_t invited = 0;
	ret = this->gf_invite_code.check_user_invited(RECVBUF_USERID, &invited);
	if (ret != SUCC)
	{
		return ret;
	}
	DEBUG_LOG("uid:%u check user invited : %u", RECVBUF_USERID, invited);
	if (!invited)
	{
		STD_RETURN(GF_USER_NOT_INVITED);
	}
	STD_RETURN(ret);
}

int Croute_func::gf_check_invite_code(DEAL_FUN_ARG)
{
	gf_check_invite_code_in* p_in = PRI_IN_POS;
	uint32_t usedflg = 0;
	ret = this->gf_invite_code.check_invite_code(p_in->invite_code, &usedflg);
	if (ret != SUCC)
	{
		return GF_INVALID_INVITE_CODE;
	}
	if (usedflg)
	{
		return GF_INVITE_CODE_USED;
	}
	ret = this->gf_invite_code.update_invite_code(RECVBUF_USERID, p_in->invite_code);
	DEBUG_LOG("uid:%u check invite code:%s ", RECVBUF_USERID, p_in->invite_code);
	STD_RETURN(ret);
}

int Croute_func::gf_get_invit_code(DEAL_FUN_ARG)
{
	gf_get_invit_code_in* p_in = PRI_IN_POS;
	gf_get_invit_code_out_header out_header = {0};

	gf_get_invit_code_out_item *p_out_item = NULL;
	ret = this->gf_invite_code.get_unused_code(p_in->num, &(out_header.cnt), 
		&p_out_item);
	DEBUG_LOG("get_invit_code num:%u", out_header.cnt);
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.cnt);

}
/** 
 * @fn  拉取角色日常任务
 * @brief  cmd route interface
 */
int Croute_func::gf_get_daily_action(DEAL_FUN_ARG)
{
	char out[PROTO_MAX_SIZE] = {0};
	int cpy_len = 0;
    uint32_t dcount = 0;
    uint32_t wcount = 0;
    uint32_t mcount = 0;
    gf_get_daily_action_out_header* p_out = (gf_get_daily_action_out_header*)out;
    gf_get_daily_action_out_item* p_item = NULL;

	char* p_offset = &(out[0]) + sizeof(*p_out);
    
    ret = this->gf_daction.get_action_type_list(USERID_ROLETM, &dcount, &p_item);
    if (ret != SUCC) {
        return ret;
    }
    if (p_item)	{
		cpy_len = sizeof(*p_item) * dcount;
		memcpy(p_offset, p_item, cpy_len);
		free(p_item);
        p_item = NULL;
		p_offset += cpy_len;
	}

    ret = this->gf_waction.get_action_type_list(USERID_ROLETM, &wcount, &p_item);
    if (ret != SUCC) {
        return ret;
    }
    if (p_item) {
		cpy_len = sizeof(*p_item) * wcount;
		memcpy(p_offset, p_item, cpy_len);
		free(p_item);
        p_item = NULL;
		p_offset += cpy_len;
	}

    ret = this->gf_maction.get_action_type_list(USERID_ROLETM, &mcount, &p_item);
    if (ret != SUCC) {
        return ret;
    }
    if (p_item) {
		cpy_len = sizeof(*p_item) * mcount;
		memcpy(p_offset, p_item, cpy_len);
		free(p_item);
        p_item = NULL;
		p_offset += cpy_len;
	}
    p_out->count = dcount + wcount + mcount; 

	uint32_t role_type = 0;
	this->gf_hunter_top.get_role_type(USERID_ROLETM, &role_type);
	p_out->fumo_old_place = this->gf_hunter_top.get_hunter_old_place(USERID_ROLETM, role_type);
	
    DEBUG_LOG("get user info:uid=[%u], count=[%u], len=[%ld]",
			RECVBUF_USERID,  p_out->count, p_offset-&(out[0]));
	STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0]));
}

/** 
 * @fn  设置角色日常任务
 * @brief  cmd route interface
 */
int Croute_func::gf_set_daily_action(DEAL_FUN_ARG)
{
	gf_set_daily_action_in* p_in = PRI_IN_POS;
	gf_set_daily_action_out p_out = {0};

    if (p_in->flag == 0) {
        ret = this->gf_daction.increase_action_count(USERID_ROLETM, p_in->real_type, p_in->toplimit);
        if (ret != SUCC) {
            return ret; //out of limit
        }
    } else if (p_in->flag == 1) {
        ret = this->gf_waction.increase_action_count(USERID_ROLETM, p_in->real_type, p_in->toplimit);
        if (ret != SUCC) {
            return ret;
        }
    } else if (p_in->flag == 2) {
        ret = this->gf_maction.increase_action_count(USERID_ROLETM, p_in->real_type, p_in->toplimit);
        if (ret != SUCC) {
            return ret;
        }
    } else {
        return GF_DAILY_ACTION_TYPE_FLAG_ERR;
    }
    p_out.type = p_in->real_type;
    STD_RETURN_WITH_STRUCT(ret, p_out);
}

//bengin summon
/**
 * @fu 拉取召唤兽信息
 * @
 */
int Croute_func::gf_get_summon_list(DEAL_FUN_ARG)
{
    char out[PROTO_MAX_SIZE] = {0};
	int cpy_len = 0;
    gf_get_summon_list_out_header* p_out = (gf_get_summon_list_out_header*)out;
    gf_get_summon_list_out_item* p_item = NULL;

	char* p_offset = &(out[0]) + sizeof(*p_out);

	uint32_t summon_off_tm = 0;
	ret = gf_role.get_int_value(USERID_ROLETM, "summon_off_tm", &summon_off_tm);
	if (ret != SUCC) {
        return ret;
    }
	
	if (!summon_off_tm) {
		gf_summon.reset_off_summon(USERID_ROLETM);
	}
    ret = this->gf_summon.get_summon_list(USERID_ROLETM, &(p_out->count), &p_item);
    if (ret != SUCC) {
        return ret;
    }

    cpy_len = sizeof(gf_get_summon_list_out_item);
    gf_get_summon_list_out_item * summon_item = p_item;
    for (uint32_t i = 0; i < p_out->count; i++) {
        sum_skill_t * skill_list = NULL;
        uint32_t cnt = 0;
        //DEBUG_LOG("XXXXX [%u %u]", summon_item[i].mon_tm, summon_item[i].mon_type);
        gf_summon_skill.get_summon_skill_list(USERID_ROLETM, summon_item[i].mon_tm, &(cnt), &(skill_list));
        
        summon_item[i].skill_cnt = cnt;
        //DEBUG_LOG("XXXXX---------- [%u]", summon_item[i].skill_cnt);
        if (skill_list) {
            for (uint32_t j = 0; j < summon_item[i].skill_cnt && j < max_summon_skills; j++) {
                //DEBUG_LOG("xxxxx skill [%u %u]", skill_list[j].skillid, skill_list[j].skilllv);
                summon_item[i].skill_data[j].skillid = skill_list[j].skillid;
                summon_item[i].skill_data[j].skilllv = skill_list[j].skilllv;
            }

            free(skill_list);
            skill_list = NULL;
        }

        memcpy(p_offset, &(summon_item[i]), cpy_len);
        p_offset += cpy_len;
    }

    if (p_item) {
        free(p_item);
        p_item = NULL;
    }
/*
    if (p_item)	{
        summon_skill_list skill_list = {0};
        gf_summon_skill.get_summon_skill_list(USERID_ROLETM, p_item->mon_tm, cnt, );
		cpy_len = sizeof(*p_item) * (p_out->count);
		memcpy(p_offset, p_item, cpy_len);
		free(p_item);
        p_item = NULL;
		p_offset += cpy_len;
	}*/
    DEBUG_LOG("get summon list:uid=[%u], count=[%u], len=[%ld]",
			RECVBUF_USERID,  p_out->count, p_offset-&(out[0]));
	STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0]));
}


/**
 * @fu 拉取召唤兽信息
 * @
 */
int Croute_func::gf_get_last_summon_nick(DEAL_FUN_ARG)
{
    gf_get_last_summon_nick_out out = { {0} };

    ret = this->gf_summon.get_last_summon_nick(USERID_ROLETM, out.nick);
    STD_RETURN_WITH_STRUCT(ret, out); 
}

/**
 * @fn 孵化召唤兽
 * @
 */
int Croute_func::gf_hatch_summon(DEAL_FUN_ARG)
{
    gf_hatch_summon_in* p_in = PRI_IN_POS;
    gf_hatch_summon_out p_out = {0};

    ret = this->gf_item.reduce_item_int_value(USERID_ROLETM, p_in->item_id, "count", 1);
    if (ret != SUCC) {
		STD_ROLLBACK();
        return ret;
    }
    time_t ctm = time(NULL);
    ret = this->gf_summon.add_summon(USERID_ROLETM, ctm, p_in->mon_type, p_in->status, p_in->mon_nick);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }

	if (p_in->pre_summon_tm) {
		ret = this->gf_summon.del_summon(USERID_ROLETM, p_in->pre_summon_tm);
		if (ret != SUCC) {
	        STD_ROLLBACK();
	        return ret;
	    }
	}
	p_out.pre_summon_tm = p_in->pre_summon_tm;
    p_out.mon_tm   = ctm;
    p_out.mon_type = p_in->mon_type;
    p_out.item_id  = p_in->item_id;
    p_out.lv  = 1;
    p_out.fight_value  = 100;
    memcpy(p_out.mon_nick, p_in->mon_nick, sizeof(p_out.mon_nick));
    p_out.status = p_in->status;
    STD_RETURN_WITH_STRUCT(ret, p_out); 
}

/**
 * @fn 喂养召唤兽
 * @
 */
int Croute_func::gf_feed_summon(DEAL_FUN_ARG)
{
    gf_feed_summon_in* p_in = PRI_IN_POS;
    gf_feed_summon_out p_out = {0};

    ret = this->gf_item.reduce_item_int_value(USERID_ROLETM, p_in->item_id, "count", 1);
    if (ret != SUCC) {
	STD_ROLLBACK();
        return ret;
    }
    ret = this->gf_summon.increase_int_value(USERID_ROLETM,p_in->mon_tm,"fight_value",p_in->add_val,p_in->max_val, &(p_out.total_val));
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
    p_out.mon_tm = p_in->mon_tm;
	p_out.item_id  = p_in->item_id;
    STD_RETURN_WITH_STRUCT(ret, p_out); 
}

/**
 * @fn 设置召唤兽的昵称
 * @
 */
int Croute_func::gf_set_summon_nick(DEAL_FUN_ARG)
{
    gf_set_summon_nick_in* p_in = PRI_IN_POS;
    gf_set_summon_nick_out p_out = {0};

    ret = this->gf_summon.update_summon_nick(USERID_ROLETM, p_in->mon_tm, p_in->nick);
    if (ret != SUCC) {
        return ret;
    }
    p_out.mon_tm  = p_in->mon_tm;
    STD_RETURN_WITH_STRUCT(ret, p_out); 
}

/**
 * @fn 交换出战的召唤兽
 * @
 */
int Croute_func::gf_change_summon_fight(DEAL_FUN_ARG)
{
    gf_change_summon_fight_in* p_in = PRI_IN_POS;

	if(p_in->flag == 1 || p_in->flag == 2)
	{
    	ret = this->gf_summon.callback_fight_summon(USERID_ROLETM);
    	if (ret != SUCC) {
        	return ret;
    	}
	}
    ret = this->gf_summon.set_summon_status(USERID_ROLETM, p_in->mon_tm, p_in->flag);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
    //p_out.mon_tm  = p_in->mon_tm;
	//p_out.flag    = p_in->flag;
    //STD_RETURN_WITH_STRUCT(ret, p_out); 
	return 0;
}

/**
 * @fn 设置召唤兽的基本属性
 * @
 */
int Croute_func::gf_set_summon_property(DEAL_FUN_ARG)
{
    gf_set_summon_property_in* p_in = PRI_IN_POS;

    ret = this->gf_summon.set_summon_property(USERID_ROLETM, p_in);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
    STD_RETURN(SUCC);
}

/**
 * @fn 使用灵兽技能卷轴
 * @
 */
int Croute_func::gf_use_summon_skills_scroll(DEAL_FUN_ARG)
{
    gf_use_summon_skills_scroll_in* p_in = PRI_IN_POS;
    gf_use_summon_skills_scroll_out out = {0};

    ret = gf_reduce_item_count(USERID_ROLETM, p_in->id, 1);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }

    if (p_in->flag == 1) {
        ret = this->gf_summon_skill.add_summon_skill(USERID_ROLETM, 
                                                     p_in->mon_tm, 
                                                     p_in->skill_info.skillid, 
                                                     p_in->skill_info.skilllv);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }

    out.mon_tm    =    p_in->mon_tm;
    out.id        =        p_in->id;
    out.flag      =      p_in->flag;
    out.skill_info.skillid = p_in->skill_info.skillid;
    out.skill_info.skilllv = p_in->skill_info.skilllv;
    STD_RETURN_WITH_STRUCT(ret, out);
}

/**
 * @fn 设置召唤兽的技能
 * @
 */
int Croute_func::gf_set_summon_skills(DEAL_FUN_ARG)
{
    gf_set_summon_skills_in* p_in = PRI_IN_POS;
    gf_set_summon_skills_out out = {0};

    ret = this->gf_summon_skill.update_summon_skill(USERID_ROLETM, 
                                                    p_in->mon_tm, 
                                                    p_in->src_id,
                                                    p_in->skill_info.skillid, 
                                                    p_in->skill_info.skilllv);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }

    out.mon_tm  = p_in->mon_tm;
    out.src_id  = p_in->src_id;
    out.skill_info.skillid = p_in->skill_info.skillid;
    out.skill_info.skilllv = p_in->skill_info.skilllv;

    STD_RETURN_WITH_STRUCT(ret, out);
}

/**
 * @fn 设置召唤兽的基本属性
 * @
 */
int Croute_func::gf_set_summon_mutate(DEAL_FUN_ARG)
{
    gf_set_summon_mutate_in* p_in = PRI_IN_POS;
    gf_set_summon_mutate_out out = {0};

	if (p_in->evolve_need_item) {
		ret = this->gf_item.reduce_item_int_value(USERID_ROLETM, p_in->evolve_need_item, item_count_column,1);
		if (ret != SUCC) {
		STD_ROLLBACK();
	        return ret;
	    }
	}

    uint32_t level = 0;
    ret = this->gf_summon.get_int_value(USERID_ROLETM, p_in->mon_tm, "lv", &level);
    if (ret != SUCC) {
	STD_ROLLBACK();
        return ret;
    }
    if (level != p_in->lv) {
	STD_ROLLBACK();
        return GF_SUMMON_NOFIND_ERR;
    }

    ret = this->gf_summon.set_int_value(USERID_ROLETM, p_in->mon_tm, "mon_type", p_in->mon_type);
    if (ret != SUCC) {
	STD_ROLLBACK();
        return ret;
    }
    out.new_type = p_in->mon_type;
    STD_RETURN_WITH_STRUCT(ret, out);
}
/**
 * @fn 恢复召唤兽超灵状态
 */
int Croute_func::gf_set_summon_type(DEAL_FUN_ARG)
{
    gf_set_summon_type_in_header* p_in = PRI_IN_POS;
    CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->count * sizeof(gf_set_summon_type_in_item));
    gf_set_summon_type_in_item* p_item = (gf_set_summon_type_in_item*)(p_in + 1);

    for (uint32_t i = 0; i < p_in->count; i++) {
        ret = this->gf_summon.set_int_value(USERID_ROLETM, p_item->mon_tm, "mon_type", p_item->mon_type);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }
    STD_RETURN_WITH_BUF(ret, p_in, sizeof(*p_in) + p_in->count * sizeof(*p_item));
}
/**
 *
 */
int Croute_func::gf_allocate_exp2summon(DEAL_FUN_ARG)
{
    gf_allocate_exp2summon_in* p_in = PRI_IN_POS;
    gf_allocate_exp2summon_in out = {0};
    uint32_t left_exp = 0;
    ret = this->gf_role.reduce_int_value(USERID_ROLETM,GF_ALLOTER_EXP_NOENOUGH_ERR, ROLE_ALLOCATOR_EXP_COLUMN,p_in->alloter_exp,&left_exp);
    if (ret != SUCC) {
        return ret;
    }
    //ret = this->gf_summon.set_summon_exp_lv(USERID_ROLETM,p_in->mon_tm,p_in->mon_exp, p_in->lv);
    ret = this->gf_summon.set_int_value(USERID_ROLETM, p_in->mon_tm, "exp", p_in->mon_exp);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
    ret = this->gf_summon.set_int_value(USERID_ROLETM, p_in->mon_tm, "lv", p_in->lv);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }

    /*
    ret = this->gf_summon.get_int_value(USERID_ROLETM,p_in->mon_tm,"exp",&(out.mon_exp));
    if (ret != SUCC){
        STD_ROLLBACK();
        return ret;
    }*/
    out.alloter_exp = left_exp;
    out.mon_tm = p_in->mon_tm;
    out.mon_exp = p_in->mon_exp;
    out.lv = p_in->lv;
    DEBUG_LOG("Allocate summon exp %u %u %u %u", out.alloter_exp, out.mon_tm, out.mon_exp, out.lv);
    STD_RETURN_WITH_STRUCT(ret, out);
}
//end summon
//---- Begin Numen
/**
 * @fu 拉取守护神信息
 */
int Croute_func::gf_get_numen_list(DEAL_FUN_ARG)
{
    char out[PROTO_MAX_SIZE] = {0};
	int cpy_len = 0;
    gf_get_numen_list_out_header* p_out = (gf_get_numen_list_out_header*)out;
    gf_get_numen_list_out_item* p_item = NULL;

	char* p_offset = &(out[0]) + sizeof(*p_out);

    ret = this->gf_numen.get_numen_list(USERID_ROLETM, &(p_out->numen_cnt), &p_item);
    if (ret != SUCC) {
        return ret;
    }

    cpy_len = sizeof(gf_get_numen_list_out_item);
    gf_get_numen_list_out_item * numen_item = p_item;
    for (uint32_t i = 0; i < p_out->numen_cnt; i++) {
        numen_skill_t * skill_list = NULL;
        uint32_t cnt = 0;
        //DEBUG_LOG("XXXXX [%u]", numen_item[i].numen_type);
        gf_numen_skill.get_numen_skill_list(USERID_ROLETM, numen_item[i].numen_type, &(cnt), &(skill_list));
        
        numen_item[i].skill_cnt = cnt;

        memcpy(p_offset, &(numen_item[i]), cpy_len);
        p_offset += cpy_len;

        //DEBUG_LOG("XXXXX---------- [%u]", numen_item[i].skill_cnt);
        if (skill_list) {
            /*
            for (uint32_t j = 0; j < numen_item[i].skill_cnt; j++) {
                DEBUG_LOG("xxxxx skill [%u %u]", skill_list[j].id, skill_list[j].lv);
                //numen_item[i].skills[j].id = skill_list[j].id;
                //numen_item[i].skills[j].lv = skill_list[j].lv;
            }*/

            memcpy(p_offset, skill_list, cnt * sizeof(numen_skill_t));
            p_offset += (cnt * sizeof(numen_skill_t));
            //DEBUG_LOG("XXXXX---skill len------- [%u]", cnt * sizeof(numen_skill_t));

            free(skill_list);
            skill_list = NULL;
        }
    }

    if (p_item) {
        free(p_item);
        p_item = NULL;
    }

    DEBUG_LOG("get numen list:uid=[%u], count=[%u], len=[%ld]",
			RECVBUF_USERID,  p_out->numen_cnt, p_offset-&(out[0]));
	STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0]));
}

/**
 * @fu 召唤守护神
 */
int Croute_func::gf_invite_numen(DEAL_FUN_ARG)
{
    gf_invite_numen_in_header* p_in = PRI_IN_POS;
    gf_invite_numen_in_item* p_in_item = (gf_invite_numen_in_item *)(p_in + 1);

    CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->skill_cnt * sizeof(gf_invite_numen_in_item));

    ret = this->gf_reduce_item_count(USERID_ROLETM, p_in->call_item_id, 1);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }

    ret = gf_numen.add_numen(USERID_ROLETM, p_in->numen_type, p_in->name);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }

    for (uint32_t i = 0; i < p_in->skill_cnt; i++) {
        ret = gf_numen_skill.add_numen_skill(USERID_ROLETM, p_in->numen_type, p_in_item->skill_id, p_in_item->skill_lv);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }
    gf_invite_numen_out out = {0};
    out.call_item_id    =   p_in->call_item_id;
    out.numen_type      =   p_in->numen_type;

    STD_RETURN_WITH_STRUCT(SUCC, out);
}

/**
 * @fu 改变守护神状态
 */
int Croute_func::gf_change_numen_status(DEAL_FUN_ARG)
{
    gf_change_numen_status_in* p_in = PRI_IN_POS; 

    //DEBUG_LOG("numen --> [%u %u]", p_in->numen_type, p_in->status);
    ret = gf_numen.set_role_numen_status(USERID_ROLETM, 0);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }

    ret = this->gf_numen.set_numen_status(USERID_ROLETM, p_in->numen_type, p_in->status);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }

    STD_RETURN(SUCC);
}

/**
 * @fu 
 */
int Croute_func::gf_make_numen_sex(DEAL_FUN_ARG)
{
    gf_make_numen_sex_in* p_in = PRI_IN_POS;

    if (p_in->sex_item_id != 0) {
        ret = this->gf_reduce_item_count(USERID_ROLETM, p_in->sex_item_id, 1);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }

    if (p_in->action == 2 || p_in->action == 3) {
        ret = gf_numen_skill.add_numen_skill(USERID_ROLETM, 
            p_in->numen_type, p_in->new_id, p_in->new_lv);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    } else if (p_in->action == 5) {
        ret = gf_numen_skill.update_numen_skill(USERID_ROLETM, 
            p_in->numen_type, p_in->old_id, p_in->new_id, p_in->new_lv);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }
    STD_RETURN_WITH_BUF(ret, p_in, sizeof(gf_make_numen_sex_in));
}

/**
 * @fu 改变守护神昵称
 */
int Croute_func::gf_change_numen_nick(DEAL_FUN_ARG)
{
    gf_change_numen_nick_in* p_in = PRI_IN_POS; 

    ret = gf_numen.update_numen_nick(USERID_ROLETM, p_in->numen_type, p_in->name);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }

    STD_RETURN_WITH_BUF(ret, p_in, sizeof(gf_change_numen_nick_in));
}


//---- End Numen
#if 0
int Croute_func::gf_use_invite_code(DEAL_FUN_ARG)
{
	gf_use_invite_code_in* p_in = PRI_IN_POS;
	ret = this->gf_invite_code.use_invite_code(p_in->invite_code);
	if (ret != SUCC)
	{
		return ret;
	}
	DEBUG_LOG("uid:%u  use invite code:%s ", RECVBUF_USERID, p_in->invite_code);
	STD_RETURN(SUCC);
}
#endif

int Croute_func::gf_set_child_count(DEAL_FUN_ARG)
{
    gf_set_child_count_in* p_in = PRI_IN_POS;
    gf_set_child_count_out out = {0};
  //  ret = this->gongfu.update_user_count(RECVBUF_USERID, "child_cnt");
  //  if (ret != SUCC) {
  //      return ret;
  //  }
	gf_master_add_apprentice_in tmp = { 0 };
  	tmp.uid = p_in->childid;
    tmp.roletm = p_in->chlid_role_tm;
    tmp.prentice_lv = 1;
	time_t now = time (NULL);
    tmp.tm = (uint32_t)now;
  	this->gf_master.add_apprentice(USERID_ROLETM, &tmp);
    this->gf_friend.add(RECVBUF_USERID, p_in->childid, 0, TYPE_FRIEND);
   // ret = this->gf_role.get_user_max_level(RECVBUF_USERID, &(out.lv));
   // if (ret != SUCC) {
   //    STD_ROLLBACK();
   //    return ret;
   //}
	STD_RETURN_WITH_STRUCT(SUCC, out);
}

int Croute_func::gf_set_achieve_count(DEAL_FUN_ARG)
{
    ret = this->gongfu.update_user_count(RECVBUF_USERID, "achieve_cnt");
    if (ret != SUCC) {
        return ret;
    }
    /*
    uint32_t achieve_cnt = 0;
    ret = this->gongfu.get_amb_achieve_count(RECVBUF_USERID, &achieve_cnt);
    if (ret != SUCC) {
        return ret;
    }
    if (achieve_cnt >= 30) {
        return this->gongfu.update_amb_status(RECVBUF_USERID, 2);
    }
    */
	STD_RETURN(SUCC);
}

int Croute_func::gf_set_amb_status(DEAL_FUN_ARG)
{
    gf_set_amb_status_in* p_in = PRI_IN_POS;
    gf_set_amb_status_out out = {0};
    ret = this->gongfu.update_amb_status(RECVBUF_USERID, p_in->status);
    if (ret != SUCC) {
        return ret;
    }
    out.status = p_in->status;
    STD_RETURN_WITH_STRUCT(ret, out); 
}

int Croute_func::gf_set_amb_reward_flag(DEAL_FUN_ARG)
{
    gf_set_amb_reward_flag_in* p_in = PRI_IN_POS;
    ret = this->gongfu.update_amb_reward_flag(RECVBUF_USERID, p_in->buf);
    if (ret != SUCC) {
        return ret;
    }
	STD_RETURN(SUCC);
}
int Croute_func::gf_set_account_forbid(DEAL_FUN_ARG)
{
    gf_set_account_forbid_in* p_in = PRI_IN_POS;
	ret = this->gongfu.add_user_for_boss(RECVBUF_USERID);
	if (ret != SUCC) {
		STD_ROLLBACK();
        return ret;
    }
    uint32_t limit_tm = 0;
    if (p_in->deadline != 0xffffffff) {
        //limit_tm = time(NULL) + (p_in->deadline) * 3600 * 24;
        limit_tm = (p_in->deadline) / 7 + 1;
    } else {
        //limit_tm = p_in->deadline;
        limit_tm = 4;
    }
    DEBUG_LOG("forbid flag=[%u] dur=[%x] time=[%x]",p_in->forbid_flag,p_in->deadline,limit_tm);
    ret = this->gongfu.update_account_forbid(RECVBUF_USERID, p_in->forbid_flag, limit_tm);
    if (ret != SUCC) {
        return ret;
    }
    STD_RETURN(SUCC);
}
//vip start
int Croute_func::gf_get_user_vip(DEAL_FUN_ARG)
{
    gf_get_user_vip_out out = {0};
    ret = this->gongfu.get_vip_info(RECVBUF_USERID, &out);
    if (ret != SUCC) {
        return ret;
    }
    if (out.vip_is != 0) {
        uint32_t tail_time = time(NULL);
        if ( out.vip_is == 2 ) {
            tail_time = out.end_tm;
        }
        //vip_config_data_mgr::getInstance()->init();
        out.vip_lv = calc_player_vip_level(&vip_obj, out.start_tm, tail_time, out.x_value, out.vip_is);
        DEBUG_LOG("calc_player_vip_level: vip_lv=%u",out.vip_lv);
        //vip_config_data_mgr::getInstance()->final();
    }

    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_set_vip(DEAL_FUN_ARG)
{
    gf_set_vip_in* p_in = PRI_IN_POS;
    DEBUG_LOG("VIP CHECK 1 ----> [%u] [%u %u %u %u]", RECVBUF_USERID, p_in->vip_is, p_in->vip_type, p_in->vip_month_cnt, p_in->start_tm);
	ret = this->gongfu.add_user_for_boss(RECVBUF_USERID);
	if (ret != SUCC) {
		STD_ROLLBACK();
        return ret;
    }
    if (p_in->vip_is == 4) {
        ret = this->gongfu.set_int_value(RECVBUF_USERID, 0, "auto_incr");
        if (ret != SUCC) {
            return ret;
        }
    } else {

        uint32_t cur_month = 0;
        this->gongfu.get_int_value(RECVBUF_USERID, "vip_month_count", &cur_month);
        if ( test_bit_on(&(p_in->vip_type), 1, 2) ) {
            DEBUG_LOG("SET YEAR VIP FLAG\t uid=[%u] vip_type=[%u]",RECVBUF_USERID, p_in->vip_type);
            p_in->vip_is = VIP_YEAR_USER_FLAG;
            this->gongfu.set_int_add_value(RECVBUF_USERID, 100, "x_value");
        }
        ret = this->gongfu.set_vip(RECVBUF_USERID, p_in);
        if (ret != SUCC) {
            return ret;
        }
        /// ------------- (乐翻翻) vip充值活动下架需要注释 ---------------------
        if (p_in->vip_is != 0) {
            uint8_t buf[40] = {0};
            this->gongfu.get_act_record(RECVBUF_USERID, buf);
            DEBUG_LOG("set vip: [%u %u]", cur_month, p_in->vip_month_cnt);
            if (p_in->vip_month_cnt > cur_month ) {
                buf[8] += p_in->vip_month_cnt - cur_month;
				this->gongfu.set_act_record(RECVBUF_USERID, buf);
            }
        }
    }
    STD_RETURN(SUCC);
}

int Croute_func::gf_set_base_svalue(DEAL_FUN_ARG)
{
    gf_set_base_svalue_in* p_in = PRI_IN_POS;
    uint32_t vip_type = 0;
    this->gongfu.get_int_value(RECVBUF_USERID, "vip", &vip_type);

    if (vip_type == VIP_YEAR_USER_FLAG) {
        p_in->x_val += 100;
    }
    DEBUG_LOG("VIP CHECK 2 ---->[%u] [%u, %u]", RECVBUF_USERID, vip_type, p_in->x_val);
    ret = this->gongfu.set_int_value(RECVBUF_USERID, p_in->x_val, "x_value");
    if (ret != SUCC) {
        return ret;
    }
    STD_RETURN(SUCC);
}

int Croute_func::gf_set_gold(DEAL_FUN_ARG)
{
    gf_set_gold_in* p_in = PRI_IN_POS;

	ret = this->gongfu.add_user_for_boss(RECVBUF_USERID);
	if (ret != SUCC) {
		STD_ROLLBACK();
        return ret;
    }
    ret = this->gongfu.set_int_add_value(RECVBUF_USERID, p_in->gold, "gold_coin");
    if (ret != SUCC) {
        return ret;
    }
    STD_RETURN(SUCC);
}

int Croute_func::gf_set_vip_exinfo(DEAL_FUN_ARG)
{
    gf_set_vip_exinfo_in* p_in = PRI_IN_POS;
	ret = this->gongfu.add_user_for_boss(RECVBUF_USERID);
	if (ret != SUCC) {
		STD_ROLLBACK();
        return ret;
    }
    ret = this->gongfu.set_vip_exvalue(RECVBUF_USERID, p_in);
    STD_RETURN(SUCC);
}

int Croute_func::other_gf_sync_vip(DEAL_FUN_ARG)
{  
    other_gf_sync_vip_in *p_in=PRI_IN_POS;
    uint32_t vipflag = 0;
    if (p_in->vip_is != 0)
        vipflag = 3;
    if ( test_bit_on(&(p_in->vip_type), 1, 2) ) {
        vipflag = VIP_YEAR_USER_FLAG;
    }
    ret=this->send_email.gf_sync_vip(RECVBUF_USERID, vipflag,
        p_in->vip_month_cnt, p_in->auto_incr, p_in->end_tm,p_in->start_tm, p_in->vip_type);    
    STD_RETURN(ret);
}

int Croute_func::other_gf_sync_base_svalue(DEAL_FUN_ARG)
{   
    other_gf_sync_base_svalue_in *p_in=PRI_IN_POS;
    //uint32_t vip_type = 0;
    //this->gongfu.get_int_value(RECVBUF_USERID, "vip", &vip_type);
    /*
    if (vip_type == VIP_YEAR_USER_FLAG) {
        p_in->x_val += 100;
    }*/
    ret=this->send_email.gf_sync_base_svalue(RECVBUF_USERID, p_in->x_val, p_in->chn); 
    STD_RETURN(ret);
}

int Croute_func::other_gf_sync_gold(DEAL_FUN_ARG)
{
    other_gf_sync_gold_in *p_in=PRI_IN_POS;
    ret=this->send_email.gf_sync_set_gold(RECVBUF_USERID, p_in->gold);   
    STD_RETURN(ret);
}
/** 
 * @fn vip发送系统消息
 * @brief  cmd route interface
 */
int Croute_func::other_gf_vip_sys_msg(DEAL_FUN_ARG)
{
	other_gf_vip_sys_msg_in_header *p_in= PRI_IN_POS;
    DEBUG_LOG("vip system message:len=[%u %ld]",p_in->msglen, strlen(p_in->msg));
    char msg[OFFLINE_MSG_MAX_LEN] = {0};
    CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->msglen);
    memcpy(msg, p_in->msg, p_in->msglen);

    this->send_email.gf_vip_sys_msg(RECVBUF_USERID, p_in->msglen, msg);
	STD_RETURN(SUCC);
}
//end vip
/**
 * @fn 武林密码
 */
int Croute_func::gf_set_cryptogram_gift(DEAL_FUN_ARG)
{
    gf_set_cryptogram_gift_in *p_in = PRI_IN_POS;
    gf_set_cryptogram_gift_out out = {0};

    switch (p_in->itemtype)
    {
        case TYPE_ATTIRE:
            m_attire_elem.attireid = p_in->itemid;
            m_attire_elem.gettime = time(NULL);
            m_attire_elem.attire_rank = 0;
            m_attire_elem.duration = 20*100;
            m_attire_elem.endtime = 0;
            m_attire_elem.usedflag = 0;
            ret = this->gf_attire.add_attire(USERID_ROLETM, &m_attire_elem, p_in->max_bag);
            if (ret != SUCC) {
                STD_ROLLBACK();
                return ret;			
            }
            ret = this->gf_attire.get_max_id(USERID_ROLETM, &(out.uniqueid));
            if (ret != SUCC) { 
                STD_ROLLBACK();
                return ret;
            }

            break;
        case TYPE_ITEM:
            ret = gf_increase_item(USERID_ROLETM,p_in->itemid,p_in->item_cnt,set_item_limit(p_in->itemid),p_in->max_bag);
            if (ret!=SUCC) {
                STD_ROLLBACK();
                return ret;
            }
            break;
        case TYPE_ROLE:
            if (p_in->itemid == TYPE_ROLE_XIAOMEE)
                ret = this->gf_role.increase_int_value(USERID_ROLETM,
                    ROLE_XIAOMEE_COLUMN, p_in->item_cnt);
            else if (p_in->itemid == TYPE_ROLE_EXP)
                ret = this->gf_role.increase_int_value(USERID_ROLETM, 
                    ROLE_EXP_COLUMN, p_in->item_cnt);
            else if (p_in->itemid == TYPE_ROLE_SKILLPOINT)
                ret = this->gf_role.increase_int_value(USERID_ROLETM, 
                    ROLE_SKILL_POINT_COLUMN, p_in->item_cnt);
            else if (p_in->itemid == TYPE_ROLE_FUMOPOINT)
                ret = this->gf_role.increase_int_value(USERID_ROLETM, 
                    ROLE_FUMO_POINTS_COLUMN, p_in->item_cnt);
            else if (p_in->itemid == TYPE_ROLE_PVP_FIGHT)
                ret = this->gf_role.increase_int_value(USERID_ROLETM,
                    ROLE_EXPLOIT_COLUMN, p_in->item_cnt);
            else if (p_in->itemid == TYPE_ROLE_HONOR)
                ret = this->gf_role.increase_int_value(USERID_ROLETM,
                    ROLE_HONOR_COLUMN, p_in->item_cnt);
            else {
                STD_ROLLBACK();
                return ENUM_OUT_OF_RANGE_ERR;
            }
            break;
        default:
            STD_ROLLBACK();
            DEBUG_LOG("there %u ", p_in->itemtype);
            return ENUM_OUT_OF_RANGE_ERR;
            break;

    }

    out.itemid = p_in->itemid;
    out.itemtype = p_in->itemtype;
    out.item_cnt = p_in->item_cnt;

    DEBUG_LOG("Magic Gift: %u %u %u %u",out.itemid, out.uniqueid, out.itemtype, out.item_cnt);
    STD_RETURN_WITH_STRUCT(ret, out);
}
/**
 * @fn 神奇密码
 */
int Croute_func::gf_set_magic_gift(DEAL_FUN_ARG)
{
    char out[PROTO_MAX_SIZE] = {0};
    int cpy_len = 0;
    gf_set_magic_gift_in_header *p_in = PRI_IN_POS;
    gf_set_magic_gift_in_item *p_in_item = (gf_set_magic_gift_in_item*)(p_in + 1);

	CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->count * sizeof(*p_in_item));
    gf_set_magic_gift_out_header *p_out_header = (gf_set_magic_gift_out_header*)out;
    char* p_offset = &(out[0]) + sizeof(*p_out_header);
	
    uint32_t i = 0;
    gf_set_magic_gift_out_item p_out_item = {0};
    for (i = 0; i < p_in->count; i++) {
        DEBUG_LOG("magic gift id=%u type=%u cnt=%u", p_in_item->itemid, p_in_item->itemtype, p_in_item->item_cnt);
        memset(&p_out_item, 0, sizeof(gf_set_magic_gift_out_item));
        if (p_in_item->itemtype == 1) {
            struct add_attire_t attire = {0};
            attire.attireid = p_in_item->itemid;
            attire.gettime = time(NULL);
            attire.attire_rank = 1;
            //attire.duration = p_in_item->duration;
            ret = this->gf_attire.add_attire(USERID_ROLETM, &attire, p_in->max_bag);
            if (ret != SUCC) {
                STD_ROLLBACK();
                return ret;
            }
            ret = this->gf_attire.get_max_id(USERID_ROLETM, &(p_out_item.uniqueid));
            if (ret != SUCC) {
                STD_ROLLBACK();
                return ret;
            }
        } else if (p_in_item->itemtype == 2) {
            ret = gf_increase_item(USERID_ROLETM,p_in_item->itemid,p_in_item->item_cnt,
                set_item_limit(p_in_item->itemid),p_in->max_bag);
            if (ret != SUCC) {
                STD_ROLLBACK();
                return ret;
            }
        }
        p_out_item.itemid = p_in_item->itemid;
        p_out_item.item_cnt = p_in_item->item_cnt;
        cpy_len = sizeof(gf_set_magic_gift_out_item);
        //DEBUG_LOG("Magic out gift id u cnt len[%u %u %u %u]",p_out_item.itemid,p_out_item.uniqueid,p_out_item.item_cnt, cpy_len);
        memcpy(p_offset, &p_out_item, cpy_len);
        p_offset += cpy_len;
        p_in_item += 1;
    }
    p_out_header->count = i;
    DEBUG_LOG("Magic Gift in count=[%u] out count=[%u] len[%ld]",p_in->count, p_out_header->count, p_offset-&(out[0]));
    STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0]));
}
/**
 * @brief 魔术箱子
 */
int Croute_func::gf_set_magic_box(DEAL_FUN_ARG)
{
    char out[PROTO_MAX_SIZE] = {0};
    int cpy_len = 0;
    gf_set_magic_box_in_header *p_in = PRI_IN_POS;
    gf_set_magic_box_in_item *p_in_item = (gf_set_magic_box_in_item *)(p_in + 1);

    CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->box_cnt * sizeof(*p_in_item));
    //gf_set_magic_box_out_header *p_out = (gf_set_magic_box_out_header *)out;
    //char* p_offset = &(out[0]) + sizeof(*p_out);
    //gf_set_magic_box_out_item p_out_item = {0};
    DEBUG_LOG("user=[%u] box type=[%u], id=[%u] cnt=[%u]", RECVBUF_USERID, p_in->box_type, p_in->box_id, p_in->box_cnt);
    uint32_t uniqueid = 0;
    for (uint32_t i = 0; i < p_in->box_cnt; i++) {
        cpy_len = sizeof(*p_in_item);
        if (p_in_item->save_flag == 1) {
            DEBUG_LOG("save box Q type id cnt :[%u %u %u %u]", p_in_item->box_quality, p_in_item->item_type, p_in_item->item_id, p_in_item->item_cnt);
            //1--attire; 2--item
            if (p_in_item->item_type == 1) {
                struct add_attire_t attire = {0};
                attire.attireid = p_in_item->item_id;
                attire.gettime = time(NULL);
                attire.attire_rank = 1;
                attire.duration = p_in_item->duration;
                ret = this->gf_attire.add_attire(USERID_ROLETM, &attire, p_in->max_bag_num);
                if (ret != SUCC) {
                    p_in_item->save_flag = ret;
                    ret = SUCC;
                }
                ret = this->gf_attire.get_max_id(USERID_ROLETM, &uniqueid);
                if (ret != SUCC) {
                    STD_ROLLBACK();
                    return ret;
                }
                p_in_item->uniqueid = uniqueid;
            } else if (p_in_item->item_type == 2) {
                ret = gf_increase_item(USERID_ROLETM,p_in_item->item_id,p_in_item->item_cnt,
                    set_item_limit(p_in_item->item_id),p_in->max_bag_num);
                if (ret != SUCC) {
		    STD_ROLLBACK();
                    p_in_item->save_flag = ret;
                    ret = SUCC;
                }
            }
            break;
        }
        p_in_item += 1;
    }
    cpy_len = sizeof(*p_in) - 4 + (p_in->box_cnt * sizeof(*p_in_item));
    char* p_offset = (char *)PRI_IN_POS;
    memcpy(out, p_offset + 4, cpy_len);
    STD_RETURN_WITH_BUF(ret, out, cpy_len);
}
int Croute_func::gf_set_amb_info(DEAL_FUN_ARG)
{
    DEBUG_LOG("set_amb_info:len=[%u %u]", RECVBUF_USERID, RECVBUF_ROLETM);
    this->gf_amb.set_amb_info(RECVBUF_USERID, RECVBUF_ROLETM);
	STD_RETURN(SUCC);
}

int Croute_func::gf_get_amb_info(DEAL_FUN_ARG)
{
	gf_get_amb_info_out out = {0};
	uint32_t count = 0;
	uint32_t power_flag = 0;
	ret = this->gf_role.get_role_power_user_flag(USERID_ROLETM, &power_flag);

	ret = this->gf_master.get_prentice_cnt(USERID_ROLETM, &count);

	uint32_t vip_lv = gf_get_player_vip_level(RECVBUF_USERID);

	if (count >= 10 + vip_lv * 5 || !power_flag) {
		ret = USER_ID_NOFIND_ERR;
	}
    //ret = this->gf_amb.get_amb_info(RECVBUF_USERID, &out);
	//if (!ret) 
	//{
	//	this->gf_amb.del_amb_info(RECVBUF_USERID);
	//}
	STD_RETURN_WITH_STRUCT(ret, out);
}
//begin store
int Croute_func::gf_prepare_add_product(DEAL_FUN_ARG)
{
    gf_prepare_add_product_in_header *p_in = PRI_IN_POS;
    CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->attire_cnt * sizeof(gf_prepare_add_product_in_item1) + 
        p_in->item_cnt * sizeof(gf_prepare_add_product_in_item2));
    
    DEBUG_LOG("store prepare product : max_limit=[%u] attire_cnt=[%u] item_cnt=[%u]", p_in->max_limit, p_in->attire_cnt, p_in->item_cnt);
    gf_prepare_add_product_in_item1 *p_item1 = (gf_prepare_add_product_in_item1*)(p_in + 1); 
    gf_prepare_add_product_in_item2 *p_item2 = (gf_prepare_add_product_in_item2*)(p_item1 + p_in->attire_cnt);
    uint32_t cur_attire_cnt = 0;
    ret = this->gf_attire.get_attire_cnt(USERID_ROLETM, &cur_attire_cnt, 0);
    if ( p_in->attire_cnt != 0 && cur_attire_cnt + p_in->attire_cnt > p_in->max_bag) {
        DEBUG_LOG("add_product attire error. [%u %u %u]", p_in->max_bag, cur_attire_cnt, p_in->attire_cnt);
        return GF_ITEM_KIND_MAX_ERR;
    }
    uint32_t attire_type_cnt = 0;
    for (uint32_t i = 0; i < p_in->attire_cnt; i++) {
        //DEBUG_LOG("store attire : id duration timelag [%u %u %d]",p_item1->attireid,p_item1->duration,p_item1->timelag);
        attire_type_cnt = 0;
        this->gf_attire.get_attire_cnt_by_attireid(USERID_ROLETM, &attire_type_cnt, p_item1->attireid);
        if (p_in->max_limit != 0 && attire_type_cnt >= p_in->max_limit) {
            DEBUG_LOG("add_product max_limit error. [%u %u %u]",p_item1->attireid,p_in->max_limit,attire_type_cnt);
            return GF_ATTIRE_MAX_BUYLIMIT_ERR;
        }
        p_item1 += 1;
    }
    if (p_in->item_cnt !=0 ) {
        DEBUG_LOG("store item: id cnt [%u %u]", p_item2->itemid, p_item2->cnt);
        //if item is strengthen material return succ
        if (is_strengthen_material(p_item2->itemid)) {
            STD_RETURN(SUCC);
        } else {
            uint32_t cur_item_cnt = 0;
            ret = this->gf_item.get_item_kind_cnt(USERID_ROLETM, &cur_item_cnt);
            if (cur_item_cnt + p_in->item_cnt > p_in->max_bag) {
                DEBUG_LOG("add_product item error. [%u %u %u]", p_in->max_bag, cur_item_cnt, p_in->item_cnt);
                return GF_ITEM_KIND_MAX_ERR;
            }
        }
    }
    /*
    uint32_t item_type_cnt = 0;
    for (uint32_t i = 0; i < p_in->item_cnt; i++) {
        item_type_cnt = 0;
        this->gf_item.get_item_cnt_by_itemid(USERID_ROLETM, &item_type_cnt, p_item2->itemid);
        if (p_in->max_limit != 0 && item_type_cnt >= p_in->max_limit) {
            DEBUG_LOG("add_product max_limit error. [%u %u %u]",p_item2->itemid,p_in->max_limit,item_type_cnt,p_item2->cnt);
            return GF_ATTIRE_MAX_BUYLIMIT_ERR;
        }
        p_item2 += 1;
    }*/
    STD_RETURN(SUCC);
}
int Croute_func::gf_add_store_product(DEAL_FUN_ARG)
{
    char out[PROTO_MAX_SIZE] = {0};
    int cpy_len = 0;
    gf_add_store_product_in_header *p_in = PRI_IN_POS;
    DEBUG_LOG("store product : attire_cnt=[%u] item_cnt=[%u]", p_in->attire_cnt, p_in->item_cnt);
    CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->attire_cnt * sizeof(gf_add_store_product_in_item1) + 
        p_in->item_cnt * sizeof(gf_add_store_product_in_item2));
    
    gf_add_store_product_in_item1 *p_item1 = (gf_add_store_product_in_item1*)(p_in + 1); 
    gf_add_store_product_in_item2 *p_item2 = (gf_add_store_product_in_item2*)(p_item1 + p_in->attire_cnt);

    gf_add_store_product_out_header *out_header = (gf_add_store_product_out_header*)out;
    char *offset = &(out[0]) + sizeof(*out_header);
    out_header->attire_cnt = p_in->attire_cnt;
    out_header->item_cnt = p_in->item_cnt;

    gf_add_store_product_out_item1 out1 = {0};
    for (uint32_t i = 0; i < p_in->attire_cnt; i++) {
        //DEBUG_LOG("store attire : id duration timelag [%u %u %d]",p_item1->attireid,p_item1->duration,p_item1->timelag);
        this->gf_attire.del_timeout_attire_by_attireid(USERID_ROLETM, p_item1->attireid);
        add_attire_t attire_obj = {0};
        attire_obj.attireid = p_item1->attireid;
        attire_obj.duration = p_item1->duration;
        attire_obj.gettime = time(NULL);
        attire_obj.endtime = 0xffffffff;
        if ( p_item1->timelag > 0 ) {
            attire_obj.endtime = time(NULL) + ((p_item1->timelag + 1) * 3600 * 24);
        }
        ret = this->gf_attire.add_attire(USERID_ROLETM, &attire_obj, p_in->max_bag);
        if ( ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }

        memset(&out1, 0, sizeof(gf_add_store_product_out_item1));
        out1.attireid = attire_obj.attireid;
        out1.gettime = attire_obj.gettime;
        out1.timelag = attire_obj.endtime;
        ret = this->gf_attire.get_max_id(USERID_ROLETM, &(out1.uniquekey));
        if ( ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
        cpy_len = sizeof(gf_add_store_product_out_item1);
        memcpy(offset, &out1, cpy_len);
        offset += cpy_len;

        p_item1 += 1;
    }

    gf_add_store_product_out_item2 out2 = {0};
    for (uint32_t i = 0; i < p_in->item_cnt; i++) {
        DEBUG_LOG("store item : id cnt [%u %u]",p_item2->itemid,p_item2->cnt);
        ret = gf_increase_item(USERID_ROLETM,p_item2->itemid,p_item2->cnt,set_item_limit(p_item2->itemid),p_in->max_bag, item_change_log_opt_type_vip_add);
        if ( ret != SUCC ) {
            STD_ROLLBACK();
            return ret;
        }
        memset(&out2, 0, sizeof(gf_add_store_product_out_item2));
        out2.itemid = p_item2->itemid;
        if (out2.itemid == gold_coin_item_id) {
            ret = this->gf_item.get_item_int_value(USERID_ROLETM, gold_coin_item_id, "count", &(out2.cnt));
        } else {
            out2.cnt = p_item2->cnt;
        }
        cpy_len = sizeof(gf_add_store_product_out_item2);
        memcpy(offset, &out2, cpy_len);
        offset += cpy_len;

        p_item2 += 1;
    }
    STD_RETURN_WITH_BUF(ret, out, offset-&(out[0]));
}
int Croute_func::gf_prepare_repair_product(DEAL_FUN_ARG)
{
    gf_prepare_repair_product_in_header *p_in = PRI_IN_POS;
    gf_prepare_repair_product_in_item *p_item = (gf_prepare_repair_product_in_item *)(p_in + 1);
    CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->product_cnt * sizeof(gf_prepare_repair_product_in_item));

    uint32_t cur_attire_cnt = 0;
    ret = this->gf_attire.get_attire_cnt(USERID_ROLETM, &cur_attire_cnt, 0);
    if ( cur_attire_cnt + p_in->product_cnt > p_in->max_bag) {
        DEBUG_LOG("repair_product attire error. [%u %u %u]", p_in->max_bag, cur_attire_cnt, p_in->product_cnt);
        return GF_ITEM_KIND_MAX_ERR;
    }
    uint32_t attireid = 0;
    DEBUG_LOG("repair_product cnt=[%u]:",p_in->product_cnt);
    for (uint32_t i = 0; i < p_in->product_cnt; i++) {
        //DEBUG_LOG("repair_product id key timelag [%u %u %d]",p_item->attireid, p_item->uniquekey, p_item->timelag);
        ret = this->gf_attire.get_outdated_attire_by_index(USERID_ROLETM, p_item->uniquekey ,&attireid);
        if ( ret != SUCC) {
            return ret;
        }
        if (attireid != p_item->attireid) {
            return GF_ATTIREID_NOFIND_ERR;
        }
        p_item += 1;
    }
    STD_RETURN(SUCC);    
}
int Croute_func::gf_repair_store_product(DEAL_FUN_ARG)
{
    char out[PROTO_MAX_SIZE] = {0};
    int cpy_len = 0;
    gf_repair_store_product_in_header *p_in = PRI_IN_POS;
    gf_repair_store_product_in_item *p_item = (gf_repair_store_product_in_item *)(p_in + 1);
    DEBUG_LOG("repair_product cnt=[%u]:",p_in->product_cnt);
    CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->product_cnt * sizeof(gf_repair_store_product_in_item));
    
    gf_repair_store_product_out_header *out_header = (gf_repair_store_product_out_header*)out;
    char *offset = &(out[0]) + sizeof(*out_header);
    out_header->count = p_in->product_cnt;

    gf_repair_store_product_out_item out_item = {0};
    for (uint32_t i = 0; i < p_in->product_cnt; i++) {
        //DEBUG_LOG("repair_product id key timelag [%u %u %d]",p_item->attireid, p_item->uniquekey, p_item->timelag);
        uint32_t gettime = time(NULL);
        uint32_t endtime = 0xffffffff;
        if ( p_item->timelag > 0 ) {
            endtime = time(NULL) + ((p_item->timelag + 1) * 3600 * 24);
        }
        ret = this->gf_attire.getback_attire_by_index(USERID_ROLETM, p_item->uniquekey, gettime, endtime);
        if ( ret != SUCC) {
            return ret;
        }
        
        memset(&out_item, 0, sizeof(gf_repair_store_product_out_item));
        out_item.attireid = p_item->attireid;
        out_item.uniquekey = p_item->uniquekey;
        out_item.gettime = gettime;
        out_item.timelag = endtime;

        cpy_len = sizeof(gf_repair_store_product_out_item);
        memcpy(offset, &out_item, cpy_len);
        offset += cpy_len;

        p_item += 1;
    }
    STD_RETURN_WITH_BUF(ret, out, offset-&(out[0]));
}
int Croute_func::gf_get_outdated_product(DEAL_FUN_ARG)
{
    gf_get_outdated_product_out_header out_header = {0};
    gf_get_outdated_product_out_item* p_out_item ;
    ret = this->gf_attire.get_attire_by_usedflg(USERID_ROLETM, &(out_header.count), &(p_out_item), 2);
    if (ret != SUCC) {
        return ret;
    }
    STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}
int Croute_func::gf_synchro_user_gfcoin(DEAL_FUN_ARG)
{
    gf_synchro_user_gfcoin_in *p_in = PRI_IN_POS;

	ret = this->gongfu.add_user_for_boss(RECVBUF_USERID);
	if (ret != SUCC) {
		STD_ROLLBACK();
        return ret;
    }
    ret = this->gongfu.set_int_value(RECVBUF_USERID, p_in->count, "gold_coin");
    if (ret != SUCC) {
        return ret;
    }
    STD_RETURN(SUCC);
}
int Croute_func::gf_query_user_gfcoin(DEAL_FUN_ARG)
{
    gf_query_user_gfcoin_out out = {0};
    ret = this->gongfu.get_user_gfcoin(RECVBUF_USERID, &(out.count));
    if (ret != SUCC) {
        return ret;
    }
    STD_RETURN_WITH_STRUCT(ret, out);
}
//end store
/**
 * @fn coupons exchange function
 * @brief add item to package
 */
int Croute_func::gf_coupons_exchange(DEAL_FUN_ARG)
{
    char out_buf[PROTO_MAX_SIZE] = {0};
    int cpy_len = 0;
    gf_coupons_exchange_in_header *p_in = PRI_IN_POS;
    CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->add_cnt * sizeof(gf_coupons_exchange_in_item));
    gf_coupons_exchange_in_item *p_in_item = (gf_coupons_exchange_in_item*)(p_in + 1);

    gf_coupons_exchange_out_header *out_header = (gf_coupons_exchange_out_header*)out_buf;                                 
    char* p_offset = &(out_buf[0]) + sizeof(*out_header); 

    ret = gf_reduce_item_count(USERID_ROLETM, p_in->del_item_id, p_in->del_item_cnt);
    //ret = this->gf_item.reduce_item_int_value(USERID_ROLETM, p_in->del_item_id, "count", p_in->del_item_cnt);
    if ( ret != SUCC ) {
	STD_ROLLBACK();
        return ret;
    }

    gf_coupons_exchange_out_item out = {0};
    for (uint32_t i = 0; i < p_in->add_cnt; i++) {
        memset(&out, 0, sizeof(gf_coupons_exchange_out_item));
        out.id  = p_in_item->item_id;
        out.cnt = p_in_item->item_cnt;
        out.type = p_in_item->type;
        switch ( p_in_item->type )
        {
        case TYPE_ATTIRE:
            m_attire_elem.attireid = p_in_item->item_id;
            m_attire_elem.gettime = time(NULL);
            m_attire_elem.attire_rank = 0;
            m_attire_elem.duration = p_in_item->duration;
            if (p_in_item->lifetime == 0) {
                m_attire_elem.endtime = 0;
            } else {
                m_attire_elem.endtime = m_attire_elem.gettime + ((p_in_item->lifetime+1) * 3600 * 24);
            }
            m_attire_elem.usedflag = 0;
            ret = this->gf_attire.add_attire(USERID_ROLETM, &m_attire_elem, p_in->max_bag);
            if ( ret != SUCC ) {
                STD_ROLLBACK();
                return ret;
            }

            ret = this->gf_attire.get_max_id(USERID_ROLETM, &(out.uniqueid));
            if ( ret != SUCC ){
                STD_ROLLBACK();
                return ret;
            }
            out.cnt = 1;
            out.gettime = m_attire_elem.gettime;
            out.endtime = m_attire_elem.endtime;
            break;
        case TYPE_ITEM:
            ret = gf_increase_item(USERID_ROLETM, p_in_item->item_id, p_in_item->item_cnt, 
                DEFAULT_MAX_BACKAGE, p_in->max_bag);
            break;
            /*
        case TYPE_ROLE:
            if (p_in_item->item_id == TYPE_ROLE_XIAOMEE)
                ret = this->gf_role.increase_int_value(USERID_ROLETM, ROLE_XIAOMEE_COLUMN, p_in_item->item_cnt);
            else if (p_in_item->item_id == TYPE_ROLE_EXP)
                ret = this->gf_role.increase_int_value(USERID_ROLETM, ROLE_EXP_COLUMN, p_in_item->item_cnt);
            else if (p_in_item->item_id == TYPE_ROLE_SKILLPOINT)
                ret = this->gf_role.increase_int_value(USERID_ROLETM, ROLE_SKILL_POINT_COLUMN, p_in_item->item_cnt);
            else if (p_in_item->item_id == TYPE_ROLE_FUMOPOINT)
                ret = this->gf_role.increase_int_value(USERID_ROLETM, ROLE_FUMO_POINTS_COLUMN, p_in_item->item_cnt);
            else if (p_in_item->item_id == TYPE_ROLE_PVP_FIGHT)
                ret = this->gf_role.increase_int_value(USERID_ROLETM, ROLE_EXPLOIT_COLUMN, p_in_item->item_cnt);
            else if (p_in_item->item_id == TYPE_ROLE_HONOR)
                ret = this->gf_role.increase_int_value(USERID_ROLETM, ROLE_HONOR_COLUMN, p_in_item->item_cnt);
            else {
                return ENUM_OUT_OF_RANGE_ERR;
            }
            break;
            */
        default:
            DEBUG_LOG("there enum out of range %u ", p_in_item->type);
            return ENUM_OUT_OF_RANGE_ERR;
            break;
        }
        if ( ret != SUCC ) {
            STD_ROLLBACK();
            return ret;
        }

        cpy_len = sizeof(gf_coupons_exchange_out_item);
        DEBUG_LOG("coupons exchange:[%u %u %u %u]",out.type,out.id,out.cnt,out.uniqueid);
        memcpy(p_offset, &out, cpy_len);
        p_offset += cpy_len;
        p_in_item += 1;

        out_header->item_cnt++;
    }

    out_header->trade_id = p_in->trade_id;
    out_header->left_coupons = 0;
    //ret = this->gf_item.get_item_int_value(USERID_ROLETM,p_in->del_item_id,"count",&(out_header->left_coupons));
    ret = gf_get_item_count(USERID_ROLETM,p_in->del_item_id, &(out_header->left_coupons));
    if ( ret !=SUCC && ret != GF_ITEM_NOFIND_ERR) {
        STD_ROLLBACK();
        return ret;
    }
    STD_RETURN_WITH_BUF(SUCC, out_buf, p_offset-&(out_buf[0]));
}

int Croute_func::gf_set_role_state(DEAL_FUN_ARG)
{
	gf_set_role_state_in* p_in = PRI_IN_POS;
	gf_role.set_role_show_state(RECVBUF_USERID, RECVBUF_ROLETM, p_in->state);
	return SUCC;
}

int Croute_func::gf_set_open_box_times(DEAL_FUN_ARG)
{
	gf_set_open_box_times_in* p_in = PRI_IN_POS;
	gf_role.set_role_open_box_times(RECVBUF_USERID, RECVBUF_ROLETM, p_in->times);
	return SUCC;
}


int Croute_func::gf_get_donate_count(DEAL_FUN_ARG)
{
	gf_donate_count_out out ={ 0 };
	ret = gf_donate.get_red_black_count( &(out.red) , &(out.black));
	if(ret != SUCC){
		return ret;
	}
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_add_buff(DEAL_FUN_ARG)
{
	gf_add_buff_in* p_in = PRI_IN_POS;
	gf_add_buff_out_header out = {0};
	ret = gf_buff.add_buff(USERID_ROLETM, p_in->buff_type, p_in->duration, p_in->mutex_type, p_in->start_tm);
//	if (ret != SUCC) {
//        ret = gf_buff.update_buff(USERID_ROLETM, p_in->buff_type, p_in->duration);
//    }
	out.buff_type = p_in->buff_type;
	out.duration = p_in->duration;
	out.mutex_type = p_in->mutex_type;
	out.start_tm = p_in->start_tm;
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_update_buff_list(DEAL_FUN_ARG)
{
	gf_update_buff_list_in_header* p_in = PRI_IN_POS;
	gf_update_buff_list_in_item *p_in_item = (gf_update_buff_list_in_item *)(p_in + 1);

	CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->count * sizeof(*p_in_item));
	gf_buff.del_all_btl_buff(USERID_ROLETM);
	for (uint32_t i = 0; i < p_in->count; i++)
	{
		ret = gf_buff.add_buff(USERID_ROLETM, p_in_item->buff_type, p_in_item->duration, p_in_item->mutex_type, p_in_item->start_tm);
		p_in_item++;
	}
	return SUCC;
}

int Croute_func::gf_del_buff_on_player(DEAL_FUN_ARG) 
{
	gf_del_buff_on_player_in * p_in = PRI_IN_POS;
	gf_buff.del_one_buff_on_player(USERID_ROLETM, p_in->buff_type);
	return SUCC;
}

int Croute_func::gf_notify_use_item(DEAL_FUN_ARG)
{
	gf_notify_use_item_in* p_in = PRI_IN_POS;
	gf_item_log.insert_use_item_log(RECVBUF_USERID, RECVBUF_ROLETM, p_in->item_id, p_in->item_cnt);
	return SUCC;
}

int Croute_func::gf_notify_user_get_shop_item(DEAL_FUN_ARG)
{
	gf_notify_user_get_shop_item_in* p_in = PRI_IN_POS;
	gf_get_shopitem_log.insert_item_log(RECVBUF_USERID, RECVBUF_ROLETM, p_in->item_id, p_in->item_cnt);
	return SUCC;
}

int Croute_func::gf_get_hero_top_info(DEAL_FUN_ARG)
{
	gf_get_hero_top_info_in * p_in = PRI_IN_POS;
	gf_get_hero_top_info_out out = {0}; 
	ret = gf_hero_top.get_user_hero_top_info(p_in->user_exp, &out.sort);
	if (ret != SUCC)
	{
		return ret;
	}
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_set_hero_top_info(DEAL_FUN_ARG)
{
	gf_set_hero_top_info_in * p_in = PRI_IN_POS;
	uint32_t sort = 0;
	ret = gf_hero_top.get_user_hero_top_info(p_in->user_exp, &sort);
	if (ret != SUCC)
	{
		STD_RETURN(ret);
	}
	if (sort < 1000)
	{
		ret = gf_hero_top.set_user_hero_top_info(RECVBUF_USERID, RECVBUF_ROLETM, p_in->user_lv, p_in->user_exp);
		if (ret != SUCC)
		{
			STD_RETURN(ret);
		}
	}
	STD_RETURN(ret);
}

/** 
 * @brief 领取伏魔塔奖励时，验证上周数据 
 * 
 */
int Croute_func::gf_get_hunter_info(DEAL_FUN_ARG)
{
	gf_get_hunter_info_in * p_in = PRI_IN_POS;
    gf_get_hunter_info_out out = {0};
	uint32_t role_type = p_in->role_type;
    uint32_t layer = 0, interval = 0, place_tm = 0;
	//上周战斗记录层数， 时间
    ret = gf_hunter_top.get_over_tower_info(RECVBUF_USERID, RECVBUF_ROLETM, &layer, &interval, &place_tm, 1);
    out.layer = layer;
    out.interval = interval;
    if (ret == SUCC && out.layer) {
        uint32_t order = 0;
		//上周排名
        ret = this->gf_hunter_top.get_hunter_order(role_type, layer, interval, place_tm, &order, 1);
        out.order = order + 1;
    } else {
    	out.order= 0;
		ret = SUCC;
    }
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_get_hunter_top_info(DEAL_FUN_ARG)
{
	//	ERROR_LOG("get hunter top info");
	gf_get_hunter_top_info_in * p_in = PRI_IN_POS;
    gf_get_hunter_top_info_out_header out_header = {0};
    gf_get_hunter_top_info_out_item * p_item = NULL;
	uint32_t role_type = p_in->role_type;

    //get player self information
    uint32_t layer = 0, interval = 0, place_tm = 0;

	/** 
	 * @brief 拉去玩家自己本周战斗记录信息 层数，时间
	 */
    ret = this->gf_hunter_top.get_over_tower_info(RECVBUF_USERID, RECVBUF_ROLETM, &layer, &interval, &place_tm);

	//ERROR_LOG("get other record!");	

    out_header.layer  = layer;
    out_header.interval = interval;

    if (ret == SUCC) {
        uint32_t order = 0;
		/** 
		 * @brief 拉取玩家附魔塔本周排名 
		 */
        ret = this->gf_hunter_top.get_hunter_order(role_type, layer, interval, place_tm, &order);
        out_header.order = order;
    }

	/** 
	 * @brief 拉去排名前10的信息 
	 * 
	 */
    ret = this->gf_hunter_top.get_hunter_top_info(role_type, 10, &(out_header.cnt), &p_item);
	ERROR_LOG("GOT HUNTER OUT_%u", out_header.cnt);

    STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_item, out_header.cnt);
}


int Croute_func::gf_get_hunter_top_info_for_web(DEAL_FUN_ARG)
{
	gf_get_hunter_top_info_for_web_in * p_in = PRI_IN_POS;
    gf_get_hunter_top_info_for_web_out_header out_header = {0};
    gf_get_hunter_top_info_for_web_out_item * p_item = NULL;
	uint32_t role_type = p_in->role_type;

	/** 
	 * @brief 拉去排名前10的信息 
	 * 
	 */
    ret = this->gf_hunter_top.get_hunter_top_info(role_type, p_in->top_num, &(out_header.cnt), &p_item);
	DEBUG_LOG("HUNTER TOP FOR WEB %u", out_header.cnt);
    STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_item, out_header.cnt);
}

int Croute_func::gf_get_clothes_info_for_web(DEAL_FUN_ARG)
{
    gf_get_clothes_info_for_web_out_header out_header = {0};
    gf_get_clothes_info_for_web_out_item * p_item = NULL;

    ret = this->gf_attire.get_clothes_info_for_web(RECVBUF_USERID, RECVBUF_ROLETM, &(out_header.cnt), &p_item);
	DEBUG_LOG("CLOTHES INFO FOR WEB %u", out_header.cnt);
    STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_item, out_header.cnt);
}

/** 
 * @brief 记录附魔塔战斗信息 
 * 
 */
int Croute_func::gf_set_hunter_top_info(DEAL_FUN_ARG)
{
    gf_set_hunter_top_info_in* p_in = PRI_IN_POS;
	uint32_t top_cnt = 0;
	uint32_t role_type = p_in->role_type;
	this->gf_hunter_top.get_top_cnt(&top_cnt, role_type);
	if (top_cnt >= 100) { //大于100名
		uint32_t layer = 0;
		uint32_t tm = 0;
		uint32_t place_tm = 0;
		//与最后一名比较
		ret = this->gf_hunter_top.get_last_info(&layer, &tm, &place_tm, role_type);
		if (p_in->layer < layer) {
			STD_RETURN(SUCC);
		}
		if (p_in->layer == layer && p_in->interval > tm) {
			STD_RETURN(SUCC);
		}
		if (p_in->layer == layer && p_in->interval == tm) {
			STD_RETURN(SUCC);
		}
	}
	 // @brieaf  数据记录小于100时直接插入
    ret = this->gf_hunter_top.set_hunter_top_info(RECVBUF_USERID, RECVBUF_ROLETM, p_in);
	this->gf_hunter_top.cut_hunter_top(role_type);
    STD_RETURN(SUCC);
}

int Croute_func::gf_del_player_place_in_old_top(DEAL_FUN_ARG)
{
	this->gf_hunter_top.del_player_place_in_old_top(USERID_ROLETM);
	STD_RETURN(SUCC);
}

int Croute_func::gf_add_shop_log(DEAL_FUN_ARG)
{
	gf_add_shop_log_in * p_in = PRI_IN_POS;
	ret = gf_shop_log.add_shop_log(RECVBUF_USERID, RECVBUF_ROLETM, p_in);
	STD_RETURN(ret);
}

int Croute_func::gf_shop_last_log(DEAL_FUN_ARG)
{
	gf_get_shop_last_log_out_header out_header = { 0 };
	gf_get_shop_last_log_out_item* p_out_item = 0;
	ret = gf_shop_log.get_shop_last_log_list(RECVBUF_USERID, RECVBUF_ROLETM, &(out_header.count), &p_out_item);
	if (ret != SUCC)
	{
		STD_RETURN(ret);
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::gf_add_donate_count(DEAL_FUN_ARG)
{
	gf_add_donate_count_in *p_in = PRI_IN_POS;
	if( p_in->value == 1){
		gf_donate.add_red();
	}else{
		gf_donate.add_black();
	}
	STD_RETURN(SUCC);
}

int Croute_func::gf_set_role_vitality_point(DEAL_FUN_ARG)
{
	gf_set_role_vitality_point_in *p_in = PRI_IN_POS;
	gf_role.set_role_vitality_point(RECVBUF_USERID, RECVBUF_ROLETM, p_in->point);
	STD_RETURN(SUCC);
}

int Croute_func::gf_set_second_pro_exp(DEAL_FUN_ARG)
{
	gf_set_second_pro_exp_in* p_in = PRI_IN_POS;
	gf_secondary_pro.update_player_secondary_pro(RECVBUF_USERID, RECVBUF_ROLETM, p_in->pro_type, p_in->pro_exp);	
	STD_RETURN(SUCC);
}

//extern interface for boss
int Croute_func::gf_get_role_base_info_for_boss(DEAL_FUN_ARG)
{
	//ROLE_TYPE LEVEL NICK CLOTHES
	gf_get_role_base_info_for_boss_out_header out_header = {0};
	gf_get_role_base_info_for_boss_out_item* p_out_item = 0;
	userid_t role_tm = 0;
	ret = gf_role.get_role_base_info_for_boss(RECVBUF_USERID, &role_tm, &out_header);
	if (ret != SUCC)
	{
		STD_RETURN(ret);
	}

	ret = gf_attire.get_clothes_list_for_boss(RECVBUF_USERID, role_tm, &(out_header.clothes_num), &p_out_item);
	if (ret != SUCC)
	{
		STD_RETURN(ret);
	}
	RECVBUF_ROLETM = role_tm;
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.clothes_num);
	
}

int Croute_func::gf_get_player_community_info(DEAL_FUN_ARG)
{
	//ROLE_TYPE LEVEL NICK CLOTHES
	gf_get_player_community_info_out out = {0};
	ret = gf_role.get_community_info(USERID_ROLETM, &out);
	if (ret != SUCC)
	{
		STD_RETURN(ret);
	}

	ret = gf_friend.get_friend_count(RECVBUF_USERID, TYPE_FRIEND, &(out.friend_cnt));
	if (ret != SUCC)
	{
		STD_RETURN(ret);
	}

	ret = gf_card.get_card_cnt(USERID_ROLETM, &out);
	if (ret != SUCC)
	{
		STD_RETURN(ret);
	}

	ret = gf_summon.get_role_summon_cnt(USERID_ROLETM, &out);
	if (ret != SUCC)
	{
		STD_RETURN(ret);
	}

	STD_RETURN_WITH_STRUCT(ret, out);

}
int Croute_func::gf_get_ring_task_list(DEAL_FUN_ARG)
{
	gf_get_ring_task_list_out_header out_header = { 0 };
	gf_get_ring_task_list_out_element* p_out_item = NULL;

	ret = gf_ring_task.get_ring_task_list(RECVBUF_USERID, RECVBUF_ROLETM, &p_out_item, &(out_header.count));
	if(ret != SUCC)
	{
		STD_RETURN(ret);
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::gf_get_ring_task_history_list(DEAL_FUN_ARG)
{
	gf_get_ring_task_history_list_out_header out_header = { 0 };
	gf_get_ring_task_history_list_out_element* p_out_item = NULL;

	ret = gf_ring_task_history.get_ring_task_history_list(RECVBUF_USERID, RECVBUF_ROLETM, &p_out_item, &(out_header.count));
	if(ret != SUCC)
	{
		STD_RETURN(ret);
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::gf_replace_ring_task(DEAL_FUN_ARG)
{
	gf_replace_ring_task_in *p_in = PRI_IN_POS;	
	ret = gf_ring_task.replace_ring_task(RECVBUF_USERID, RECVBUF_ROLETM, p_in->task_id, p_in->task_type, 
										 p_in->master_task_id,
										 p_in->task_group_id
										 );
	if(ret != SUCC)
	{
		STD_RETURN(ret);
	}
	return 0;	
}


int Croute_func::gf_delete_ring_task(DEAL_FUN_ARG)
{
	gf_delete_ring_task_in* p_in = PRI_IN_POS;
	if(p_in->task_type == 5)
	{
		gf_ring_task.delete_ring_master_task(RECVBUF_USERID, RECVBUF_ROLETM, p_in->task_id);
	}
	if(p_in->task_type == 6)
	{
		gf_ring_task.delete_ring_slave_task(RECVBUF_USERID, RECVBUF_ROLETM, p_in->task_id);
	}
	if(ret != SUCC)
	{
		STD_RETURN(ret);
	}
	return 0;
}

int Croute_func::gf_replace_ring_task_history(DEAL_FUN_ARG)
{
	gf_replace_ring_task_history_in* p_in = PRI_IN_POS;
	ret = gf_ring_task_history.replace_ring_task_history(RECVBUF_USERID, RECVBUF_ROLETM, p_in->task_id, 
											p_in->day_count,
											p_in->last_tm
										);
	if(ret != SUCC)
	{
		STD_RETURN(ret);
	}
	return 0;
}

//for 天下第一比武大会
/** 
 * @brief  加入比武队伍
 */
int Croute_func::gf_join_contest_team(DEAL_FUN_ARG)
{
//	ERROR_LOG("JOIN TEAM!________");
	gf_join_contest_team_in * p_in = PRI_IN_POS;
	gf_join_contest_team_out out = {0};
	int ret = gf_contest.join_contest_team(RECVBUF_USERID, RECVBUF_ROLETM, p_in);
	if (ret != SUCC) {
		STD_RETURN(ret);
	}
	out.team_id = p_in->team_id;
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_get_contest_team_info(DEAL_FUN_ARG)
{
//	ERROR_LOG("GET TEAM!_______");
	
	gf_get_contest_team_info_header header = {0};

	contest_team_info_t * teams;
	int ret = gf_contest.get_contest_team_info(&teams, &(header.team_cnt));
	if (ret != SUCC) {
		STD_RETURN(ret);
	}
//	ERROR_LOG("GoT TEAM! %u", header.team_cnt);
	STD_RETURN_WITH_STRUCT_LIST(ret, header, teams, header.team_cnt);
}


int Croute_func::gf_get_contest_donate(DEAL_FUN_ARG)
{
//	ERROR_LOG("GET CONTEST DONATE!>>>>>>>");
	gf_get_contest_donate_header header;
    uint32_t win_stage = 0;
	int ret = gf_contest.get_one_team(RECVBUF_USERID, RECVBUF_ROLETM, &(header.self_team),
			&(header.self_cnt), &win_stage);
	if (ret != SUCC) {
		STD_RETURN(ret);
	}

	if (header.self_team) {
		donate_t * donate_in;
		ret = gf_contest.get_contest_donate_info(&donate_in, header.self_team, &(header.top_cnt));
		if (ret != SUCC) {
			STD_RETURN(ret);
		}

		STD_RETURN_WITH_STRUCT_LIST(ret, header, donate_in, header.top_cnt);

	} else {
		header.top_cnt = 0;
		STD_RETURN_WITH_STRUCT(ret, header);
	}

}

int Croute_func::gf_donate_item(DEAL_FUN_ARG)
{
//	ERROR_LOG("DONATE ITEM!............");
	gf_donate_item_in_header * p_in = PRI_IN_POS;

	CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->type_cnt * sizeof(gf_donate_item_in_item));

	ret = 0;

	for (uint32_t i = 0; i < p_in->type_cnt; i++) {
		gf_donate_item_in_item* item = (gf_donate_item_in_item*)((char*)PRI_IN_POS + 
				sizeof(*p_in) + sizeof(gf_donate_item_in_item) * i);
		if (item->cnt) {

			if (item->type == 1) {
				uint32_t left_num = 0;
				ret = this->gf_role.reduce_int_value(USERID_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, 
					ROLE_XIAOMEE_COLUMN, item->cnt, &(left_num));
				if (ret != SUCC) {
					item->cnt = 0;
					STD_ROLLBACK();
				}
			} else {
				ret = gf_item.reduce_item_int_value(RECVBUF_USERID, RECVBUF_ROLETM, item->type,
					   	"count", item->cnt);
			//	ERROR_LOG("reudec item %u", ret);
				if (ret != SUCC) {
					item->cnt = 0;
					STD_ROLLBACK();
				}
			}
		}
	}

	STD_RETURN_WITH_BUF(ret, (char*)p_in, p_in->type_cnt * sizeof(gf_donate_item_in_item) + 4);
}

int Croute_func::gf_contest_donate_plant(DEAL_FUN_ARG)
{
//	ERROR_LOG("ADD CONTEST DONATE!");
	gf_contest_donate_plant_in * p_in = PRI_IN_POS;
	uint32_t plant_cnt = p_in->plant_cnt;
	int ret = gf_contest.contest_donate_plant(RECVBUF_USERID, RECVBUF_ROLETM, plant_cnt);
	if (ret != SUCC) {
		STD_RETURN(ret);
	}	
	STD_RETURN_WITH_STRUCT(ret, *p_in);
}

int Croute_func::gf_get_self_contest_team(DEAL_FUN_ARG)
{
	gf_get_self_contest_out out = {0};
	int ret = gf_contest.get_one_team(RECVBUF_USERID, RECVBUF_ROLETM, &(out.team_id),
		   	&(out.plant_cnt), &(out.win_stage));
	if (ret != SUCC) {
		STD_RETURN(ret);
	}
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_upgrade_item(DEAL_FUN_ARG)
{
	gf_upgrade_item_in_header* p_in = PRI_IN_POS;
	gf_upgrade_item_in_item *p_in_item = (gf_upgrade_item_in_item *)(p_in + 1);

	ret = SUCC;
	DEBUG_LOG("%u %u", p_in->del_cnt, p_in->add_cnt);
	for (uint32_t i = 0; i < p_in->del_cnt; i++) {
		switch (p_in_item->type)
		{
			case TYPE_ATTIRE:
				ret = this->gf_attire.del_attire_with_index(USERID_ROLETM, p_in_item->count);
				break;
			case TYPE_ITEM:
				//ret = gf_item.reduce_item_int_value(USERID_ROLETM,p_item1->id,item_count_column,p_item1->count);
				if (p_in_item->count) {
                ret = gf_reduce_item_count(USERID_ROLETM, p_in_item->id, p_in_item->count);
					if (ret == GF_ITEM_NOFIND_ERR) {
						STD_ROLLBACK();
						return ENUM_OUT_OF_RANGE_ERR;
					}
				}
				break;
			case TYPE_ROLE:
				if (p_in_item->id==TYPE_ROLE_XIAOMEE)
					ROLE_REDUCE_VALUE(XIAOMEE,RECVBUF_USERID, RECVBUF_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, p_in_item->count, NULL);
				else if (p_in_item->id==TYPE_ROLE_EXP)
					ROLE_REDUCE_VALUE(EXP,RECVBUF_USERID, RECVBUF_ROLETM, GF_EXP_NOENOUGH_ERR, p_in_item->count, NULL);
                else if (p_in_item->id==TYPE_ROLE_FUMOPOINT)
					ROLE_REDUCE_VALUE(FUMO_POINTS,RECVBUF_USERID, RECVBUF_ROLETM, GF_FUMO_NOENOUGH_ERR, p_in_item->count, NULL);

				else
				{
					STD_ROLLBACK();
					return ENUM_OUT_OF_RANGE_ERR;
				}
				break;
			default:
				STD_ROLLBACK();
				return ENUM_OUT_OF_RANGE_ERR;
				break;
		}	
		p_in_item ++;
	}
	
	for (uint32_t i = 0; i < p_in->add_cnt; i++) {
		switch (p_in_item->type)
		{
			case TYPE_ATTIRE:		
				m_attire_elem.attireid = p_in_item->id;
				m_attire_elem.gettime = time(NULL);
				m_attire_elem.attire_rank = 1;
				m_attire_elem.duration = p_in_item->duration;
                m_attire_elem.endtime = 0;
				m_attire_elem.usedflag = 0;
				ret = this->gf_attire.add_attire(USERID_ROLETM, &m_attire_elem, 999, &(p_in_item->count));
				if (ret!=SUCC)
				{
					STD_ROLLBACK();
					return ret;			
				}
				break;
			default:
				STD_ROLLBACK();
                DEBUG_LOG("there %u ", p_in_item->type);
				return ENUM_OUT_OF_RANGE_ERR;
				break;
		}
		
		if (ret!=SUCC)
		{
			STD_ROLLBACK();
			return ret;			
		}
		p_in_item ++;
		
	}

	STD_RETURN_WITH_BUF(ret, p_in, sizeof(gf_upgrade_item_in_header) + (p_in->add_cnt + p_in->del_cnt) * sizeof(gf_upgrade_item_in_item));
}

int Croute_func::gf_get_pvp_game_rank(DEAL_FUN_ARG)
{
    gf_get_pvp_game_rank_in * p_in = PRI_IN_POS;
    gf_get_pvp_game_rank_out_header out_header = { 0 };
    gf_get_pvp_game_rank_out_item* p_item = NULL;

    ret = gf_taotai.get_final_score_rank_list(&p_item, &(out_header.count), p_in->rank);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
    out_header.rank = p_in->rank;
    STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_item, out_header.count);
}

int Croute_func::gf_get_pvp_game_data(DEAL_FUN_ARG)
{
    gf_get_pvp_game_data_in * p_in = PRI_IN_POS;

    char out[PROTO_MAX_SIZE] = {0};
	gf_get_pvp_game_data_out_header* p_out_header = (gf_get_pvp_game_data_out_header*)out;
    p_out_header->count = p_in->pvp_type;
    char* p_offset = &(out[0]) + sizeof(*p_out_header);

    gf_get_pvp_game_data_out_item item = {0};
    for (uint32_t i = 0; i <= p_in->pvp_type; i ++) {
        memset(&item, 0, sizeof(gf_get_pvp_game_data_out_item));
        item.pvp_type = i;
        ret = gf_taotai.get_role_pvp_game_info(USERID_ROLETM,&(item.do_times), 
            &(item.win_times), &(item.flower), &(item.rank), &(item.segment), i);
        if (ret != SUCC && ret != ROLE_ID_EXISTED_ERR) {
            STD_RETURN(ret);
        }
        if (ret == ROLE_ID_EXISTED_ERR) {
            ret = 0;
            item.do_times  = 0;
            item.win_times = 0;
            item.flower    = 0;
            item.rank      = 0;
            item.segment   = 0;
        }

        memcpy(p_offset, &(item), sizeof(gf_get_pvp_game_data_out_item));
        p_offset += sizeof(gf_get_pvp_game_data_out_item);
    }
    STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0]));
}

int Croute_func::gf_set_pvp_game_data(DEAL_FUN_ARG)
{
	gf_set_pvp_game_data_in * p_in = PRI_IN_POS;
//	ERROR_LOG("PLAYER SET GAME %u INFO!", p_in->userid);
    ret = gf_taotai.set_pvp_game_info(p_in->userid, p_in->roletm, p_in->lv, p_in->nick,
        p_in->done ,p_in->win, p_in->pvp_type, p_in->flower_cnt);
    if (ret != SUCC) {
        STD_ROLLBACK();
    }
    STD_RETURN(ret);
}


int Croute_func::gf_get_swap_action(DEAL_FUN_ARG)
{
	gf_get_swap_action_out_header out_header  = {0};
	gf_get_swap_action_out_item  *p_out_item = 0;
	ret = this->gf_swap_action.get_action_type_list(USERID_ROLETM, &(out_header.cnt), &p_out_item);
	if (ret != SUCC) {
        STD_ROLLBACK();
    }
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.cnt);
}

//added by cws 0607 for get swap list
int Croute_func::gf_add_swap(DEAL_FUN_ARG)
{
	gf_add_swap_in  *p_in = PRI_IN_POS;
	ret = this->gf_swap_action.add_action_type_list(RECVBUF_USERID, RECVBUF_ROLETM, p_in);
	STD_RETURN(ret);
}
///by cws
int Croute_func::gf_clear_swap_action(DEAL_FUN_ARG)
{
    gf_clear_swap_action_in * p_in = PRI_IN_POS;
	ret = this->gf_swap_action.delete_action(RECVBUF_USERID, RECVBUF_ROLETM, p_in);
    STD_RETURN(SUCC);
}

int Croute_func::gf_set_pvp_game_flower(DEAL_FUN_ARG)
{
    gf_set_pvp_game_flower_in * p_in = PRI_IN_POS;
    ret = gf_taotai.set_pvp_game_flower(p_in->userid, p_in->roletm, p_in->flower, p_in->pvp_type);
    if (ret != SUCC) {
        STD_ROLLBACK();
    }
    STD_RETURN(ret);
}


int Croute_func::gf_swap_action_func(DEAL_FUN_ARG)
{

	gf_swap_action_func_in_header *p_in = PRI_IN_POS;
	gf_swap_action_func_in_item *p_item = (gf_swap_action_func_in_item*)(p_in + 1);
	//gf_swap_action_func_out_header out_header = {0};
	//gf_swap_action_func_out_item *p_out_item = (gf_swap_action_func_out_item*)(p_in + 1);

	uint32_t attire_cnt = 0;

	//check proto len
	CHECK_PRI_IN_LEN_WITHOUT_HEADER(
		(p_in->del_cnt * sizeof(gf_swap_action_func_in_item) + 
		p_in->add_cnt * sizeof(gf_swap_action_func_in_item)));
	
	//del items when finishing task
	for (uint32_t i=0;i<p_in->del_cnt;i++)
	{
		switch (p_item->type)
		{
			case TYPE_ATTIRE:
				ret = this->gf_attire.del_attire_by_attireid(USERID_ROLETM, p_item->id);
				break;
			case TYPE_ITEM:
				//ret = gf_item.reduce_item_int_value(USERID_ROLETM,p_item1->id,item_count_column,p_item1->count);
                ret = gf_reduce_item_count(USERID_ROLETM, p_item->id, p_item->count);
				if (ret == GF_ITEM_NOFIND_ERR)
					ret = GF_ITEM_NOTENOUGH_ERR;
				break;
			case TYPE_ROLE:
				if (p_item->id==TYPE_ROLE_XIAOMEE)
					ROLE_REDUCE_VALUE(XIAOMEE,RECVBUF_USERID, RECVBUF_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, p_item->count, NULL);
				else if (p_item->id==TYPE_ROLE_EXP)
					ROLE_REDUCE_VALUE(EXP,RECVBUF_USERID, RECVBUF_ROLETM, GF_EXP_NOENOUGH_ERR, p_item->count, NULL);
                else if (p_item->id==TYPE_ROLE_FUMOPOINT)
					ROLE_REDUCE_VALUE(FUMO_POINTS,RECVBUF_USERID, RECVBUF_ROLETM, GF_FUMO_NOENOUGH_ERR, p_item->count, NULL);

				else if (p_item->id==TYPE_ROLE_PVP_FIGHT)
					ROLE_REDUCE_VALUE(EXPLOIT,RECVBUF_USERID,RECVBUF_ROLETM,GF_ITEM_NOTENOUGH_ERR, p_item->count, NULL);
				else
				{
					STD_ROLLBACK();
					return ENUM_OUT_OF_RANGE_ERR;
				}
				break;
			default:
				STD_ROLLBACK();
				return ENUM_OUT_OF_RANGE_ERR;
				break;
		}
		
		if (ret!=SUCC)
		{
			STD_ROLLBACK();
			return ret;			
		}
		p_item ++;
	}

    //uint32_t max_limit = 0;
	//add items when finishing task
	for (uint32_t i = 0; i < p_in->add_cnt; i++)
	{
		DEBUG_LOG("%u %u", i, p_in->add_cnt);
		switch (p_item->type)
		{
			case TYPE_ATTIRE:
				
				for (uint32_t j=0;j<p_item->count;j++)
				{
					m_attire_elem.attireid = p_item->id;
					m_attire_elem.gettime = time(NULL);
					m_attire_elem.attire_rank = 1;
					m_attire_elem.duration = p_item->duration;
                    m_attire_elem.endtime = 0;
					m_attire_elem.usedflag = 0;
					ret = this->gf_attire.add_attire(USERID_ROLETM, &m_attire_elem, MAX_ATTIRE_CNT, &(p_item->uniqe_id));
					if (ret!=SUCC)
					{
						STD_ROLLBACK();
						return ret;			
					}
				}
				attire_cnt ++;
				break;
			case TYPE_ITEM:
                ret = gf_increase_item(USERID_ROLETM,p_item->id,p_item->count,set_item_limit(p_item->id),DEFAULT_MAX_ITEM_GRID_CNT);
				break;
			case TYPE_ROLE:
                
				if (p_item->id==TYPE_ROLE_XIAOMEE)
					ret = this->gf_role.increase_int_value(USERID_ROLETM,
						ROLE_XIAOMEE_COLUMN, p_item->count);
				else if (p_item->id==TYPE_ROLE_EXP)
					ret = this->gf_role.increase_int_value(USERID_ROLETM, 
						ROLE_EXP_COLUMN, p_item->count);
				else if (p_item->id==TYPE_ROLE_SKILLPOINT)
					ret = this->gf_role.increase_int_value(USERID_ROLETM, 
						ROLE_SKILL_POINT_COLUMN, p_item->count);
				else if (p_item->id==TYPE_ROLE_FUMOPOINT)
					ret = this->gf_role.increase_int_value(USERID_ROLETM, 
						ROLE_FUMO_POINTS_COLUMN, p_item->count);
				else if (p_item->id==TYPE_ROLE_PVP_FIGHT)
                    ret = this->gf_role.increase_int_value(USERID_ROLETM,
                        ROLE_EXPLOIT_COLUMN, p_item->count);
				else if (p_item->id==TYPE_ROLE_HONOR)
					ret = this->gf_role.increase_int_value(USERID_ROLETM,
                        ROLE_HONOR_COLUMN, p_item->count);
                else if (p_item->id==TYPE_ROLE_YAOSHI_EXP)
                    ret = this->gf_secondary_pro.add_player_secondary_pro_exp(USERID_ROLETM,
                        1, p_item->count, DEFAULT_MAX_SECONDARY_PRO_EXP);
				else if (p_item->id == TYPE_ROLE_ALLOCATOR_EXP)
					ret = this->gf_role.increase_int_value(USERID_ROLETM, ROLE_ALLOCATOR_EXP_COLUMN, p_item->count);
                else if (p_item->id == TYPE_ROLE_SWORD_VALUE)
                    ret = this->gongfu.set_int_add_value(RECVBUF_USERID, p_item->count, "sword_value");
                else if (p_item->id == TYPE_ROLE_VIP_QUALIFY)
                    ret = SUCC;
				else
				{
					STD_ROLLBACK();
					return ENUM_OUT_OF_RANGE_ERR;
				}
				break;
			case TYPE_SKILL:
				ret = this->gf_skill.add_skill(USERID_ROLETM, p_item->id, 1);
				if (ret != SUCC) {
					STD_ROLLBACK();
					return ret;	
				}
			default:
				STD_ROLLBACK();
                DEBUG_LOG("there %u ", p_item->type);
				return ENUM_OUT_OF_RANGE_ERR;
				break;
		}
		
		if (ret!=SUCC)
		{
			STD_ROLLBACK();
			return ret;			
		}
		p_item ++;

	}

	if (p_in->user_flg) {
		ret = this->gf_swap_action.increase_action_count(RECVBUF_USERID, 0, p_in->id, p_in->type, p_in->add_times);
	} else {
		ret = this->gf_swap_action.increase_action_count(USERID_ROLETM, p_in->id, p_in->type, p_in->add_times);
	}
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;			
	}
	STD_RETURN_WITH_BUF(ret, p_in, sizeof(gf_swap_action_func_out_header) + (p_in->add_cnt + p_in->del_cnt) * sizeof(gf_swap_action_func_out_item));
}

int Croute_func::gf_set_client_buf(DEAL_FUN_ARG)
{
	gf_set_client_buf_in* p_in = PRI_IN_POS;
	ret = this->gf_role.set_client_buf(USERID_ROLETM, p_in->buf);
	if (ret!=SUCC) {
		STD_ROLLBACK();
		return ret;			
	}
	STD_RETURN(ret);
}

int Croute_func::gf_get_client_buf(DEAL_FUN_ARG)
{
	gf_get_client_buf_out out = {{0}};
	ret = this->gf_role.get_client_buf(USERID_ROLETM, out.buf);
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_get_plant_list(DEAL_FUN_ARG)
{
	gf_get_plant_list_out_header out_header = { 0 };
	gf_get_plant_list_out_item* p_out_item = 0;

	this->gf_plant.del_plant(USERID_ROLETM, 0);
	ret = this->gf_plant.get_plant_list(USERID_ROLETM, &(out_header.count), &p_out_item);
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}


int Croute_func::gf_set_plant_status(DEAL_FUN_ARG)
{
	gf_set_plant_status_in* p_in = PRI_IN_POS;
	gf_set_plant_status_out out = { 0 };
	if (p_in->status) {
		ret = this->gf_plant.set_plant_status(USERID_ROLETM,p_in);
	} else {
		ret = this->gf_plant.del_plant(USERID_ROLETM,p_in->field_id);
	}
	memcpy(&out,p_in,sizeof(out));
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_add_effect_to_all_plants(DEAL_FUN_ARG)
{
	gf_add_effect_to_all_plants_in* p_in = PRI_IN_POS;
	
	ret = this->gf_plant.add_effect_to_all_plants(USERID_ROLETM, p_in);
	STD_RETURN(ret);
}

int Croute_func::gf_add_home_log(DEAL_FUN_ARG)
{
	gf_add_home_log_in* p_in = PRI_IN_POS;
	
	ret = this->gf_home_log.add_log(USERID_ROLETM, p_in);
	this->gf_home_log.clear_log(USERID_ROLETM);
	STD_RETURN(ret);
}

int Croute_func::gf_get_home_log(DEAL_FUN_ARG)
{
	gf_get_home_log_out_header out_header = { 0 };
	gf_get_home_log_out_item* p_out_item = NULL;

	ret = this->gf_home_log.get_home_log(USERID_ROLETM, &(out_header.count), &p_out_item);
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::gf_pick_fruit(DEAL_FUN_ARG)
{
	gf_pick_fruit_in_header* p_in = PRI_IN_POS;
	gf_pick_fruit_in_item *p_item = (gf_pick_fruit_in_item*)(p_in + 1);
    
    //ret = 0;

	if (p_in->fruit_id) {
		ret = gf_increase_item(USERID_ROLETM, p_in->fruit_id, 1, 999999, 999999);
		if (ret != SUCC) {
	        STD_ROLLBACK();
	        return ret;
	    }
	}

	//del items when finishing task
	for (uint32_t i=0;i<p_in->cost_cnt;i++)
	{
		switch (p_item->type)
		{
			case TYPE_ATTIRE:
				ret = this->gf_attire.del_attire_by_attireid(USERID_ROLETM, p_item->id);
				break;
			case TYPE_ITEM:
				//ret = gf_item.reduce_item_int_value(USERID_ROLETM,p_item1->id,item_count_column,p_item1->count);
                ret = gf_reduce_item_count(USERID_ROLETM, p_item->id, p_item->count);
				if (ret == GF_ITEM_NOFIND_ERR)
					ret = GF_ITEM_NOTENOUGH_ERR;
				break;
			case TYPE_ROLE:
				if (p_item->id==TYPE_ROLE_XIAOMEE)
					ROLE_REDUCE_VALUE(XIAOMEE,RECVBUF_USERID, RECVBUF_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, p_item->count, NULL);
				else if (p_item->id==TYPE_ROLE_EXP)
					ROLE_REDUCE_VALUE(EXP,RECVBUF_USERID, RECVBUF_ROLETM, GF_EXP_NOENOUGH_ERR, p_item->count, NULL);
                else if (p_item->id==TYPE_ROLE_FUMOPOINT)
					ROLE_REDUCE_VALUE(FUMO_POINTS,RECVBUF_USERID, RECVBUF_ROLETM, GF_FUMO_NOENOUGH_ERR, p_item->count, NULL);

				else if (p_item->id==TYPE_ROLE_PVP_FIGHT)
					ROLE_REDUCE_VALUE(EXPLOIT,RECVBUF_USERID,RECVBUF_ROLETM,GF_ITEM_NOTENOUGH_ERR, p_item->count, NULL);
				else
				{
					STD_ROLLBACK();
					return ENUM_OUT_OF_RANGE_ERR;
				}
				break;
			default:
				STD_ROLLBACK();
				return ENUM_OUT_OF_RANGE_ERR;
				break;
		}
		
		if (ret!=SUCC)
		{
			STD_ROLLBACK();
			return ret;			
		}
		p_item ++;
	}


	STD_RETURN_WITH_BUF(ret, p_in, 
		sizeof(gf_pick_fruit_out_header) + p_in->cost_cnt * sizeof(gf_pick_fruit_out_item));
}

int Croute_func::gf_team_member_reward(DEAL_FUN_ARG)
{
    char out[PROTO_MAX_SIZE] = {0};
    gf_team_member_reward_in_header *p_in = PRI_IN_POS;
	gf_team_member_reward_in_item *p_item = (gf_team_member_reward_in_item*)(p_in + 1);

    CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->count * sizeof(gf_team_member_reward_in_item));

    gf_team_member_reward_out_header* p_out_header = (gf_team_member_reward_out_header*)out;
    p_out_header->win_stage = p_in->win_stage;
    p_out_header->count = p_in->count;
    char* p_offset = &(out[0]) + sizeof(*p_out_header);

    gf_team_member_reward_out_item out_item = {0};
    for (uint32_t i = 0; i < p_in->count; i++) {
        switch (p_item->type) {
            case TYPE_ITEM:
                ret = gf_increase_item(USERID_ROLETM,p_item->id,p_item->cnt,set_item_limit(p_item->id),DEFAULT_MAX_ITEM_GRID_CNT);
                out_item.id = p_item->id;
                out_item.cnt = p_item->cnt;
                break;
            case TYPE_ROLE:
                if (p_item->id==TYPE_ROLE_XIAOMEE) {
                    ret = this->gf_role.increase_int_value(USERID_ROLETM,
                        ROLE_XIAOMEE_COLUMN, p_item->cnt);
                }
                else if (p_item->id==TYPE_ROLE_EXP)
                    ret = this->gf_role.increase_int_value(USERID_ROLETM, 
                        ROLE_EXP_COLUMN, p_item->cnt);
                out_item.id = p_item->id;
                out_item.cnt = p_item->cnt;
                break;
            default:
                DEBUG_LOG("there %u ", p_item->type);
                return ENUM_OUT_OF_RANGE_ERR;
                break;
        }
        if (ret!=SUCC) {
			STD_ROLLBACK();
			return ret;			
		}
		p_item ++;

        memcpy(p_offset, &(out_item), sizeof(gf_team_member_reward_out_item));
        p_offset += sizeof(gf_team_member_reward_out_item);
    }
    STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0])); 
}

int Croute_func::gf_get_summon_dragon_list(DEAL_FUN_ARG)
{
	gf_get_summon_dragon_list_out_header out_header = { 0 };
	gf_get_summon_dragon_list_out_item* p_out_item = 0;
	ret = this->gf_summon_dragon_list.get_summon_dragon_list(100, &p_out_item, &(out_header.count));
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);	
}

int Croute_func::gf_add_summon_dragon(DEAL_FUN_ARG)
{
    gf_add_summon_dragon_in* p_in = PRI_IN_POS;
    ret = this->gf_summon_dragon_list.add_summon_dragon(p_in);
    if (ret != SUCC) {
        return ret;
    }
	STD_RETURN(ret);
}

int Croute_func::gf_get_card_list(DEAL_FUN_ARG) {

	gf_get_card_list_out_header out_header = {0};
	gf_get_card_list_out_item * p_list = 0;

	ret = this->gf_card.get_card_list(USERID_ROLETM, &p_list, &(out_header.count));
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_list, out_header.count);	
}

int Croute_func::gf_insert_card(DEAL_FUN_ARG) {
	gf_insert_card_in * p_in = PRI_IN_POS;
	gf_insert_card_out out = {0};
	ret = gf_item.reduce_item_int_value(RECVBUF_USERID, RECVBUF_ROLETM, p_in->item_id,
				   	"count", 1);
	if (ret != SUCC) {
		STD_ROLLBACK();
		return ret;
	}

	ret = gf_card.insert_card(USERID_ROLETM, p_in->card_type, p_in->card_set, &(out.card_id));
	if (ret != SUCC) {
		STD_ROLLBACK();
		return ret;
	}

	out.card_type = p_in->card_type;
	out.card_set = p_in->card_set;
	out.item_id = p_in->item_id;

	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_erase_card(DEAL_FUN_ARG) {
	return 0;
}

int Croute_func::gf_fresh_summon_attr(DEAL_FUN_ARG) {
	gf_fresh_summon_attr_in * p_in = PRI_IN_POS;
	ret = gf_item.reduce_item_int_value(USERID_ROLETM, p_in->item_id,
									"count", 1);
	if (ret != SUCC) {
		STD_ROLLBACK();
		return ret;
	}
//cws 0513
    DEBUG_LOG("4cws test db");


	uint32_t new_type = p_in->mon_type;
	if (p_in->mon_type % 10 != 2) {
		new_type = (new_type / 10) * 10 + 2; 
	}

	ret = gf_summon_skill.clear_summon_skill(USERID_ROLETM, p_in->mon_tm);

    if (p_in->vip_sum) {
        ret = gf_summon.fresh_summon_attr_add_ex(USERID_ROLETM, 
            p_in->mon_tm, p_in->mon_lv, p_in->mon_exp, p_in->attr_per);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    } else {
        ret = gf_summon.fresh_summon_attr_add(USERID_ROLETM, 
            p_in->mon_tm, new_type, p_in->attr_per);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }
	STD_RETURN_WITH_STRUCT(ret, *p_in);
}
	
int Croute_func::gf_get_offline_info(DEAL_FUN_ARG)
{
	gf_get_offline_info_out out = {0};

	ret = gf_role.get_role_offline_info(USERID_ROLETM, &out);
	//玩家有宠物挂机状态标识， 却没有在挂机状态的宠物
	if (out.summon_off_tm) {
		uint32_t off_summon_tm = 0;
		gf_summon.get_offline_summon(USERID_ROLETM, &off_summon_tm);
		if (!off_summon_tm) {
			gf_role.set_int_value(USERID_ROLETM, "summon_off_tm", 0);
			out.summon_off_tm = 0;
		}
	}

	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_set_offline_info(DEAL_FUN_ARG)
{
	gf_set_offline_info_in *p_in = PRI_IN_POS;
	ret = gf_role.set_role_offline_info(USERID_ROLETM, p_in);
	if (ret)
	{
		STD_ROLLBACK();
		return ret;
	}
    if (p_in->reduce_coins > 0) {
        uint32_t left = 0;
        gf_role.reduce_int_value(USERID_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, "xiaomee", p_in->reduce_coins, &left);
        if (ret != SUCC) 
        {
            STD_ROLLBACK();
            return ret;
        }
    }

    if (p_in->reduce_fumo > 0) {
        ROLE_REDUCE_VALUE(FUMO_POINTS,RECVBUF_USERID, RECVBUF_ROLETM, GF_FUMO_NOENOUGH_ERR, p_in->reduce_fumo, NULL);
        if (ret != SUCC) 
        {
            STD_ROLLBACK();
            return ret;
        }
    }
	
	uint32_t left_mon_exp = 0;
	if (p_in->mon_tm) 
	{
		if (p_in->summon_end_flag) {
			ret = this->gf_summon.increase_int_value(USERID_ROLETM,p_in->mon_tm,"exp",p_in->mon_add_exp, 0xffffffff, &(left_mon_exp));
	 	  	if (ret != SUCC) 
			{
        		STD_ROLLBACK();
	       		return ret;
    		}
			ret = this->gf_summon.increase_int_value(USERID_ROLETM,p_in->mon_tm,"lv",p_in->mon_add_lv, 100, &(left_mon_exp));
	   		if (ret != SUCC) 
			{
        		STD_ROLLBACK();
	       		return ret;
    		}
			ret = this->gf_summon.set_summon_status(USERID_ROLETM, p_in->mon_tm, 3);
			if (ret != SUCC) 
			{
				STD_ROLLBACK();
				return ret;
			}
		} else { 
			ret = this->gf_summon.set_summon_status(USERID_ROLETM, p_in->mon_tm, 4);
			if (ret != SUCC) 
			{
				STD_ROLLBACK();
				return ret;
			}
		}
	}

	STD_RETURN(ret);
}


int Croute_func::gf_create_fight_team(DEAL_FUN_ARG)
{
	gf_create_fight_team_in *p_in = PRI_IN_POS;
	gf_create_fight_team_out out = {0};

	
	ret = gf_team.create_team(USERID_ROLETM, p_in->team_name, p_in->captain_name, &(out.team_id));
	if (ret != SUCC) 
	{
		STD_ROLLBACK();
		return ret;
	}
	ret = gf_team_detail.add_team_member(USERID_ROLETM, out.team_id);
	if (ret != SUCC) 
	{
		STD_ROLLBACK();
		return ret;
	}

	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_add_fight_team_member(DEAL_FUN_ARG)
{
	gf_add_fight_team_member_in *p_in = PRI_IN_POS;
	gf_add_fight_team_member_out_header out_header = {0};
	gf_add_fight_team_member_out_item* p_list = 0;

	
	ret = gf_team.increase_team_member_count(p_in->team_id);
	if (ret != SUCC) 
	{
		STD_ROLLBACK();
		return ret;
	}

	ret = gf_team_detail.add_team_member(USERID_ROLETM, p_in->team_id);
	if (ret != SUCC) 
	{
		STD_ROLLBACK();
		return ret;
	}

	ret = gf_team_detail.get_team_member_list(p_in->team_id, &(out_header.member_cnt), &p_list);
	if (ret != SUCC) 
	{
		STD_ROLLBACK();
		return ret;
	}

	out_header.team_id = p_in->team_id;
	out_header.inviter = p_in->inviter;
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_list, out_header.member_cnt);	
}

int Croute_func::gf_set_player_team_flg(DEAL_FUN_ARG)
{
	gf_set_player_team_flg_in *p_in = PRI_IN_POS;
	gf_set_player_team_flg_out out = {0};

	
	ret = gf_role.set_int_value(USERID_ROLETM, "team_id", p_in->team_id);
	if (ret != SUCC) 
	{
		STD_ROLLBACK();
		return ret;
	}

	out.team_id = p_in->team_id;
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_set_team_member_level(DEAL_FUN_ARG)
{
	gf_set_team_member_level_in * p_in = PRI_IN_POS;
	ret = gf_team_detail.set_team_member_level(p_in);
	return 0;
}

int Croute_func::gf_change_team_name(DEAL_FUN_ARG)
{
	gf_change_team_name_in * p_in = PRI_IN_POS;
	ret = gf_team.change_team_name(p_in);
	return 0;
}

int Croute_func::gf_change_team_mcast(DEAL_FUN_ARG)
{
	gf_change_team_mcast_in * p_in = PRI_IN_POS;
	ret = gf_team.change_team_mcast(p_in);
	return 0;
}


int Croute_func::gf_get_team_info(DEAL_FUN_ARG)
{
	gf_get_team_info_in *p_in = PRI_IN_POS;
	gf_get_team_info_out_header out_header = {0};
	gf_get_team_info_out_item * p_list = 0;

	uint32_t score_index = 0xffffffff;
	ret = gf_team.get_team_info(p_in->team_id , &out_header);
	if (ret != SUCC) 
	{
		STD_ROLLBACK();
		return ret;
	}
	score_index = out_header.score_index;
	if (!(out_header.score_index)) {
		uint32_t top_cnt = 0;
		gf_get_team_top100_out_item* p_top_100 = 0;
		ret = gf_team.get_team_top100(&top_cnt, &p_top_100);
		if (ret != SUCC) 
		{
			STD_ROLLBACK();
			if (p_top_100) {
				free(p_top_100);p_top_100 = 0;
			}
			return ret;
		}
		for (uint32_t i = 0; i < top_cnt; i++) {
			gf_get_team_top100_out_item* p_tmp = p_top_100 + i;
			if (p_tmp->teamid == p_in->team_id) {
				score_index = i + 1;
				break;
			}
		}
		if (score_index == 0) {
			score_index = 0xffffffff;
		}
		ret = gf_team.set_team_int_value(p_in->team_id, "score_index", score_index);
		if (ret != SUCC) 
		{
			STD_ROLLBACK();
			return ret;
		}
		if (p_top_100) {
			free(p_top_100);p_top_100 = 0;
		}
	}
	out_header.score_index = score_index;
	ret = gf_team_detail.get_team_member_list(p_in->team_id, &(out_header.member_cnt), &p_list);
	if (ret != SUCC) 
	{
		STD_ROLLBACK();
		return ret;
	}

	out_header.team_id = p_in->team_id;
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_list, out_header.member_cnt);	
}

int Croute_func::gf_search_team_info(DEAL_FUN_ARG)
{
	gf_search_team_info_in * p_in = PRI_IN_POS;

	gf_search_team_info_out_header out_header = {0};
	gf_search_team_info_out_item * p_list = 0;
	ret = gf_team.search_team_info(p_in->team_id, p_in->captain_id, &(out_header.team_cnt), &p_list);
	if (ret != SUCC) {
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_list, out_header.team_cnt);	
}

int Croute_func::gf_gain_team_exp(DEAL_FUN_ARG)
{
	gf_gain_team_exp_in * p_in = PRI_IN_POS;
	gf_gain_team_exp_out out = {0};
	ret = gf_team.increase_team_int_value(p_in->team_id, "team_exp", p_in->gain_exp, &out.new_exp);
	if (ret != SUCC) {
		return ret;
	}

	if (RECVBUF_USERID) {
	    ret = this->gf_team_detail.increase_team_member_int_value(p_in->team_id, USERID_ROLETM, p_in->gain_exp, "team_exp");
	    if (ret != SUCC) {
	        STD_ROLLBACK();
	        return ret;
	    }
	}
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_del_team_member(DEAL_FUN_ARG)
{
	gf_del_team_member_in *p_in = PRI_IN_POS;
	gf_del_team_member_out out = {0};
	
	ret = gf_team.reduce_team_member_count(p_in->team_id);
	if (ret != SUCC) 
	{
		STD_ROLLBACK();
		return ret;
	}

	ret = gf_team_detail.del_team_member(p_in->del_uid, p_in->del_role_tm, p_in->team_id);
	if (ret != SUCC) 
	{
		STD_ROLLBACK();
		return ret;
	}

	out.team_id = p_in->team_id;
	out.del_uid = p_in->del_uid;
	out.del_role_tm = p_in->del_role_tm;
	STD_RETURN_WITH_STRUCT(ret, out);	
}

int Croute_func::gf_del_team(DEAL_FUN_ARG)
{
	gf_del_team_in *p_in = PRI_IN_POS;
	
	ret = gf_team.del_team(USERID_ROLETM, p_in->team_id);
	if (ret != SUCC) 
	{
		STD_ROLLBACK();
		return ret;
	}

	ret = gf_team_detail.del_team(p_in->team_id);
	if (ret != SUCC) 
	{
		STD_ROLLBACK();
		return ret;
	}

	//out.team_id = p_in->team_id;
	STD_RETURN(ret);	
}

int Croute_func::gf_team_info_changed(DEAL_FUN_ARG)
{
	gf_team_info_changed_in *p_in = PRI_IN_POS;

	if (p_in->type) {
		ret = gf_team.increase_score(p_in->team_id, p_in->score);
		if (ret != SUCC) 
		{
			STD_ROLLBACK();
			return ret;
		}
	} else {
		ret = gf_team.reduce_score(p_in->team_id, p_in->score);
		if (ret != SUCC) 
		{
			STD_ROLLBACK();
			return ret;
		}
	}
	return ret;
}

int Croute_func::gf_team_active_score_change(DEAL_FUN_ARG)
{
    gf_team_active_score_change_in * p_in = PRI_IN_POS;

    if (p_in->type == 1) {
        ret = this->gf_team.increase_active_score(p_in->teamid, p_in->active_flag, p_in->score);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    } else if (p_in->type == 2) {
        ret = this->gf_team.reduce_active_score(p_in->teamid, p_in->active_flag, p_in->score);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }
    return ret;
}

int Croute_func::gf_get_team_top10(DEAL_FUN_ARG)
{
	gf_get_team_top10_in *p_in = PRI_IN_POS;
	gf_get_team_top10_out_header out_header = {0};
	gf_get_team_top10_out_item * p_list = 0;

	ret = gf_team.get_team_top10(&(out_header.cnt), &p_list);
	if (ret != SUCC) 
	{
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_list, out_header.cnt);	
}

int Croute_func::gf_contribute_team(DEAL_FUN_ARG)
{
    gf_contribute_team_in *p_in = PRI_IN_POS;
    gf_contribute_team_out out = {0};
    //ret = this->gf_role.reduce_int_value(USERID_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, 
	//	ROLE_XIAOMEE_COLUMN, p_in->coin, &(out.player_coin));
	//if (ret != SUCC) {
	//	STD_ROLLBACK();
	//	return ret;
	//}
    
    ret = gf_team.increase_team_int_value(p_in->teamid, "coin", p_in->coin, &(out.team_coin));
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }

    ret = this->gf_team_detail.increase_team_member_int_value(p_in->teamid, USERID_ROLETM, p_in->coin, "team_coins");
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }

    out.teamid = p_in->teamid;
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_team_reduce_tax(DEAL_FUN_ARG)
{
	gf_team_reduce_tax_in * p_in = PRI_IN_POS;
	gf_team_reduce_tax_out out = {0};
	out.team_id = p_in->team_id;
	uint32_t last_tax_time = 0;
	uint32_t team_coin = 0;

	ret = gf_team.get_team_last_tax_info(p_in->team_id, &last_tax_time, &team_coin);

	if (!is_same_day(p_in->update_tm, last_tax_time) && team_coin >= p_in->reduce_coin) {
		out.reduce_coin = p_in->reduce_coin;
		out.update_tm = p_in->update_tm;
		gf_team.reduce_team_tax_coin(p_in->team_id, team_coin - out.reduce_coin, p_in->update_tm);
	}
//	ERROR_LOG("OUT INFO %u %u %u", out.team_id, out.reduce_coin, out.update_tm);
	STD_RETURN_WITH_STRUCT(ret, out);
}
	

int Croute_func::gf_fetch_team_coin(DEAL_FUN_ARG)
{
    gf_fetch_team_coin_in *p_in = PRI_IN_POS;
    gf_fetch_team_coin_out out = {0};

    ret = gf_team.reduce_team_int_value(p_in->teamid, GF_XIAOMEE_NOENOUGH_ERR, 
        "coin", p_in->coin, &(out.team_coin));
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }

    //ret = gf_role.increase_int_value_with_ret(USERID_ROLETM, ROLE_XIAOMEE_COLUMN, 
    //    p_in->coin, &(out.player_coin));
    //if (ret != SUCC) {
    //    STD_ROLLBACK();
    //    return ret;
    //}

    out.teamid = p_in->teamid;
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_team_active_enter(DEAL_FUN_ARG)
{
    gf_team_active_enter_in *p_in = PRI_IN_POS;
    gf_team_active_enter_out out = {0};

    if (p_in->need_coin > 0) {
        ret = gf_team.reduce_team_int_value(p_in->teamid, GF_XIAOMEE_NOENOUGH_ERR,
            "coin", p_in->need_coin, &(out.need_coin));
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }

    ret = gf_team.set_team_int_value(p_in->teamid, "active_flag", p_in->active_flag);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }

    out.teamid = p_in->teamid;
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_get_team_active_top100(DEAL_FUN_ARG)
{
    gf_get_team_active_top100_in *p_in = PRI_IN_POS;
    gf_get_team_active_top100_out_header out_header = {0};
	gf_get_team_active_top100_out_item * p_list = 0;

    out_header.type = p_in->type;
	ret = gf_team.get_team_active_top100(p_in->type, &(out_header.cnt), &p_list);
	if (ret != SUCC) {
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_list, out_header.cnt);	
}

int Croute_func::gf_list_true_wusheng_info(DEAL_FUN_ARG)
{
	gf_list_true_wusheng_info_out_header out_header = {0};
	gf_list_true_wusheng_info_out_item * plist = 0;

	ret = gf_wuseng_info.list_wuseng_pass_info(&plist, &out_header.cnt);
	if (ret != SUCC) {
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, plist, out_header.cnt);
}

int Croute_func::gf_set_true_wusheng_info(DEAL_FUN_ARG)
{
	gf_set_true_wusheng_info_in *p_in = PRI_IN_POS;
	uint32_t quicker_count = 0;
	ret = gf_wuseng_info.get_quicker_pass(p_in->pass_time, &quicker_count);
	if (ret != SUCC) {
		return ret;
	}

	uint32_t pass_time = 0;
	ret = gf_wuseng_info.get_user_pass_info(p_in->uid, p_in->role_tm, &pass_time);

//	ERROR_LOG("wuseng %u %u %u", quicker_count, p_in->pass_time, pass_time);

	if (quicker_count < 10) {
		if (pass_time == 0 || 
				(pass_time && p_in->pass_time < pass_time)) {
			ret = gf_wuseng_info.set_wuseng_pass_info(p_in);

			if (ret != SUCC) {
				STD_ROLLBACK();
			}
		} 
	}
	return 0;
}

int Croute_func::gf_get_other_info_list(DEAL_FUN_ARG)
{
    gf_get_other_info_list_out_header out_header = {0};
	gf_get_other_info_list_out_item * p_list = 0;

	ret = gf_other_info.get_other_info_list(USERID_ROLETM, &p_list, &(out_header.cnt));
	if (ret != SUCC) {
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_list, out_header.cnt);
}


int Croute_func::gf_set_other_info(DEAL_FUN_ARG)
{
    gf_set_other_info_in *p_in = PRI_IN_POS;

	ret = gf_other_info.replace_other_info(USERID_ROLETM, p_in->type, p_in->value);
	if (ret != SUCC) {
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN(ret);
}


//for ranker
int Croute_func::gf_list_ranker_info(DEAL_FUN_ARG)
{
	gf_list_ranker_info_in * p_in = PRI_IN_POS;
	gf_list_ranker_info_out_header out_header = {0};
	out_header.ranker_id = p_in->ranker_id;
	gf_list_ranker_info_out_item * item = NULL;

	
	ret = gf_ranker.list_ranker_info(p_in->ranker_id,
									 p_in->range_type,
									 p_in->ranker_range,
									 &item,
									 &out_header.info_count);
	if (ret != SUCC) {
		return ret;
	}

	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, item, out_header.info_count);

}

int Croute_func::gf_set_ranker_info(DEAL_FUN_ARG)
{
	gf_set_ranker_info_in * p_in = PRI_IN_POS;

	uint32_t better_count = 0;

	gf_ranker.get_better_count(p_in->ranker_id,
			                   p_in->range_type,
							   p_in->score,
							   &better_count);

	if (!better_count || better_count < max_ranker_range) {
		uint32_t old_score = 0;
		gf_ranker.get_user_old_score(p_in->ranker_id,
									 p_in->userid,
									 p_in->role_tm,
									 &old_score);
	
		if (!old_score) {
			gf_ranker.save_user_ranker_info(p_in);
		} else if (p_in->range_type == range_type_time  && 
				   p_in->score < old_score) {
			gf_ranker.save_user_ranker_info(p_in);
		} else if (p_in->range_type == range_type_score &&
				   p_in->score > old_score) {
			gf_ranker.save_user_ranker_info(p_in);
		}

		
		uint32_t total_count = 0;
		gf_ranker.get_total_count(p_in->ranker_id, &total_count);
		if (total_count > max_ranker_range) {
			gf_ranker.del_worst_info(p_in->ranker_id, p_in->range_type);
		}
	}
	return 0;
}

int Croute_func::gf_set_power_user(DEAL_FUN_ARG)
{
    gf_set_power_user_in *p_in = PRI_IN_POS;
    gf_set_power_user_out out = {0};
	ret = this->gf_skill.clear_skills(USERID_ROLETM);
	if (ret!=SUCC) {
		STD_ROLLBACK();
		return ret;
	}
	ret = this->gf_role.update_skillbind(USERID_ROLETM, "", 0);
	if (ret!=SUCC) {
		STD_ROLLBACK();
		return ret;
	}
	ret = this->gf_role.increase_int_value_with_ret(USERID_ROLETM, ROLE_SKILL_POINT_COLUMN, p_in->skill_point, &(out.skill_point));
	if (ret!=SUCC) {
		STD_ROLLBACK();
		return ret;
	}

    ret = this->gf_role.set_int_value(USERID_ROLETM, "power_user", p_in->power_user);
    if (ret!=SUCC) {
        STD_ROLLBACK();
        return ret;
    }

    ////------add item---------
    ret = this->gf_increase_item(USERID_ROLETM, p_in->item_id_1, 1, set_item_limit(p_in->item_id_1), 999);
    if (ret!=SUCC) {
         STD_ROLLBACK();
         return ret;
    }
    ret = this->gf_increase_item(USERID_ROLETM, p_in->item_id_2, 1, set_item_limit(p_in->item_id_2), 999);
    if (ret!=SUCC) {
         STD_ROLLBACK();
         return ret;
    }


    out.power_user  = p_in->power_user;
    out.item_id_1   = p_in->item_id_1;
    out.item_id_2   = p_in->item_id_2;
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_get_master(DEAL_FUN_ARG)
{
    char out[PROTO_MAX_SIZE] = {0}; 
    int cpy_len = 0;

    gf_get_master_out_header *p_out = (gf_get_master_out_header*)out;
    char* p_offset = &(out[0]) + sizeof(*p_out);

    gf_get_master_out_item master = {0};
    cpy_len = sizeof(gf_get_master_out_item);

    mp_member_t * plist = NULL;
    //uint32_t cnt = 0;
    ret = this->gf_master.get_member_list_by_type(USERID_ROLETM, RELATION_TYPE_MASTER, &plist, &(p_out->cnt));
    //p_out->cnt = cnt;
    DEBUG_LOG("PLAYER [ %u ] MASTER cnt [%u]", RECVBUF_USERID, p_out->cnt);
    for (uint32_t i = 0; i < p_out->cnt; i++) {
        memset(&master, 0x00, sizeof(gf_get_master_out_item));
        if (plist) {
            master.master_id   =   plist[i].uid;
            master.master_tm   =   plist[i].roletm;
			master.join_tm	   =   plist[i].tm;
            DEBUG_LOG("PLAYER [ %u ] MASTER [%u %u] %u", RECVBUF_USERID, master.master_id, master.master_tm, master.join_tm);
        }

        memcpy(p_offset, &master, cpy_len);
        p_offset += cpy_len;
    }
    if (plist) {
        free(plist);
        plist = NULL;
    }
    STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0]));
}

int Croute_func::gf_get_apprentice(DEAL_FUN_ARG)
{
    char out[PROTO_MAX_SIZE] = {0}; 
    int cpy_len = 0;
    gf_get_apprentice_out_header *p_out = (gf_get_apprentice_out_header *)(out);
    gf_get_apprentice_out_item prentice = {0};

    char* p_offset = &(out[0]) + sizeof(*p_out);
    cpy_len = sizeof(gf_get_apprentice_out_item);

    mp_member_t * plist = NULL;
    //uint32_t cnt = 0;
    ret = this->gf_master.get_member_list_by_type(USERID_ROLETM, RELATION_TYPE_PRENTICE, &plist, &(p_out->cnt));
    //p_out->cnt  = cnt;
    DEBUG_LOG("PLAYER [ %u ] prentice cnt [%u]", RECVBUF_USERID, p_out->cnt);
    for (uint32_t i = 0; i < p_out->cnt; i++) {
        memset(&prentice, 0x00, sizeof(gf_get_apprentice_out_item));
        if (plist) {
            prentice.uid    =   plist[i].uid;
            prentice.roletm =   plist[i].roletm;
            prentice.tm     =   plist[i].tm;
            prentice.lv     =   plist[i].lv;
            prentice.grade  =   plist[i].grade;
            DEBUG_LOG("PLAYER [ %u ] prentice [%u %u]", RECVBUF_USERID, prentice.uid, prentice.roletm);
        }

        memcpy(p_offset, &prentice, cpy_len);
        p_offset += cpy_len;
    }
    if (plist) {
        free(plist);
        plist = NULL;
    }
    STD_RETURN_WITH_BUF(ret, out, p_offset-&(out[0]));
}

int Croute_func::gf_master_add_apprentice(DEAL_FUN_ARG)
{
    gf_master_add_apprentice_in *p_in = PRI_IN_POS;
    gf_master_add_apprentice_out out = {0};

    memcpy(&out, p_in, sizeof(gf_master_add_apprentice_out));

    if (p_in->action == 1) {

        //gf_get_user_base_info_out_header vip_info = {0};
        gf_get_user_vip_out vip_info = {0};
        //ret = this->gongfu.get_user_base_info(RECVBUF_USERID, &vip_info);
        ret = gongfu.get_vip_info(RECVBUF_USERID, &vip_info);
        if (ret != SUCC) {
            return ret;
        }

        uint32_t vip_lv = 0;
        if (vip_info.vip_is == 1 || vip_info.vip_is == 3 || vip_info.vip_is == 9) {
            uint32_t tail_time = time(NULL);
            vip_lv = calc_player_vip_level( &vip_obj, vip_info.start_tm, tail_time, vip_info.x_value, vip_info.vip_is);
        }

        uint32_t prentice_cnt = 0;
        this->gf_master.get_prentice_cnt(USERID_ROLETM, &prentice_cnt);
        if (prentice_cnt > 10 + (vip_lv * 5)) {
            out.action = 0;
            STD_RETURN_WITH_STRUCT(ret, out);
        }
        ret = this->gf_master.add_apprentice(USERID_ROLETM, p_in);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
        this->gf_friend.add(RECVBUF_USERID, p_in->uid, 1, TYPE_FRIEND);

        STD_RETURN_WITH_STRUCT(ret, out);
    } else if (p_in->action == 2) {
        ret = this->gf_master.add_master(USERID_ROLETM, p_in->uid, p_in->roletm, p_in->tm);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }

        this->gf_friend.add(RECVBUF_USERID, p_in->uid, 1, TYPE_FRIEND);
        //-------- 标志奖励信息 ----------------
        //uint32_t cur_lv = 0;
        //this->gf_role.get_int_value(USERID_ROLETM, "level", &cur_lv);
        //if ( cur_lv >= 20 && cur_lv < 30 ) {
        //    this->gf_swap_action.insert_action(USERID_ROLETM, 1213, 4);
        //}
    }

	STD_RETURN(SUCC);
}

int Croute_func::gf_master_del_apprentice(DEAL_FUN_ARG)
{
    gf_master_del_apprentice_in *p_in = PRI_IN_POS;
    //gf_master_add_apprentice_out out = {0};

    if (p_in->action == 1) {
        ret = this->gf_master.del_apprentice(USERID_ROLETM, p_in->uid, p_in->roletm);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    } else if (p_in->action == 2) {
        ret = this->gf_master.del_master(USERID_ROLETM, p_in->uid, p_in->roletm);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }

	//STD_RETURN_WITH_STRUCT(ret, *p_in);
    STD_RETURN(SUCC);
}

int Croute_func::gf_set_prentice_grade(DEAL_FUN_ARG)
{
    gf_set_prentice_grade_in *p_in = PRI_IN_POS;
    uint32_t db_val = 0;
    ret = this->gf_master.get_prentice_grade(USERID_ROLETM, p_in->prentice_id, p_in->prentice_tm, &db_val);
    if (ret != SUCC) {
        return ret;
    }
    //uint32_t grade = p_in->is_or_operate ? (db_val | p_in->grade) : (db_val & p_in->grade);
    uint32_t grade = p_in->is_or_operate ? (db_val | p_in->grade) : (p_in->grade);
    
	ret = this->gf_master.update_grade(USERID_ROLETM, p_in->prentice_id, p_in->prentice_tm, grade);
    if (ret != SUCC) {
        return ret;
    }
    STD_RETURN(ret);
}

int Croute_func::gf_get_other_active(DEAL_FUN_ARG)
{
	gf_get_other_active_in * p_in = PRI_IN_POS;
	gf_get_other_active_out out = {0};

	out.active_id = p_in->active_id;

	ret = this->gf_other_active.get_player_other_active_info(USERID_ROLETM, &out);
	if (ret != SUCC) {
		return ret;
	}
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_set_other_active(DEAL_FUN_ARG)
{
	gf_set_other_active_in * p_in = PRI_IN_POS; 

	ret = gf_other_active.set_player_other_active_info(USERID_ROLETM, p_in);
	if (ret != SUCC) {
		return ret;
	}
	return 0;
}

int Croute_func::gf_get_other_active_list(DEAL_FUN_ARG)
{
	gf_get_other_active_list_out_header out_header  = {0};
	gf_get_other_active_list_out_item  *p_out_item = 0;
	ret = this->gf_other_active.get_other_active_list(USERID_ROLETM, &(out_header.cnt), &p_out_item);
	if (ret != SUCC) {
        STD_ROLLBACK();
    }
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.cnt);
}

int Croute_func::gf_get_single_other_active(DEAL_FUN_ARG)
{
    gf_get_single_other_active_in *p_in = PRI_IN_POS;
    gf_get_single_other_active_out out = {0};

    out.active_id = p_in->active_id;
	ret = this->gf_other_active.get_player_single_other_active(p_in->uid, p_in->rtm, &out);
	if (ret != SUCC) {
        STD_ROLLBACK();
    }
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_set_role_god_guard(DEAL_FUN_ARG)
{
	gf_set_role_god_guard_in * p_in = PRI_IN_POS; 

    if (p_in->money) {
        uint32_t left_coins = 0;
        ret = gf_role.reduce_int_value(USERID_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, "xiaomee", p_in->money, &left_coins);
        if (ret != SUCC) {
            return ret;
        }
    }
	ret = gf_role.set_role_god_guard_info(USERID_ROLETM, &(p_in->group));
	if (ret != SUCC) {
        STD_ROLLBACK();
		return ret;
	}
	STD_RETURN(SUCC);
}


int Croute_func::gf_get_prentice_cnt(DEAL_FUN_ARG)
{
    gf_get_prentice_cnt_out out = {0};
    ret = this->gf_master.get_prentice_cnt(USERID_ROLETM, &(out.cnt));
    if (ret != SUCC) {
        return ret;
    }
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_set_reward_player(DEAL_FUN_ARG)
{
    gf_set_reward_player_in *p_in = PRI_IN_POS;
    ret = gf_reward.insert_reward_player(p_in->userid, p_in->roletm, p_in->reward_id);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
    STD_RETURN(ret);
}

int Croute_func::gf_get_reward_player(DEAL_FUN_ARG)
{
    gf_get_reward_player_out_header out_header = {0};
	gf_get_reward_player_out_item * p_list = 0;

	ret = gf_reward.get_reward_player_list(&p_list, &(out_header.cnt));
	if (ret != SUCC) {
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_list, out_header.cnt);	
}

int Croute_func::gf_set_player_reward_flag(DEAL_FUN_ARG)
{
    gf_set_player_reward_flag_in *p_in = PRI_IN_POS;
    ret = gf_reward.set_reward_flag(USERID_ROLETM, p_in->key);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
    STD_RETURN(ret);
}

int Croute_func::gf_get_decorate_list(DEAL_FUN_ARG)
{
    gf_get_decorate_list_out_header out_header = {0};
	gf_get_decorate_list_out_item * p_list = 0;

	ret = gf_decorate.get_decorate_list(USERID_ROLETM, &(out_header.cnt), &p_list);
	if (ret != SUCC) {
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_list, out_header.cnt);	
}

int Croute_func::gf_set_decorate(DEAL_FUN_ARG)
{
    gf_set_decorate_in *p_in = PRI_IN_POS;
    gf_set_decorate_out out = {0};
    out.decorate_id = p_in->decorate_id;
    out.decorate_tm = time(NULL);
    out.decorate_lv = 1;
    ret = gf_decorate.set_decorate(USERID_ROLETM, p_in->decorate_id, out.decorate_tm);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_distract_clothes_strength(DEAL_FUN_ARG)
{
	gf_distract_clothes_strength_in *p_in = PRI_IN_POS;
	gf_distract_clothes_strength_out out = {0};

	if (p_in->keep_flag) {
		ret = gf_attire.set_attire_int_value(USERID_ROLETM, p_in->old_clothes_uid, "attirelv", 0);
		if (ret) {
			STD_ROLLBACK();
			return ret;
		}
	} else {
		ret = gf_attire.del_attire_with_index(USERID_ROLETM, p_in->old_clothes_uid); 
		if (ret) {
			STD_ROLLBACK();
			return ret;
		}
	}
	out.keep_flag = p_in->keep_flag;

	ret = gf_attire.set_attire_int_value(USERID_ROLETM, p_in->new_clothes_uid, "attirelv", p_in->new_level);
	if (ret) {
		STD_ROLLBACK();
		return ret;
	}

	if (p_in->stone_id) {

    	ret = gf_reduce_item_count(USERID_ROLETM, p_in->stone_id, 1);
		if (ret) {
			STD_ROLLBACK();
			return ret;
		}
	}
	uint32_t left_coin;
	ret = gf_role.reduce_int_value(USERID_ROLETM, GF_ITEM_NOTENOUGH_ERR, "xiaomee", p_in->reduce_coin, &left_coin);
	if (ret) {
		STD_ROLLBACK();
		return ret;
	}
	out.new_clothes_uid = p_in->new_clothes_uid;
	out.old_clothes_uid = p_in->old_clothes_uid;
	out.new_level = p_in->new_level;
	out.reduce_coin = p_in->reduce_coin;
	out.stone_id = p_in->stone_id;
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::gf_add_decorate_lv(DEAL_FUN_ARG)
{
    gf_add_decorate_lv_in_header *p_in = PRI_IN_POS;
    //uint32_t *del_item = p_in->del_item_arr;

    /*
    for (uint32_t i = 0; i < p_in->itemcnt; i++) {
        ret = this->gf_reduce_item_count(USERID_ROLETM, del_item[i], 1);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }
    */
    if (p_in->flag == 1) {
        ret = gf_decorate.add_decorate_lv(USERID_ROLETM, p_in->decorate_id, p_in->fillingin);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    } else if (p_in->flag == 2) {
        ret = gf_decorate.set_decorate_status(USERID_ROLETM, p_in->decorate_id, p_in->fillingin);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }
    STD_RETURN(ret);
}

int Croute_func::gf_check_update_trade_info(DEAL_FUN_ARG)
{
	gf_check_update_trade_info_in_header *p_in = PRI_IN_POS;
	gf_check_update_trade_info_in_item_1* p_item_1 = (gf_check_update_trade_info_in_item_1*)((uint8_t*)p_in + sizeof(gf_check_update_trade_info_in_header));
	gf_check_update_trade_info_in_item_2* p_item_2 = (gf_check_update_trade_info_in_item_2*)((uint8_t*)p_in + 
		sizeof(gf_check_update_trade_info_in_header) + p_in->itm_cnt * sizeof(gf_check_update_trade_info_in_item_1));

	for (uint32_t i = 0; i < p_in->itm_cnt; i++) {
		uint32_t db_cnt1 = 0;
		this->gf_item.get_item_cnt_by_itemid(USERID_ROLETM, &db_cnt1, p_item_1->item_id);
		uint32_t db_cnt2 = 0;
		this->gf_material.get_material_cnt_by_id(USERID_ROLETM, &db_cnt2, p_item_1->item_id);
		if ((db_cnt2 + db_cnt1) < p_item_1->item_cnt) {
			ERROR_LOG("check update trade err %u %u %u %u %u", USERID_ROLETM, db_cnt1, db_cnt2, p_item_1->item_cnt);
			STD_RETURN(SYS_ERR);
		}
		p_item_1++;
	}
	for (uint32_t i = 0; i < p_in->attire_cnt; i++) {
		uint32_t db_cnt1 = 0;
		this->gf_attire.get_attire_cnt_by_unique_id(USERID_ROLETM, &db_cnt1, p_item_2->unique_id);
		if (db_cnt1 != 1) {
			ERROR_LOG("check update trade attireerr %u %u %u %u", USERID_ROLETM, db_cnt1, p_item_1->item_cnt);
			STD_RETURN(SYS_ERR);
		}
		p_item_2++;
	}
	STD_RETURN(SUCC);
}

int Croute_func::gf_safe_trade_item(DEAL_FUN_ARG)
{
	gf_safe_trade_item_in_header *p_in = PRI_IN_POS;
	gf_safe_trade_item_in_item1* p_item_1 = (gf_safe_trade_item_in_item1*)((uint8_t*)p_in + sizeof(gf_safe_trade_item_in_header));
	gf_safe_trade_item_in_item2* p_item_2 = (gf_safe_trade_item_in_item2*)((uint8_t*)p_in + 
		sizeof(gf_safe_trade_item_in_header) + p_in->item_cnt * sizeof(gf_safe_trade_item_in_item1));

    uint32_t left_coins = 0;
	ret= SUCC;
    if (p_in->action == 1 && p_in->xiaomee != 0) {
        ret = this->gf_role.reduce_int_value(USERID_ROLETM, GF_XIAOMEE_NOENOUGH_ERR, "xiaomee", p_in->xiaomee, &left_coins);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    } else if (p_in->action == 2 && p_in->xiaomee != 0) {
        ret = this->gf_role.increase_int_value(USERID_ROLETM, "xiaomee", p_in->xiaomee); 
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }

	for (uint32_t i = 0; i < p_in->item_cnt; i++) {
        if (p_in->action == 1) {
            ret = this->gf_reduce_item_count(USERID_ROLETM, p_item_1->item_id, p_item_1->item_cnt);
            if (ret != SUCC) {
                STD_ROLLBACK();
                return ret;
            }
        } else if (p_in->action == 2) {
            ret = this->gf_increase_item(USERID_ROLETM, p_item_1->item_id, p_item_1->item_cnt, 9999, 999);
            if (ret != SUCC) {
                STD_ROLLBACK();
                return ret;
            }
        }
        p_item_1++;
	}
	for (uint32_t i = 0; i < p_in->attire_cnt; i++) {
        if (p_in->action == 1) {
            ret = this->gf_attire.del_attire_with_index(USERID_ROLETM, p_item_2->unique_id);
            if (ret != SUCC) {
                STD_ROLLBACK();
                return ret;
            }
        } else if (p_in->action == 2) {
            uint32_t unique_id = 0;
            add_attire_t attire_obj = {0};
            memset(&attire_obj, 0x00, sizeof(add_attire_t));
            attire_obj.attireid = p_item_2->attire_id;
            attire_obj.attire_lv = p_item_2->lv;
            ret = this->gf_attire.add_attire(USERID_ROLETM, &attire_obj, 999, &unique_id);
            if (ret != SUCC) {
                STD_ROLLBACK();
                return ret;
            }

            p_item_2->unique_id = unique_id;
        }
		p_item_2++;
	}

    STD_RETURN_WITH_BUF(ret, p_in, sizeof(gf_safe_trade_item_in_header) + p_in->item_cnt * sizeof(gf_safe_trade_item_in_item1) + p_in->attire_cnt * sizeof(gf_safe_trade_item_in_item2));
}


int Croute_func::gf_insert_tmp_info(DEAL_FUN_ARG)
{
	gf_insert_tmp_info_in *p_in = PRI_IN_POS;
	gf_tmp_info.insert(p_in);
	STD_RETURN(SUCC);
}

int Croute_func::gf_set_player_fate(DEAL_FUN_ARG)
{
	gf_set_player_fate_in * p_in = PRI_IN_POS;
	//gf_set_player_fate_out out = {0};

    ret = this->gf_role.set_int_add_value(USERID_ROLETM, p_in->money, "xiaomee");
    if (ret != SUCC) {
        return ret;
    }

    ret = this->gf_role.set_int_add_value(USERID_ROLETM, p_in->p_exp, "exp");
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }

    ret = this->gf_role.set_int_add_value(USERID_ROLETM, p_in->m_exp, ROLE_ALLOCATOR_EXP_COLUMN);
    if (ret != SUCC) {
        STD_ROLLBACK();
        return ret;
    }

	STD_RETURN_WITH_STRUCT(ret, *p_in);
}


/* ----------------CODE FOR gf_get_user_base_info_kf  ---------*/
// ./Croute_func.cpp

/*得到用户基础信息*/
int Croute_func::gf_get_user_base_info_kf(DEAL_FUNC_ARG){
	gf_get_user_base_info_kf_out* p_out=P_OUT;

    gf_get_user_base_info_out_header out = {0};

    ret = this->gongfu.get_user_base_info(userid, &out);
	if (ret != SUCC)
	{
		return ret;
	}

    p_out->flag = out.flag;
    p_out->regtime = out.regtime;
    p_out->forbid_flag = out.forbid_flag;
    p_out->deadline = out.deadline;
    p_out->vip = out.vip;
    p_out->vip_month_count = out.vip_month_count;
    p_out->start_time = out.start_time;
    p_out->end_time = out.end_time;
    p_out->auto_incr = out.auto_incr;
    p_out->x_value = out.x_value;
    p_out->Ol_count = out.Ol_count;
    p_out->Ol_today = out.Ol_today;
    p_out->Ol_last  = out.Ol_last;
    p_out->Ol_time  = out.Ol_time;
    p_out->amb_status = out.amb_status;
    p_out->parentid   = out.parentid;
    p_out->child_cnt  = out.child_cnt;
    p_out->achieve_cnt= out.achieve_cnt;
    p_out->max_bag = 50;
    if (p_out->vip != 0) {
        uint32_t tail_time = time(NULL);
        if ( p_out->vip == 2 ) {
            tail_time = p_out->end_time;
        }
        uint32_t vip_lv = 0;
        //vip_config_data_mgr::getInstance()->init();
        vip_lv = calc_player_vip_level( &vip_obj, p_out->start_time, tail_time, p_out->x_value, p_out->vip);
        p_out->max_bag += vip_obj.get_add_item_bag_count_by_level(vip_lv);
        DEBUG_LOG("calc_player_vip_level: vip_lv=%u max_bag=%u", vip_lv, p_out->max_bag);
        //vip_config_data_mgr::getInstance()->final();
    }

	return ret;
}

/* ----------------CODE FOR gf_get_user_vip_kf  ---------*/
// ./Croute_func.cpp

/*获取vip相关信息*/
int Croute_func::gf_get_user_vip_kf(DEAL_FUNC_ARG){
	gf_get_user_vip_kf_out* p_out=P_OUT;

    gf_get_user_vip_out out = {0};
    ret = this->gongfu.get_vip_info(userid, &out);
    if (ret != SUCC) {
        return ret;
    }
    if (out.vip_is != 0) {
        uint32_t tail_time = time(NULL);
        if ( out.vip_is == 2 ) {
            tail_time = out.end_tm;
        }
        
        //vip_config_data_mgr::getInstance()->init();
        out.vip_lv = calc_player_vip_level(&vip_obj, out.start_tm, tail_time, out.x_value, out.vip_is);
        DEBUG_LOG("calc_player_vip_level: vip_lv=%u",out.vip_lv);
        //vip_config_data_mgr::getInstance()->final();
        
    }

    p_out->vip_is  = out.vip_is;
    p_out->x_value = out.x_value;
    p_out->vip_lv  = out.vip_lv;
    p_out->vip_month_cnt = out.vip_month_cnt;
    p_out->start_tm = out.start_tm;
    p_out->end_tm   = out.end_tm;
    p_out->auto_incr= out.auto_incr;
    p_out->method   = out.method;

	return ret;
}

/* ----------------CODE FOR gf_get_info_for_login_kf  ---------*/
// ./Croute_func.cpp

/*玩家登陆，拉取必要信息*/
int Croute_func::gf_get_info_for_login_kf(DEAL_FUNC_ARG){
	gf_get_info_for_login_kf_in* p_in=P_IN;
	gf_get_info_for_login_kf_out* p_out=P_OUT;

    DEBUG_LOG("XX ===%u",p_in->role_regtime );
	stru_msg_list user_msg_list = {0};
	stru_msg_list role_msg_list = {0};
	stru_role_info role_info = {0};
    
    stru_gf_info_out user_info = {0};

	//取得用户全局信息
    uint32_t forbid_flag = 0;
	ret = this->gongfu.get_user_info(userid, p_in->role_regtime, &user_info, &forbid_flag, &user_msg_list);
	if (ret != SUCC || forbid_flag != 0)
	{
		return ret;
	}

    memcpy(&user_info, p_out, sizeof(stru_gf_info_out));
	//取得角色信息
	ret = this->gf_role.get_role_info(userid, p_in->role_regtime, &role_info, &role_msg_list);
	if (ret != SUCC)
	{
		return ret;
	}
    p_out->role_regtime = role_info.role_regtime;
	p_out->role_type = role_info.role_type;
	p_out->Ol_count = role_info.Ol_count;
	strncpy(p_out->nick, role_info.nick, sizeof(p_out->nick));
	p_out->skill_point = role_info.skill_point;
	p_out->map_id = role_info.map_id;
	p_out->xpos = role_info.xpos;
	p_out->ypos = role_info.ypos;
	p_out->xiaomee = role_info.xiaomee;
	p_out->level = role_info.level;
	p_out->exp = role_info.exp;
	p_out->alloter_exp = role_info.alloter_exp;
	p_out->hp = role_info.hp;
	p_out->mp = role_info.mp;
	p_out->honour = role_info.honour;
	p_out->fight = role_info.fight;
	p_out->win = role_info.win;
	p_out->fail = role_info.fail;
	p_out->winning_streak = role_info.winning_streak;
	p_out->fumo_points_today = role_info.fumo_points_today;
	p_out->fumo_points_total = role_info.fumo_points;
	p_out->fumo_tower_top = role_info.fumo_tower_top;
	p_out->fumo_tower_used_tm = role_info.fumo_tower_used_tm;
	memcpy (p_out->uniqueitem, role_info.uniqueitem, sizeof(p_out->uniqueitem));
	memcpy (p_out->itembind, role_info.itembind, sizeof(p_out->itembind));
	p_out->double_exp_time = role_info.double_exp_time;
	p_out->day_flag = role_info.day_flag;
	p_out->max_times_chapter = role_info.max_times_chapter;
	p_out->show_state = role_info.show_state;	
	//p_out->open_box_times = role_info.open_box_times;
	p_out->strengthen_cnt = role_info.strengthen_cnt;	
	p_out->achieve_point = role_info.achieve_point;
	p_out->last_update_tm = role_info.last_update_tm;
	p_out->achieve_title = role_info.achieve_title;
	return ret;
}

/* ----------------CODE FOR gf_get_role_list_kf  ---------*/
// ./Croute_func.cpp

/*获取用户角色列表*/
int Croute_func::gf_get_role_list_kf(DEAL_FUNC_ARG){
	gf_get_role_list_kf_out* p_out=P_OUT;

	gf_get_role_list_ex_out_item_1* p_out_item = NULL;

    uint32_t out_cnt = 0;
	ret = this->gf_role.get_all_role_list(userid, &(out_cnt), &p_out_item);
	if (ret != SUCC)
	{
		return ret;
	}

    if (p_out_item) {
        for (uint32_t i = 0; i < out_cnt; i++) {
            //memset(&role_info, 0, sizeof(role_list_item));
            role_list_item role_info;
            role_info.role_time  = p_out_item[i].role_time;
            role_info.role_type  = p_out_item[i].role_type;
            role_info.level      = p_out_item[i].level;
            role_info.status     = p_out_item[i].status;
            memcpy(&(role_info.nick), &(p_out_item[i].nick), sizeof(p_out_item[i].nick));

            p_out->role_list.push_back(role_info);
        }
        free(p_out_item);
        p_out_item = NULL;
    }
	return ret;
}

/* ----------------CODE FOR gf_get_clothes_list_kf  ---------*/
// ./Croute_func.cpp

/*拉取角色所有装备信息*/
int Croute_func::gf_get_clothes_list_kf(DEAL_FUNC_ARG){
	gf_get_clothes_list_kf_in* p_in=P_IN;
	gf_get_clothes_list_kf_out* p_out=P_OUT;

	gf_get_clothes_list_ex_out_item* p_out_item = NULL;

    uint32_t out_cnt = 0;
	ret = this->gf_attire.get_clothes_list_ex(userid, p_in->role_regtime, &(out_cnt), &p_out_item);
	if (ret != SUCC)
	{
		return ret;
	}

    if (p_out_item) {
        for (uint32_t i = 0; i < out_cnt; i++) {
            role_clothes_list_item _info;
            _info.attireid  = p_out_item[i].attireid;
            _info.id        = p_out_item[i].id;
            _info.usedflag  = p_out_item[i].usedflag;
            _info.duration  = p_out_item[i].duration;

            p_out->role_clothse_list.push_back(_info);
        }
        free(p_out_item);
        p_out_item = NULL;
    }

	return ret;
}

/* ----------------CODE FOR gf_get_killed_boss_kf  ---------*/
// ./Croute_func.cpp

/*获取玩家通关关卡的信息*/
int Croute_func::gf_get_killed_boss_kf(DEAL_FUNC_ARG){
	gf_get_killed_boss_kf_in* p_in=P_IN;
	gf_get_killed_boss_kf_out* p_out=P_OUT;

	gf_get_killed_boss_kf_out_item *p_out_item = NULL;

    uint32_t out_cnt = 0;
	ret = this->gf_killboss.get_killed_boss_kf(userid, p_in->role_regtime, &(out_cnt), &p_out_item);

    if (p_out_item) {
        for (uint32_t i = 0; i < out_cnt; i++) {
            finish_stage_list_item _info;
            _info.stage_id  = p_out_item[i].stage_id;
            memcpy(_info.grade, p_out_item[i].grade, sizeof(p_out_item[i].grade));

            p_out->kill_boss_stage_list.push_back(_info);
        }
        free(p_out_item);
        p_out_item = NULL;
    }

	return ret;
}

/* ----------------CODE FOR gf_del_attire_kf  ---------*/
// ./Croute_func.cpp

/*删除角色装备*/
int Croute_func::gf_del_attire_kf(DEAL_FUNC_ARG){
	gf_del_attire_kf_in* p_in=P_IN;
	gf_del_attire_kf_out* p_out=P_OUT;

    gf_del_attire_in _in = {0};
    _in.attireid = p_in->attire_id;
    _in.index_id = p_in->index_id;

	ret = this->gf_attire.del_attire(userid, p_in->role_regtime, &(_in));
	if (ret != SUCC)
	{
		return ret;
	}

    p_out->attire_id = p_in->attire_id;
    p_out->index_id  = p_in->index_id;

	return ret;
}

/* ----------------CODE FOR gf_add_attire_kf  ---------*/
// ./Croute_func.cpp

/*给角色添加装备*/
int Croute_func::gf_add_attire_kf(DEAL_FUNC_ARG){
	gf_add_attire_kf_in* p_in=P_IN;

	m_attire_elem.attireid = p_in->attireid;
	m_attire_elem.gettime = p_in->gettime;
	m_attire_elem.attire_rank = 1;
	m_attire_elem.duration = p_in->duration;
    m_attire_elem.endtime = 0;
	m_attire_elem.usedflag= p_in->usedflag;
    m_attire_elem.attire_lv = p_in->attire_lv;
    if (p_in->attire_lv < 0 || p_in->attire_lv > 12)
        m_attire_elem.attire_lv = 0;
	ret = this->gf_attire.add_attire(userid, p_in->role_regtime, &m_attire_elem, p_in->max_bag_count);
	
	return ret;
}

/* ----------------CODE FOR gf_add_role_kf  ---------*/
// ./Croute_func.cpp

/*创建新的角色*/
int Croute_func::gf_add_role_kf(DEAL_FUNC_ARG){
	gf_add_role_kf_in* p_in=P_IN;
	gf_add_role_kf_out* p_out=P_OUT;

	//uint32_t skill_id = 0;
	bool is_existed = true;
	uint32_t regtime = 0;
	if (p_in->role_type > 4 || p_in->role_type == 0)
	{
		return ROLE_TYPE_NOFIND_ERR;
	}
	ret = this->gongfu.is_user_existed(userid, &is_existed, &regtime);

	if (!is_existed)
	{
		ret = this->gongfu.add_user(userid, p_in->parentid);
		if (ret != SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}
	}
	else if (!regtime)
	{
		time_t now = time (NULL);
		ret = this->gongfu.set_int_value(userid, (uint32_t)now, "regtime");	
		if (ret != SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}
		if (p_in->parentid) 
		{
			ret = this->gongfu.set_int_value(userid, p_in->parentid, "parentid"); 
			if (ret != SUCC)
			{
				STD_ROLLBACK();
				return ret;
			}
		}
	}
	else 
	{
		p_out->user_existed = 1;
	}

	ret = this->gf_role.is_role_num_max(userid);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

    gf_add_role_in _in = {0};
    _in.role_type = p_in->role_type;
    _in.level     = p_in->role_type;
    _in.hp        = p_in->role_type;
    _in.mp        = p_in->role_type;
    memcpy(_in.nick, p_in->nick, sizeof(p_in->nick));

    uint32_t role_regtime = time(NULL);
	ret = this->gf_role.add_role(userid, role_regtime, &(_in));
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

	add_attire_t m_attire_elem2 = {0};

	//donate towards
	if (p_in->role_type == 1)
	{
		m_attire_elem.attireid = 100001;
		m_attire_elem.duration = 32 * 50;

		m_attire_elem2.attireid = 100585;
		m_attire_elem2.duration = 116 * 50;
		//skill_id = 100204;
	}
	else if (p_in->role_type == 2)
	{
		m_attire_elem.attireid = 200001;
		m_attire_elem.duration = 35 * 50;

		m_attire_elem2.attireid = 200585;
		m_attire_elem2.duration = 116 * 50;
		//skill_id = 200201;
	}
	else if (p_in->role_type == 3)
	{
		m_attire_elem.attireid = 300001;
		m_attire_elem.duration = 29 * 50;

		m_attire_elem2.attireid = 300585;
		m_attire_elem2.duration = 116 * 50;
		//skill_id = 300213;
	}
    else if (p_in->role_type == 4) {
        m_attire_elem.attireid = 400014;
        m_attire_elem.duration = 20 * 50;

		m_attire_elem2.attireid = 400585;
		m_attire_elem2.duration = 116 * 50;
        //skill_id = 300213;
    }
		
	// insert default attires
	m_attire_elem.attire_rank = 1;
	m_attire_elem.gettime = time(NULL);
    m_attire_elem.endtime = 0;
	m_attire_elem.usedflag = 0;
	ret = this->gf_attire.add_attire(userid, role_regtime, &m_attire_elem, MAX_ATTIRE_CNT);
	//if (ret != SUCC)
	{
	//	STD_ROLLBACK();
	//	return ret;
	}

	m_attire_elem2.attire_rank = 1;
	m_attire_elem2.gettime = time(NULL);
    m_attire_elem2.endtime = 0;
	m_attire_elem2.usedflag = 1;
	ret = this->gf_attire.add_attire(userid, role_regtime, &m_attire_elem2, MAX_ATTIRE_CNT);
	
	
	if ((240273282 <= userid && userid <= 240323464) ||
		(240223021 <= userid && userid <= 240273281) ||
		(240001000 <= userid && userid <= 240101443) ||
		(240101444 <= userid && userid <= 240223020) ||
		(241000100 <= userid && userid <= 241201966) ||
		(241400010 <= userid && userid <= 241601554) ||
		(240323465 <= userid && userid <= 240976689)) {
		this->gf_mail.insert_sys_nor_mail(userid, role_regtime, 1014, "", "");
		this->gf_role.increase_int_value(userid, role_regtime, ROLE_XIAOMEE_COLUMN, 4000);
	}
	
	//insert amb reward of invitee
	uint32_t parent_id = 0;
	this->gongfu.get_parent_id(userid, &parent_id);
	if (parent_id) {
		//insert
		this->gf_increase_item(userid, role_regtime, 1700050, 1, 999, 999);
	}
	
	this->gf_increase_item(userid, role_regtime, 1700009, 1, 999, 999);
	this->gf_increase_item(userid, role_regtime, 1300001, 10, 999, 999);
	this->gf_increase_item(userid, role_regtime, 1300101, 10, 999, 999);
	this->gf_increase_item(userid, role_regtime, 1302001, 8, 999, 999);
	//
	//this->gf_mail.insert_sys_nor_mail(userid, role_regtime, 1004, "", "");

	// insert default skills
/*	
	add_skill_t skill_in = {0};
	skill_in.skill_id = skill_id;
	skill_in.skill_lv = 1;
	skill_in.bind_key = 10;
	ret = this->gf_skill.add_skill(userid, role_regtime, &skill_in);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
*/
	
	ret = this->gf_role.get_role_num(userid, ROLE_STATUS_ALL, &(p_out->role_num));
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

    if (p_in->parentid != 0) {
        ret = this->gf_friend.add(userid, p_in->parentid, 0, TYPE_FRIEND);
        if (ret != SUCC) {
            STD_ROLLBACK();
            return ret;
        }
    }

	return ret;
}

/* ----------------CODE FOR gf_del_role_kf  ---------*/
// ./Croute_func.cpp

/*删除玩家角色*/
int Croute_func::gf_del_role_kf(DEAL_FUNC_ARG){
	gf_del_role_kf_in* p_in=P_IN;

    uint32_t status = 0;
	ret = this->gf_role.get_int_value(userid, p_in->role_regtime, "status", &status);
	if (ret != SUCC)
	{
		return ret;
	}	
	if (status != ROLE_STATUS_DEL)
	{
		return SYS_ERR;
	}
	ret = del_role_phy(userid, p_in->role_regtime);

	return ret;
}

/* ----------------CODE FOR gf_add_item_kf  ---------*/
// ./Croute_func.cpp

/*添加道具物品*/
int Croute_func::gf_add_item_kf(DEAL_FUNC_ARG){
	gf_add_item_kf_in* p_in=P_IN;

    ret = gf_increase_item(userid,p_in->role_regtime,p_in->item_id,p_in->add_num,
        set_item_limit(p_in->item_id), p_in->max_bag_count);
	if (ret != SUCC) {
		STD_ROLLBACK();
		return ret;
	}
	return ret;
}

/* ----------------CODE FOR gf_set_role_delflg_kf  ---------*/
// ./Croute_func.cpp

/*设置角色可是被删除的标志（该接口暂时不可用）*/
int Croute_func::gf_set_role_delflg_kf(DEAL_FUNC_ARG){
	gf_set_role_delflg_kf_in* p_in=P_IN;
    uint32_t role_num = 0;
	ret = this->gf_role.get_role_num(userid, ROLE_STATUS_NOR_BLOCK, &role_num);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	if (role_num <= 1) 
	{
		STD_ROLLBACK();
		return ROLE_NUM_MIN_ERR;
	}

	ret = this->gf_role.set_role_delflg(userid, p_in->role_regtime);

	return ret;
}

/* ----------------CODE FOR gf_get_friend_list_type_kf  ---------*/
// ./Croute_func.cpp

/*获取朋友列表*/
int Croute_func::gf_get_friend_list_type_kf(DEAL_FUNC_ARG){
	gf_get_friend_list_type_kf_in* p_in=P_IN;
	gf_get_friend_list_type_kf_out* p_out=P_OUT;

	gf_get_friendlist_type_out_item* p_out_item = NULL;

    uint32_t out_cnt = 0;
	ret = this->gf_friend.get_friendid_list_type(userid, (FRIEND_TYPE)(p_in->friend_type), 
		&(out_cnt), &p_out_item);
	if (ret != SUCC)
	{
		return ret;
	}

    if (p_out_item) {
        for (uint32_t i = 0; i < out_cnt; i++) {
            friend_list_item_t _info;
            _info.friend_id  = p_out_item[i].friend_id;
            _info.friend_type  = p_out_item[i].friend_type;

            p_out->friend_list.push_back(_info);
        }
        free(p_out_item);
        p_out_item = NULL;
    }

	return ret;
}

/* ----------------CODE FOR gf_add_friend_kf  ---------*/
// ./Croute_func.cpp

/*添加好友*/
int Croute_func::gf_add_friend_kf(DEAL_FUNC_ARG){
	gf_add_friend_kf_in* p_in=P_IN;

    uint32_t vip_type = 0;
	ret = this->gongfu.get_vip_type(userid, &vip_type);
	if (ret != SUCC)
	{
		return ret;
	}

	ret = this->gf_friend.add(userid, p_in->friend_id, vip_type, TYPE_FRIEND);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

	return ret;
}

/* ----------------CODE FOR gf_del_friend_kf  ---------*/
// ./Croute_func.cpp

/*删除各类型的好友*/
int Croute_func::gf_del_friend_kf(DEAL_FUNC_ARG){
	gf_del_friend_kf_in* p_in=P_IN;

    //DEBUG_LOG("XXXXXXXXX [%u %u]", userid, p_in->userid);
	ret = this->gf_friend.del(userid, p_in->friend_id, TYPE_FRIEND);

	return ret;
}

/* ----------------CODE FOR gf_set_nick_kf  ---------*/
// ./Croute_func.cpp

/*修改昵称*/
int Croute_func::gf_set_nick_kf(DEAL_FUNC_ARG){
	gf_set_nick_kf_in* p_in=P_IN;
	gf_set_nick_kf_out* p_out=P_OUT;

	ret = this->gf_role.update_nick(userid, p_in->role_regtime, p_in->nick);
	if (ret != SUCC) {
		return ret;
	}
	if (p_in->use_item) {
		ret = this->gf_item.reduce_item_int_value(userid, p_in->role_regtime, p_in->use_item, "count", 1);
		if (ret != SUCC) {
			STD_ROLLBACK();
			return ret;
		}
	}

    p_out->use_item = p_in->use_item;
	return ret;
}

/* ----------------CODE FOR gf_set_role_int_value_kf  ---------*/
// ./Croute_func.cpp

/*修改角色属性*/
int Croute_func::gf_set_role_int_value_kf(DEAL_FUNC_ARG){
	gf_set_role_int_value_kf_in* p_in=P_IN;

	ret = this->gf_role.set_int_value(userid, p_in->role_regtime, p_in->column_name, p_in->value);

	return ret;
}

/* ----------------CODE FOR gf_set_item_int_value_kf  ---------*/
// ./Croute_func.cpp

/*修改物品的数量*/
int Croute_func::gf_set_item_int_value_kf(DEAL_FUNC_ARG){
	gf_set_item_int_value_kf_in* p_in=P_IN;

	if (!is_strengthen_material(p_in->item_id)) {
		uint32_t item_cnt = 0;
		ret = this->gf_item.get_item_int_value(userid,p_in->role_regtime,p_in->item_id,item_count_column,&item_cnt);
		if (ret != SUCC)
		{
			return ret;
		}

		if (item_cnt != p_in->value) {
			if (item_cnt < p_in->value) {
				ret = this->gf_increase_item(userid,p_in->role_regtime,p_in->item_id,p_in->value - item_cnt,9999, 9999);
				if (ret != SUCC)
				{	
					STD_ROLLBACK();
					return ret;
				}
			} else {
				ret = this->gf_item.reduce_item_int_value(userid,p_in->role_regtime,p_in->item_id,item_count_column, item_cnt - p_in->value);
				if (ret != SUCC)
				{
					STD_ROLLBACK();
					return ret;
				}
			}
		}
	} else {
		if (p_in->value)
		{
			ret = this->gf_material.reduce_material_count(userid,p_in->role_regtime,p_in->item_id,0xffffffff);
			if (ret != SUCC)
			{
				STD_ROLLBACK();
				return ret;
			}

			ret = this->gf_increase_item(userid,p_in->role_regtime,p_in->item_id,p_in->value, 9999, 9999);
			if (ret != SUCC)
			{
				STD_ROLLBACK();
				return ret;
			}
		}
		else
		{
			ret = this->gf_material.reduce_material_count(userid,p_in->role_regtime, p_in->item_id, 0xffffffff);
			if (ret != SUCC)
			{
				STD_ROLLBACK();
				return ret;
			}
		}
	}

	return ret;
}

/* ----------------CODE FOR gf_get_user_item_list_kf  ---------*/
// ./Croute_func.cpp

/*获取玩家物品列表*/
int Croute_func::gf_get_user_item_list_kf(DEAL_FUNC_ARG){
	gf_get_user_item_list_kf_in* p_in=P_IN;
	gf_get_user_item_list_kf_out* p_out=P_OUT;

	gf_get_user_item_list_out_item* p_out_item = NULL;

    uint32_t out_cnt = 0;
	ret = this->gf_item.get_user_item_list(userid, p_in->role_regtime, &(out_cnt), &p_out_item);
	if (ret != SUCC)
	{
		return ret;
	}

    if (p_out_item) {
        for (uint32_t i = 0; i < out_cnt; i++) {
            item_list_t _info;
            _info.item_id  = p_out_item[i].itemid;
            _info.count    = p_out_item[i].count;

            p_out->item_list.push_back(_info);
        }
        free(p_out_item);
        p_out_item = NULL;
    }

	return ret;
}

/* ----------------CODE FOR gf_get_material_list_kf  ---------*/
// ./Croute_func.cpp

/*得到材料道具类表*/
int Croute_func::gf_get_material_list_kf(DEAL_FUNC_ARG){
	gf_get_material_list_kf_in* p_in=P_IN;
	gf_get_material_list_kf_out* p_out=P_OUT;

	gf_get_strengthen_material_list_out_item* p_out_item = NULL;

    uint32_t out_cnt = 0;
	ret = this->gf_material.get_user_material_list(userid, p_in->role_regtime, &(out_cnt), &p_out_item);
	if (ret != SUCC) {
		return ret;
	}

    if (p_out_item) {
        for (uint32_t i = 0; i < out_cnt; i++) {
            item_list_t _info;
            _info.item_id  = p_out_item[i].material_id;
            _info.count    = p_out_item[i].cnt;

            p_out->material_list.push_back(_info);
        }
        free(p_out_item);
        p_out_item = NULL;
    }

	return ret;
}

/* ----------------CODE FOR gf_del_item_by_num_kf  ---------*/
// ./Croute_func.cpp

/*减少物品数量*/
int Croute_func::gf_del_item_by_num_kf(DEAL_FUNC_ARG){
	gf_del_item_by_num_kf_in* p_in=P_IN;
	gf_del_item_by_num_kf_out* p_out=P_OUT;

    ret = gf_reduce_item_count(userid, p_in->role_regtime, p_in->item_id,p_in->del_num);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

    p_out->item_id = p_in->item_id;
    p_out->del_num = p_in->del_num;

	return ret;
}

/* ----------------CODE FOR gf_get_all_task_kf  ---------*/
// ./Croute_func.cpp

/*获取所有任务列表*/
int Croute_func::gf_get_all_task_kf(DEAL_FUNC_ARG){
	gf_get_all_task_kf_in* p_in=P_IN;
	gf_get_all_task_kf_out* p_out=P_OUT;

	gf_get_all_task_out_item *p_out_item = NULL;
    uint32_t out_cnt = 0;
	ret = this->gf_task.get_all_task(userid, p_in->role_regtime, &(out_cnt), &p_out_item);
	if (ret != SUCC)
	{
		return ret;
	}

    if (p_out_item) {
        for (uint32_t i = 0; i < out_cnt; i++) {
            task_list_t _info;
            _info.taskid    = p_out_item[i].taskid;
            _info.task_flag = p_out_item[i].task_flg;
//added by cws 20120502
            _info.task_tm = p_out_item[i].task_tm;

            p_out->task_list.push_back(_info);
        }
        free(p_out_item);
        p_out_item = NULL;
    }

	return ret;
}

/* ----------------CODE FOR gf_add_killed_boss_kf  ---------*/
// ./Croute_func.cpp

/*添加玩家杀死boss信息*/
int Croute_func::gf_add_killed_boss_kf(DEAL_FUNC_ARG){
	gf_add_killed_boss_kf_in* p_in=P_IN;

    gf_add_killed_boss_in _in = {0};
    _in.boss_id = p_in->boss_id;
	ret = this->gf_killboss.add_killed_boss(userid, p_in->role_regtime, &(_in));

	return ret;
}

/* ----------------CODE FOR gf_set_task_flg_kf  ---------*/
// ./Croute_func.cpp

/*设置任务的状态*/
int Croute_func::gf_set_task_flg_kf(DEAL_FUNC_ARG){
	gf_set_task_flg_kf_in* p_in=P_IN;
	gf_set_task_flg_kf_out* p_out=P_OUT;

	if (p_in->task_flag == SET_TASK_IN_HAND)
	{
		ret = this->gf_task.set_task_in_hand(userid, p_in->role_regtime, p_in->taskid);
	}
	else if (p_in->task_flag == SET_TASK_FINISHED)
	{
		ret = this->gf_task.set_task_finished(userid, p_in->role_regtime, p_in->taskid);
	}
	else if (p_in->task_flag==SET_TASK_NULL)
	{
		ret = this->gf_task.del_task(userid, p_in->role_regtime, p_in->taskid);
    }
    else if (p_in->task_flag==SET_TASK_CANCEL)
    {
        ret =this->gf_task.set_task_cancel(userid, p_in->role_regtime, p_in->taskid);
    }
	else
		ret = ENUM_OUT_OF_RANGE_ERR;
	p_out->taskid = p_in->taskid;

	return ret;
}

/* ----------------CODE FOR gf_get_skill_list_kf  ---------*/
// ./Croute_func.cpp

/*获取技能列表*/
int Croute_func::gf_get_skill_list_kf(DEAL_FUNC_ARG){
	gf_get_skill_list_kf_in* p_in=P_IN;
	gf_get_skill_list_kf_out* p_out=P_OUT;

    uint32_t out_cnt = 0, left_sp = 0;
	ret = this->gf_role.get_int_value(userid, p_in->role_regtime, ROLE_SKILL_POINT_COLUMN, &(left_sp));
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	gf_get_skill_list_out_item *p_out_item = NULL;
	ret = this->gf_skill.get_skill_list(userid, p_in->role_regtime, &(out_cnt), &p_out_item);
	if (ret!=SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

    p_out->left_sp = left_sp;

    if (p_out_item) {
        for (uint32_t i = 0; i < out_cnt; i++) {
            skill_list_t _info;
            _info.skill_id  = p_out_item[i].skill_id;
            _info.skill_lv  = p_out_item[i].skill_lv;
            _info.skill_point  = p_out_item[i].skill_point;

            p_out->skill_list.push_back(_info);
        }
        free(p_out_item);
        p_out_item = NULL;
    }

	return ret;
}

/* ----------------CODE FOR gf_add_skill_with_no_book_kf  ---------*/
// ./Croute_func.cpp

/*添加技能*/
int Croute_func::gf_add_skill_with_no_book_kf(DEAL_FUNC_ARG){
	gf_add_skill_with_no_book_kf_in* p_in=P_IN;
	gf_add_skill_with_no_book_kf_out* p_out=P_OUT;

	ret = this->gf_skill.add_skill(userid, p_in->role_regtime, p_in->skill_id, p_in->skill_lv);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	p_out->skill_id = p_in->skill_id;

	return ret;
}

/* ----------------CODE FOR gf_del_skill_kf  ---------*/
// ./Croute_func.cpp

/*删除技能*/
int Croute_func::gf_del_skill_kf(DEAL_FUNC_ARG){
	gf_del_skill_kf_in* p_in=P_IN;

    gf_del_skill_in _in = {0};
    _in.skill_id = p_in->skill_id;
	ret = this->gf_skill.del_skill(userid, p_in->role_regtime, &(_in));

	return ret;
}

/* ----------------CODE FOR gf_get_summon_list_kf  ---------*/
// ./Croute_func.cpp

/*拉取召唤兽信息*/
int Croute_func::gf_get_summon_list_kf(DEAL_FUNC_ARG){
	gf_get_summon_list_kf_in* p_in=P_IN;
	gf_get_summon_list_kf_out* p_out=P_OUT;

    gf_get_summon_list_out_item* p_out_item = NULL;

    uint32_t out_cnt = 0;
    ret = this->gf_summon.get_summon_list(userid, p_in->role_regtime, &(out_cnt), &p_out_item);
    if (ret != SUCC) {
        return ret;
    }

    if (p_out_item) {
        for (uint32_t i = 0; i < out_cnt; i++) {
            summon_list_t _info;
            _info.mon_tm    = p_out_item[i].mon_tm;
            _info.mon_type  = p_out_item[i].mon_type;
            _info.lv        = p_out_item[i].lv;
            _info.exp       = p_out_item[i].exp;
            _info.status    = p_out_item[i].status;
            _info.fight_value = p_out_item[i].fight_value;
            memcpy(_info.mon_nick, p_out_item[i].mon_nick, sizeof(p_out_item[i].mon_nick));

            p_out->summon_list.push_back(_info);
        }
        free(p_out_item);
        p_out_item = NULL;
    }

	return ret;
}

/* ----------------CODE FOR gf_set_account_forbid_kf  ---------*/
// ./Croute_func.cpp

/*设置封号信息*/
int Croute_func::gf_set_account_forbid_kf(DEAL_FUNC_ARG){
	gf_set_account_forbid_kf_in* p_in=P_IN;

	ret = this->gongfu.add_user_for_boss(userid);
	if (ret != SUCC) {
		STD_ROLLBACK();
        return ret;
    }
    uint32_t limit_tm = 0;
    if (p_in->deadline != 0xffffffff) {
        //limit_tm = time(NULL) + (p_in->deadline) * 3600 * 24;
        limit_tm = (p_in->deadline) / 7 + 1;
    } else {
        //limit_tm = p_in->deadline;
        limit_tm = 4;
    }
    DEBUG_LOG("forbid flag=[%u] dur=[%x] time=[%x]",p_in->forbid_flag,p_in->deadline,limit_tm);
    ret = this->gongfu.update_account_forbid(userid, p_in->forbid_flag, limit_tm);
    if (ret != SUCC) {
        return ret;
    }

	return ret;
}

/* ----------------CODE FOR gf_post_msg_kf  ---------*/
// ./Croute_func.cpp

/*发送即时消息给用户(switch接口)*/
int Croute_func::gf_post_msg_kf(DEAL_FUNC_ARG){
	gf_post_msg_kf_in* p_in=P_IN;



	return ret;
}

/* ----------------CODE FOR gf_kick_user_offline_kf  ---------*/
// ./Croute_func.cpp

/*将用户踢下线(switch接口)*/
int Croute_func::gf_kick_user_offline_kf(DEAL_FUNC_ARG){
	gf_kick_user_offline_kf_in* p_in=P_IN;



	return ret;
}

/* ----------------CODE FOR gf_official_notice_kf  ---------*/
// ./Croute_func.cpp

/*发送官方消息的接口(switch接口)*/
int Croute_func::gf_official_notice_kf(DEAL_FUNC_ARG){
	gf_official_notice_kf_in* p_in=P_IN;



	return ret;
}


// ./proto/gf_db_db_src.cpp
