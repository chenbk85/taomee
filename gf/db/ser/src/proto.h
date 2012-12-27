#ifndef  GF_PROTO_H
#define  GF_PROTO_H
#ifndef  MK_PROTO_DOC 
#include "proto_header.h"
#endif
extern "C" {
#include <libtaomee/log.h>
}

#define RECVBUF_ROLETM                              (RECVBUF_HEADER->role_tm)
#define USERID_ROLETM								RECVBUF_USERID,RECVBUF_ROLETM

enum {
    max_summon_skills  = 5,
	max_client_buf_len = 40,
};

/*
class Cbase_mysql_operater() 
{
public:
	Cbase_mysql_operater(const char* column1, const char* column2) {
		strncpy(userid_column, column1, strlen(userid_column) - 1);
		strncpy(role_column, column2, strlen(userid_column) - 1);
	}
	int clear_role_info();
private:
	char	userid_column[64];
	char	role_column[64];
};
*/

//得到用户信息
#define gf_get_user_info_cmd                (0x0601 )
//获取用户当前的功夫豆
#define gf_get_xiaomee_cmd                  (0x0602 )

#define gf_fuck_version_check_cmd           (0x0604 )

#define gf_bitch_sell_items_cmd             (0x0605 )
//设置用户邀请玩家个数
#define gf_set_child_count_cmd              (0x0609 | NEED_UPDATE)
//设置用户邀请玩家达标个数
#define gf_set_achieve_count_cmd            (0x060A | NEED_UPDATE)
//设置大使任务状态标志位
#define gf_set_amb_status_cmd               (0x060B | NEED_UPDATE)
//设置大使任务奖赏标志位
#define gf_set_amb_reward_flag_cmd          (0x060C | NEED_UPDATE)

//出售装备
#define gf_sell_attire_cmd						(0x0610 | NEED_UPDATE)
//维修装备
#define gf_repair_all_attire_cmd				(0x0611 | NEED_UPDATE)
//设置装备表int字段值
#define gf_attire_set_int_value_cmd				(0x0612 | NEED_UPDATE)
//商城购买物品预验证
#define gf_prepare_add_product_cmd              (0x0613)
//商城购买物品
#define gf_add_store_product_cmd                (0x0614 | NEED_UPDATE)
//商城道具续费预验证
#define gf_prepare_repair_product_cmd           (0x0615)
//商城道具续费
#define gf_repair_store_product_cmd             (0x0616 | NEED_UPDATE)
//过期(商城)道具
#define gf_get_outdated_product_cmd             (0x0617)
//同步功夫通宝
#define gf_synchro_user_gfcoin_cmd              (0x0618 | NEED_UPDATE)
//查询用户功夫通宝
#define gf_query_user_gfcoin_cmd                (0x0619)
//功夫券交易物品接口
#define gf_coupons_exchange_cmd                 (0x061A | NEED_UPDATE)

//删除/添加物品装备接口
#define gf_safe_trade_item_cmd                  (0x061B | NEED_UPDATE)

//登陆时创建角色及其相关信息
#define gf_get_info_for_login_cmd           (0x06A1 )

//修改昵称
#define gf_set_nick_cmd			            (0x0602 | NEED_UPDATE)
//获取用户的昵称
#define gf_get_nick_cmd                     (0x0603)

//用户退出时统计信息
#define gf_logout_cmd						(0x0605 | NEED_UPDATE)

#define gf_set_client_buf_cmd				(0x060D | NEED_UPDATE)

#define gf_get_client_buf_cmd				(0x061E | NEED_UPDATE)

#define gf_get_plant_list_cmd				(0x061F)

#define gf_set_plant_status_cmd				(0x0620 | NEED_UPDATE)

#define gf_add_effect_to_all_plants_cmd		(0x062A | NEED_UPDATE)

#define gf_add_home_log_cmd					(0x062B | NEED_UPDATE)

#define gf_get_home_log_cmd					(0x062C)
#define gf_get_other_info_list_cmd			(0x062D)
#define gf_set_other_info_cmd				(0x0637 | NEED_UPDATE)

#define gf_list_simple_role_info_cmd        (0x073A)


#define gf_get_card_list_cmd                (0x07AD)
#define gf_insert_card_cmd                  (0x06AE | NEED_UPDATE)
#define gf_erase_card_cmd                   (0x06AF | NEED_UPDATE)


//添加好友
#define gf_add_friend_cmd	                (0x0630 | NEED_UPDATE)

//删除好友
#define gf_del_friend_cmd	                (0x0631 | NEED_UPDATE)

//添加黑名单
#define gf_add_black_cmd                  (0x0632 | NEED_UPDATE)

//删除黑名单
#define gf_del_black_cmd                  (0x0633 | NEED_UPDATE)

#define gf_forbiden_add_friend_flag_cmd     (0x0634 | NEED_UPDATE)
#define gf_query_forbiden_friend_flag_cmd   (0x0635 | NEED_UPDATE)

#define gf_del_friend_whatever_cmd			(0x0636 | NEED_UPDATE)

//拉取黑名单列表
#define gf_get_blacklist_cmd                  (0x0634)

//取得好友列表（php后台管理用）
#define gf_get_friendlist_type_cmd				(0x0635)

//得到用户装备信息
#define gf_get_clothes_list_cmd             (0x0641 )

//买装备
#define gf_buy_attire_cmd				(0x0643 | NEED_UPDATE)

//得到用户装备信息（php后台管理用）
#define gf_get_clothes_list_ex_cmd			(0x06E0)

//得到用户物品信息
#define gf_get_user_item_list_cmd           (0x0642)

//得到背包中的装备和物品
#define gf_get_package_iclist_cmd           (0x0643)
//得到用户强化材料信息
#define gf_get_strengthen_material_list_cmd (0x0644)
//强化装备
#define gf_strengthen_attire_cmd            (0x064B | NEED_UPDATE)
//added by cws 0620
#define gf_strengthen_attire_without_material_cmd            (0x065C | NEED_UPDATE)

#define gf_del_items_cmd					(0x064C | NEED_UPDATE)
#define gf_set_role_vitality_point_cmd      (0x064D | NEED_UPDATE)
#define gf_set_second_pro_exp_cmd           (0x064E | NEED_UPDATE)

#define gf_set_player_team_flg_cmd           (0x064F | NEED_UPDATE)

//购买物品
#define gf_buy_item_cmd						(0x0645 | NEED_UPDATE)
#define gf_donate_item_cmd                  (0x065A | NEED_UPDATE)

//使用物品
#define gf_battle_use_item_cmd				(0x0647 | NEED_UPDATE)

//拾取物品
#define gf_pick_up_item_cmd					(0x0648 | NEED_UPDATE)

#define gf_pick_up_roll_item_cmd            (0x0650 | NEED_UPDATE)

//小游戏的奖励
#define gf_game_input_cmd					(0x0651 | NEED_UPDATE)

#define gf_add_buff_cmd						(0x0652 | NEED_UPDATE)

#define gf_reset_skill_cmd					(0x0653 | NEED_UPDATE)

#define gf_update_buff_list_cmd				(0x0654 | NEED_UPDATE)

#define gf_del_buff_on_player_cmd           (0x065E | NEED_UPDATE)           

#define gf_distract_clothes_strength_cmd    (0x06B1 | NEED_UPDATE)

//设置item表的int类型的值（php后台管理用）
#define gf_set_item_int_value_cmd				(0x0655 | NEED_UPDATE)

//增加物品
#define gf_add_item_cmd						(0x0656 | NEED_UPDATE)

//删除物品
#define gf_del_item_cmd						(0x0657 | NEED_UPDATE)

//出售道具
#define gf_sell_item_cmd					(0x0658 | NEED_UPDATE)

//添加一件装备或者物品
#define gf_add_item_attire_cmd              (0x0659 | NEED_UPDATE)


//vip start
#ifdef MAKE_SEND_EMAIL
#define other_gf_sync_vip_cmd            (0x0660 | NEED_UPDATE)
#define other_gf_sync_base_svalue_cmd    (0x0661 | NEED_UPDATE)
#define other_gf_sync_gold_cmd           (0x0662 | NEED_UPDATE)
#define other_gf_vip_sys_msg_cmd         (0x0663 | NEED_UPDATE)
#else
#define gf_set_vip_cmd                   (0x0660 | NEED_UPDATE)
#define gf_set_base_svalue_cmd           (0x0661 | NEED_UPDATE)
#define gf_set_gold_cmd                  (0x0662 | NEED_UPDATE)
#define gf_set_vip_exinfo_cmd            (0x0664 | NEED_UPDATE)
#endif
//获取用户vip信息
#define gf_get_user_vip_cmd              (0x0660)
//vip end
//设置武林密码获得礼物
#define gf_set_cryptogram_gift_cmd       (0x0669 | NEED_UPDATE)
//设置神奇密码获得礼物
#define gf_set_magic_gift_cmd            (0x066A | NEED_UPDATE)
//获取魔术箱中物品
#define gf_set_magic_box_cmd             (0x066B | NEED_UPDATE)

//用户换装备
#define gf_wear_clothes_cmd                 (0x0644 | NEED_UPDATE)
//脱下装备
#define gf_take_off_clothes_cmd                 (0x0645)

//得到用户部分信息
#define gf_get_user_partial_info_cmd 			(0x0606)

//获得角色详细信息 (level,exp,hp,mp)
#define gf_get_role_detail_info_cmd				(0x0607)

//添加离线消息
#define gf_add_offline_msg_cmd					(0x0604 | NEED_UPDATE)

//用户登录
#define gf_role_login_cmd						(0x06E4)
//设置封号信息（php后台管理用）
#define gf_set_account_forbid_cmd               (0x060B)
//取得用户基础信息（php后台管理用）
#define gf_get_user_base_info_cmd				(0x060C)
//修改用户基础信息（php后台管理用）
#define gf_set_user_base_info_cmd				(0x060D)

//玩家占卜命运信息
#define gf_set_player_fate_cmd                  (0x060F)
//begin summon
//获取召唤兽信息
#define gf_get_summon_list_cmd                  (0x0620)
//孵化召唤兽
#define gf_hatch_summon_cmd                     (0x0621 | NEED_UPDATE)
//喂养召唤兽
#define gf_feed_summon_cmd                      (0x0622 | NEED_UPDATE)
//设置召唤兽的昵称
#define gf_set_summon_nick_cmd                  (0x0623 | NEED_UPDATE)
//交换出战的召唤兽
#define gf_change_summon_fight_cmd              (0x0624 | NEED_UPDATE)
//设置召唤兽的基本属性
#define gf_set_summon_property_cmd              (0x0625 | NEED_UPDATE)
//设置召唤兽的技能
#define gf_set_summon_skills_cmd                (0x062C | NEED_UPDATE)
//使用灵兽技能卷轴
#define gf_use_summon_skills_scroll_cmd         (0x062D | NEED_UPDATE)
//召唤兽变异
#define gf_set_summon_mutate_cmd                (0x0626 | NEED_UPDATE)
//复原召唤兽超灵状态
#define gf_set_summon_type_cmd                  (0x0627 | NEED_UPDATE)
//分配经验给召唤兽
#define gf_allocate_exp2summon_cmd              (0x0628 | NEED_UPDATE)
//成长系数调整
#define gf_fresh_summon_attr_cmd                (0x063B | NEED_UPDATE)

#define gf_get_last_summon_nick_cmd				(0x0638 | NEED_UPDATE)
//end summon
//
//----- Begin Numen
#define gf_get_numen_list_cmd				    (0x0606 | NEED_UPDATE)
#define gf_change_numen_status_cmd				(0x0608 | NEED_UPDATE)
#define gf_make_numen_sex_cmd                   (0x0609 )
#define gf_invite_numen_cmd                     (0x060A )
#define gf_change_numen_nick_cmd                (0x060E )
//----- End Numen

#define gf_pick_fruit_cmd						(0x0629 | NEED_UPDATE)
//trade system
#define gf_buy_goods_cmd                        (0x062E | NEED_UPDATE)
#define gf_sell_goods_cmd                       (0x062F | NEED_UPDATE)

//取得角色列表
#define gf_get_role_list_cmd					(0x06E3)

// for php
#define gf_get_role_list_ex_cmd					(0x06E8)

//删除装备
#define gf_del_attire_cmd						(0x060E | NEED_UPDATE)

//增加装备
#define gf_add_attire_cmd						(0x060F | NEED_UPDATE)
//增加角色
#define gf_add_role_cmd							(0x06E1 | NEED_UPDATE)	
//进阶
#define gf_set_power_user_cmd					(0x06E2 | NEED_UPDATE)	

//删除角色
#define gf_gray_delete_role_cmd					(0x06F0 | NEED_UPDATE)	

#define gf_logic_delete_role_cmd				(0x06F2 | NEED_UPDATE)	

#define gf_resume_gray_role_cmd					(0x06F3 | NEED_UPDATE)	

//拉取离线挂机的相关信息
#define gf_get_offline_info_cmd                 (0x06F4 | NEED_UPDATE)
//设置离线挂机的相关信息
#define gf_set_offline_info_cmd                 (0x06EF | NEED_UPDATE)


//物理删除角色
#define gf_del_role_cmd							(0x06F1 | NEED_UPDATE)

//设置角色信息中的int列的值
#define gf_set_role_int_value_cmd				(0x06E5 | NEED_UPDATE)

#define gf_exchange_coins_fight_value_cmd		(0x06EA | NEED_UPDATE)

//设置角色物品-按键绑定信息
#define gf_set_role_itembind_cmd				(0x0646 | NEED_UPDATE)

//取得用户物品绑定信息（php后台管理用）
#define gf_get_role_itembind_cmd						(0x0649 | NEED_UPDATE)

//设置唯一物品获取标志位
#define gf_set_role_uinque_itembit_cmd  (0x064A | NEED_UPDATE) 

//设置角色等级经验等
#define gf_set_role_base_info_cmd		(0x06E6 | NEED_UPDATE)

//设置角色等级经验等
#define gf_set_role_base_info_2_cmd		(0x06E8 | NEED_UPDATE)

//设置角色竞技信息
#define gf_set_role_pvp_info_cmd		(0x06E9 | NEED_UPDATE)


//设置角色等级经验等
#define gf_set_role_stage_info_cmd		(0x06E7 | NEED_UPDATE)


//task cmd
//取得已经完成的任务
#define gf_get_task_finished_cmd				(0x06D1)
#define gf_del_outmoded_task_cmd				(0x0658)

//取得正在进行中的任务
#define gf_get_task_in_hand_cmd					(0x06D2)

//取得全部任务
#define gf_get_all_task_cmd						(0x06D6)

//设置任务进行或完成..的标志
#define gf_set_task_flg_cmd						(0x06D3 | NEED_UPDATE)

//设置任务进行步骤值
#define gf_set_task_value_cmd					(0x06D4 | NEED_UPDATE)

//完成任务时物品上交或奖励
#define gf_task_swap_item_cmd					(0x06D5 | NEED_UPDATE)

#define gf_swap_action_func_cmd					(0x06D6 | NEED_UPDATE)

//增加用户杀死的boss，封魔录
#define gf_add_killed_boss_cmd					(0x06D7 | NEED_UPDATE)

#define gf_get_swap_action_cmd					(0x06D7)

#define gf_clear_swap_action_cmd				(0x07DD | NEED_UPDATE)

//拉取杀死的boss 列表
#define gf_get_killed_boss_cmd					(0x06D8)

//合成装备
#define gf_compose_attire_cmd                   (0x06D8 | NEED_UPDATE)
//分解装备
#define gf_decompose_attire_cmd                 (0x06D9 | NEED_UPDATE)

//取得任务列表
#define gf_get_task_list_cmd                    (0x06D9)

//取得双倍经验时间
#define gf_get_double_exp_data_cmd              (0x06DA)
//设置双倍经验时间
#define gf_set_double_exp_data_cmd              (0x06DA | NEED_UPDATE)
//取得已经阅读时报最大章节
#define gf_get_max_times_chapter_cmd            (0x06DB)
//更新已经阅读时报的最大章节
#define gf_set_max_times_chapter_cmd            (0x06DB | NEED_UPDATE)

//比武大会获胜队伍领取奖励
#define gf_team_member_reward_cmd               (0x06DC | NEED_UPDATE)
//skill cmd
//增加(学习)技能
#define gf_add_skill_points_cmd						(0x06C0 | NEED_UPDATE)

//增加技能
#define gf_add_skill_with_no_book_cmd			(0x06C6 | NEED_UPDATE)


//删除(遗忘)技能
#define gf_del_skill_cmd						(0x06C1 | NEED_UPDATE)

//更新技能info
#define gf_upgrade_skill_cmd					(0x06C2 | NEED_UPDATE)

//取得角色技能列表
#define gf_get_skill_list_cmd					(0x06C3)	

//设置角色技能绑定列表
#define gf_set_skill_bind_key_cmd				(0x06C4 | NEED_UPDATE)	

//取得角色技能绑定列表（php后台管理用）
#define gf_get_skill_bind_key_cmd				(0x06C5)	

#define gf_learn_new_skill_cmd				(0x06C5 | NEED_UPDATE)


#define gf_add_shop_log_cmd						(0x06C7 | NEED_UPDATE)

#define gf_shop_last_log_cmd					(0x06C8 | NEED_UPDATE)

#define gf_upgrade_item_cmd						(0x06C9  | NEED_UPDATE)

//我的师傅是谁
#define gf_get_master_cmd                       (0x06CA  | NEED_UPDATE)
//我的徒弟们
#define gf_get_apprentice_cmd                   (0x06CB  | NEED_UPDATE)
//师傅收徒
#define gf_master_add_apprentice_cmd			(0x06CC  | NEED_UPDATE)
//逐出师门
#define gf_master_del_apprentice_cmd			(0x06CD  | NEED_UPDATE)
//更新师门中徒弟的级别(与获得奖励有关)
#define gf_set_prentice_grade_cmd               (0x06CE  | NEED_UPDATE)
//获取师门已有徒弟的个数
#define gf_get_prentice_cnt_cmd                 (0x06CF  | NEED_UPDATE)

//获取小屋内装饰的列表
#define gf_get_decorate_list_cmd                (0x06DD)
//增加一个新的装饰
#define gf_set_decorate_cmd                     (0x06DE)
//增加装饰的进度
#define gf_add_decorate_lv_cmd                  (0x06DF)


#define gf_report_vesion_cmd					(0x06FE)

//用户登录（php后台管理用）
#define gf_user_login_cmd						(0x06FF)

//GF_OTHER DB
//验证邀请码
#define gf_check_invite_code_cmd				(0x0E00)

//验证用户是否已经输入过邀请码
#define gf_check_user_invited_cmd				(0x0E02)

#define gf_get_invit_code_cmd					(0x0E03)

//用于拉取限定任务
#define gf_get_daily_action_cmd                 (0x0E04)
//用于设置任务限定用
#define gf_set_daily_action_cmd                 (0x0E05)
//设置大使任务邀请信息
#define gf_set_amb_info_cmd						(0x0E06)

#define gf_get_amb_info_cmd						(0x0607 | NEED_UPDATE)

#define gf_notify_use_item_cmd					(0x0E08)

#define gf_get_hero_top_info_cmd                (0x0E09)

#define gf_notify_user_get_shop_item_cmd		(0x0E0A)

#define gf_set_hero_top_info_cmd                (0x0E10)

#define gf_get_hunter_top_info_cmd              (0x0E11)
#define gf_get_hunter_top_info_for_web_cmd			(0x0E13)
#define gf_get_clothes_info_for_web_cmd			(0x062B)
#define gf_set_hunter_top_info_cmd              (0x0E12)
#define gf_get_hunter_info_cmd                  (0x0E18)
#define gf_del_player_place_in_old_top_cmd		  (0x0E14)
#define gf_get_donate_count_cmd                   (0x0E15)
#define gf_add_donate_count_cmd                   (0x0E16)
#define gf_get_ap_toplist_cmd                     (0x0E17)

//天下第一比武大会
#define gf_join_contest_team_cmd                  (0x0E19)
#define gf_get_contest_team_info_cmd              (0x0E1A)
#define gf_get_contest_donate_cmd                 (0x0E1B)
#define gf_contest_donate_plant_cmd               (0x0E1C)
#define gf_get_self_contest_team_cmd              (0x0E1D)

#define gf_get_pvp_game_data_cmd               (0x0E21)
#define gf_set_pvp_game_data_cmd               (0x0E22)
#define gf_set_pvp_game_flower_cmd               (0x0E23)
#define gf_get_pvp_game_rank_cmd               (0x0E24)
#define gf_get_summon_dragon_list_cmd			(0x0E25)
#define gf_add_summon_dragon_cmd				(0x0E26)
#define gf_create_fight_team_cmd				(0x0E27)
#define gf_add_fight_team_member_cmd				(0x0E28)
#define gf_get_team_info_cmd				(0x0E29)
#define gf_del_team_member_cmd				(0x0E2A)
#define gf_del_team_cmd						(0x0E2B)
#define gf_team_info_changed_cmd			(0x0E2C)
#define gf_get_team_top10_cmd				(0x0E2D)
#define gf_contribute_team_cmd				(0x0E2E)
#define gf_fetch_team_coin_cmd				(0x0E2F)
#define gf_team_active_enter_cmd			(0x0E34)
#define gf_get_team_active_top100_cmd		(0x0E35)
#define gf_team_active_score_change_cmd     (0x0E36)

#define gf_set_team_member_level_cmd          (0x0E42)
#define gf_change_team_name_cmd               (0x0E43)
#define gf_change_team_mcast_cmd              (0x0E44)
#define gf_insert_tmp_info_cmd				  (0x0E45)
#define gf_search_team_info_cmd               (0x0E46)
#define gf_gain_team_exp_cmd                  (0x0E47)
#define gf_team_reduce_tax_cmd                (0x0E48)


//for true false wusheng       
#define gf_set_true_wusheng_info_cmd        (0x0E31)
#define gf_list_true_wusheng_info_cmd       (0x0E32)

#define gf_list_ranker_info_cmd             (0x0E37)
#define gf_set_ranker_info_cmd              (0x0E38)

#define gf_set_reward_player_cmd              (0x0E39)
#define gf_get_reward_player_cmd              (0x0E40)
#define gf_set_player_reward_flag_cmd         (0x0E41)

//用于用户拉取仓库信息                          
#define get_warehouse_item_list_cmd             (0x0625)

#define gf_move_item_warehouse_to_bag_cmd        (0x0626)

#define gf_move_item_bag_to_warehouse_cmd        (0x0627)

#define get_warehouse_clothes_item_list_cmd      (0x0628)

#define gf_move_clothes_item_warehouse_to_bag_cmd (0x0629)

#define gf_move_clothes_item_bag_to_warehouse_cmd  (0x062A)
//用户的副职
#define gf_learn_secondary_pro_cmd                     (0x0630)
#define gf_secondary_pro_fuse_cmd                      (0x0631)
#define gf_get_secondary_pro_list_cmd                  (0x0632) 
//邮件
#define gf_mail_head_list_cmd                          (0x0633)
#define gf_mail_body_cmd                               (0x0636)
#define gf_delete_mail_cmd                             (0x0637)
#define gf_take_mail_enclosure_cmd                     (0x0638)
#define gf_send_mail_cmd                               (0x0639)
#define gf_send_system_mail_cmd                        (0x063A)
#define gf_reduce_money_cmd                            (0x0640)
//BOSS击杀数据
#define gf_get_kill_boss_list_cmd                 	   (0x0770)
#define gf_replace_kill_boss_cmd                       (0x0771)
//by cws 0607 for swap 
#define gf_add_swap_cmd					(0x0790)
#define gf_delete_swap_cmd					(0x0791)

//小屋
#define gf_get_home_data_cmd                           (0x0780)
#define gf_set_home_data_cmd                           (0x0681 | NEED_UPDATE)
#define gf_set_home_update_tm_cmd                      (0x0682 | NEED_UPDATE)
//.#define gf_replace_home_exp_level_cmd                  (0x0781)
//#define gf_update_home_active_point_cmd                (0x0782)
//成就
#define gf_get_achievement_data_list_cmd               (0x0772)
#define gf_replace_achievement_data_cmd                (0x0773)
//title
#define gf_get_achieve_title_cmd                (0x0674)
#define gf_add_achieve_title_cmd                (0x0774)
#define gf_set_cur_achieve_title_cmd            (0x0775)
//achievement stat count
#define gf_get_stat_info_cmd                    (0x06BF | NEED_UPDATE)
#define gf_set_stat_info_cmd                    (0x07BE)

//用户的现实状态
#define gf_set_role_state_cmd                       (0x0649)
//用户购买物品限制记录
#define gf_get_buy_item_limit_list_cmd              (0x0650)
#define gf_set_buy_item_limit_data_cmd              (0x0651)
//用户环任务记录
#define gf_get_ring_task_list_cmd                   (0x0652)
#define gf_replace_ring_task_cmd                    (0x0653)
#define gf_delete_ring_task_cmd                     (0x0654)
#define gf_get_ring_task_history_list_cmd           (0x0655)
#define gf_replace_ring_task_history_cmd            (0x0656)
//开箱子
#define gf_set_open_box_times_cmd                   (0x0657)
#define gf_check_update_trade_info_cmd 				(0x065B | NEED_UPDATE)

//extern interface for boss 

#define gf_get_role_base_info_for_boss_cmd			(0x0670)
#define gf_get_player_community_info_cmd			(0x0671)


//for banner btl

#define gf_get_other_active_cmd                        (0x067A | NEED_UPDATE)
#define gf_set_other_active_cmd                        (0x067B | NEED_UPDATE)
#define gf_get_other_active_list_cmd                (0x067C | NEED_UPDATE)
#define gf_set_role_god_guard_cmd                   (0x067D | NEED_UPDATE)
#define gf_get_single_other_active_cmd              (0x067E | NEED_UPDATE)


#define OFFLINE_MSG_MAX_LEN 255
#define MSG_LIST_BUF_SIZE  2000 

#define NICK_LEN	16
#define ACTIVE_BUF_LEN  40
#define MCAST_LEN   240
#define  FRIEND_COUNT_MAX   200 
#define ID_LIST_MAX 200
#define BACK_ID_LIST_MAX 2000
#define ACCESS_COUNT_MAX 100
#define VALUE_MAX 100000000 
#define  MSG_MAX_LEN 2000

#define MAX_MAIL_TITLE_LEN 40 
#define MAX_MAIL_BODY_LEN  300

#define TASK_SER_VALUE_LEN 20
#define TASK_AS_VALUE_LEN 40


enum{
	TYPE_ATTIRE=1,
	TYPE_ITEM,
	TYPE_ROLE,
	TYPE_SKILL,
	TYPE_OTHER,
    TYPE_DVP = 101,
};


struct	stru_count{
	uint32_t	count;
}__attribute__((packed));

struct stru_id{
	userid_t	id;
}__attribute__((packed));

struct stru_id_list{
	uint32_t    count;       
   	userid_t    item[ID_LIST_MAX];
}__attribute__((packed));

struct  gf_reg_in{
	uint32_t sex;
	uint32_t birthday;
	char nick[NICK_LEN];
}__attribute__((packed));

typedef stru_count gf_get_xiaomee_out;

typedef stru_id gf_add_friend_in;
typedef stru_id gf_add_black_in;
typedef stru_id gf_del_friend_in;
typedef stru_id gf_del_friend_whatever_in;
typedef stru_id gf_add_black_out_header;


typedef gf_del_friend_in gf_del_black_in;

typedef stru_count gf_get_blacklist_out_header;
typedef stru_count gf_get_friendlist_out_header;
typedef stru_count gf_get_friendlist_type_out_header;
//typedef stru_count gf_get_role_list_out_header;
typedef stru_count gf_get_role_list_ex_out_header;

struct gf_get_role_list_out_header{
    uint32_t forbid_flag;
    uint32_t deadline;
    uint32_t vip_is;
    uint32_t x_value;
    uint32_t end_time;
    uint32_t count;
}__attribute__((packed));


struct gf_role_login_out_header{
	uint32_t vip;
	uint32_t friend_count;
	uint32_t black_count;
}__attribute__((packed));


struct gf_role_login_out_item_1{
	userid_t	friend_id;
}__attribute__((packed));

struct gf_role_login_out_item_2{
	userid_t	black_id;
}__attribute__((packed));

struct stru_friedid_list{
	uint32_t id;
}__attribute__((packed));


typedef stru_id gf_get_friendlist_out_item;
typedef stru_id gf_get_blacklist_out_item;

struct gf_get_friendlist_type_out_item{
	uint32_t	friend_id;
	uint32_t	friend_type;
}__attribute__((packed));


struct gf_get_user_info_out_item_1{
	uint32_t  attireid;
	uint32_t  id;
	uint32_t  duration;
}__attribute__((packed));


struct gf_get_user_info_out_item_2{
	uint32_t skill_id;
	uint32_t skill_lv;
	uint32_t bind_key;
}__attribute__((packed));
typedef gf_get_user_info_out_item_2 gf_skill_bind_list;

#define UNIQUE_ITEM_LEN 20
#define ITEM_BIND_LEN 40
#define STAGE_BIT_LEN 200
#define SKILL_BIND_LEN 200


struct stru_gf_info_out{
	uint32_t  role_regtime;
	uint32_t  role_type;
	uint32_t  power_user;
	uint32_t  flag;
	uint32_t  regtime;
	char      nick[NICK_LEN];
    uint32_t  nick_change_tm;

	uint32_t  vip;
	uint32_t  vip_month_cnt;
	uint32_t  vip_start_tm;
	uint32_t  vip_end_tm;
	uint32_t  vip_x_val;
	uint32_t  sword_value;
	uint32_t  vip_exwarehouse;

	uint32_t  skill_point;
    uint32_t  map_id;
    uint32_t  xpos;
    uint32_t  ypos;

	uint32_t  level;
	uint32_t  exp;
	uint32_t  alloter_exp;
	uint32_t  hp;
	uint32_t  mp;
	uint32_t  xiaomee;
    uint32_t  honour;
    uint32_t  fight;
    uint32_t  win;
    uint32_t  fail;
    uint32_t  winning_streak;
	uint32_t  fumo_points_today;
	uint32_t  fumo_points_total;
	uint32_t  fumo_tower_top;
	uint32_t  fumo_tower_used_tm;
	uint32_t  Ol_count;
	uint32_t  Ol_today;
	uint32_t  Ol_last;
	uint32_t  Ol_time;
	uint8_t   uniqueitem[UNIQUE_ITEM_LEN];
	uint8_t   itembind[ITEM_BIND_LEN];
    uint8_t   amb_status;
    uint32_t  parentid;
    uint32_t  child_cnt;
    uint32_t  achieve_cnt;
    uint8_t   flag_bit[40];
    uint8_t   act_record[40];
    uint32_t  double_exp_time;
    uint32_t  day_flag;
    uint32_t  max_times_chapter;
	uint32_t  show_state;
	uint32_t  open_box_times;
	uint32_t  strengthen_cnt;
	uint32_t  achieve_point;
	uint32_t  last_update_tm;
	uint32_t  achieve_title;
	uint32_t  forbiden_add_friend_flag;
	uint32_t  home_active_point;
	uint32_t  home_last_tm;
	uint32_t  vitality_point; 
	uint32_t  team_id;
    uint8_t   god_guard[80];
}__attribute__((packed));


struct gf_get_user_info_out_header{
	stru_gf_info_out	user_info;
	uint32_t			msg_size;
	uint32_t			used_clothes_count;
	uint32_t			skill_count;
//	uint32_t			friend_count;
//	uint32_t            black_count;
}__attribute__((packed));

//offline message
struct gf_get_info_for_login_out_item1{
    uint8_t     offline_msg[];
}__attribute__((packed));

//used clothes message
struct gf_get_info_for_login_out_item2{
    uint32_t    clothes_id;
    uint32_t    unique_id;
    uint32_t    duration;
}__attribute__((packed));

//learn skill message
struct gf_get_info_for_login_out_item3{
    uint32_t    skill_id;
    uint32_t    skill_lv;
    uint32_t    skill_key;
}__attribute__((packed));

//finished task message
struct gf_get_info_for_login_out_item4{
    uint32_t    task_id;
}__attribute__((packed));

//doing task message
struct gf_get_info_for_login_out_item5{
    uint32_t    task_id;
    uint8_t     sbuf[20];
    uint8_t     cbuf[40];
}__attribute__((packed));

//cancel daily task message
struct gf_get_info_for_login_out_item6{
    uint32_t    task_id;
}__attribute__((packed));

//packages clothes message
struct gf_get_info_for_login_out_item7{
    uint32_t    clothes_id;
    uint32_t    unique_id;
    uint32_t    duration;
}__attribute__((packed));

//packages item message
struct gf_get_info_for_login_out_item8{
    uint32_t    item_id;
    uint32_t    cnt;
}__attribute__((packed));

//finished stage message
struct gf_get_info_for_login_out_item9{
    uint32_t    stage_id;
    uint8_t     gread[6];
}__attribute__((packed));

struct gf_get_info_for_login_out_header{
	stru_gf_info_out	user_info;
	uint32_t			msg_size;
	uint32_t			used_clothes_count;
	uint32_t			skill_count;
    uint32_t            done_tasks_num;
    uint32_t            doing_tasks_num;
    uint32_t            cancel_tasks_num;
    uint32_t            pkg_clothes_cnt;
    uint32_t            pkg_item_cnt;
    uint32_t            fumo_stage_cnt;
	uint32_t			db_buff_cnt;
//	uint32_t			friend_count;
//	uint32_t            black_count;
}__attribute__((packed));


struct pvp_header{
    uint32_t  honour;
    uint32_t  fight;
    uint32_t  win;
    uint32_t  fail;
    uint32_t  winning_streak;
}__attribute__((packed));

struct  gf_get_user_base_info_out_header{
	uint32_t  flag;
	uint32_t  regtime;
	uint32_t  forbid_flag;
	uint32_t  deadline;
	uint32_t  vip;
	uint32_t  vip_month_count;
	uint32_t  start_time;
	uint32_t  end_time;
	uint32_t  auto_incr;
	uint32_t  x_value;
	uint32_t  max_bag;
	uint32_t  Ol_count;
	uint32_t  Ol_today;
	uint32_t  Ol_last;
	uint32_t  Ol_time;
    uint8_t   amb_status;
    uint32_t  parentid;
    uint32_t  child_cnt;
    uint32_t  achieve_cnt;
}__attribute__((packed));

struct  gf_set_user_base_info_in{
	uint32_t  flag;
	uint32_t  regtime;
	uint32_t  vip;
	uint32_t  Ol_count;
	uint32_t  Ol_today;
	uint32_t  Ol_last;
	uint32_t  Ol_time;
    uint8_t   amb_status;
    uint32_t  parentid;
    uint32_t  child_cnt;
    uint32_t  achieve_cnt;
}__attribute__((packed));

typedef stru_count gf_get_clothes_list_out_header;
typedef stru_count gf_get_clothes_list_ex_out_header;

struct gf_get_clothes_list_out_item{
	uint32_t	attireid;
	uint32_t	id;	
	uint32_t    duration;
	uint32_t    attirelv;
	uint32_t    gettime;
	uint32_t    timelag;
	uint32_t    usedflag;
}__attribute__((packed));
struct gf_get_clothes_list_ex_out_item{
	uint32_t	attireid;
	uint32_t	id;
	uint32_t	usedflag;
	uint32_t 	duration;
}__attribute__((packed));

typedef stru_count gf_get_strengthen_material_list_out_header;
struct gf_get_strengthen_material_list_out_item{
    uint32_t    material_id;
    uint32_t    cnt;
}__attribute__((packed));

struct gf_strengthen_attire_in {
    uint32_t    coin;
    uint32_t    strengthen_fail_cnt;
    uint32_t    attireid;
    uint32_t    uniquekey;
    uint32_t    attirelv;
    uint32_t    material_id;
    uint32_t    material_cnt;
    uint32_t    stoneid_1;
    uint32_t    stone_1_cnt;
    uint32_t    stoneid_2;
}__attribute__((packed));
//added by cws 0620
struct gf_strengthen_attire_without_material_in {
    uint32_t    uniquekey;
    uint32_t    attirelv;
}__attribute__((packed));

typedef gf_strengthen_attire_without_material_in gf_strengthen_attire_without_material_out;
typedef gf_strengthen_attire_in gf_strengthen_attire_out;

struct  gf_wear_clothes_in_header
{
	uint32_t max_bag_grid_count;
	uint32_t count;
}__attribute__((packed));

struct gf_wear_clothes_in_item{
	uint32_t	id;
	uint32_t 	equip_part;
}__attribute__((packed));

typedef stru_count gf_wear_clothes_out_header;
struct gf_wear_clothes_out_item{
	uint32_t	clothes_id;
	uint32_t	id;
	uint32_t	duration;
	uint32_t	attirelv;
	uint32_t	gettime;
	uint32_t	endtime;
}__attribute__((packed));

struct  gf_take_off_clothes_in
{
	uint32_t unique_id;
}__attribute__((packed));


typedef stru_count gf_get_user_item_list_out_header;
struct gf_get_user_item_list_out_item{
    uint32_t        itemid;
    uint32_t        count;
}__attribute__((packed));

struct gf_get_package_iclist_out_header
{
    uint32_t  clothes_count;
    uint32_t  item_count;
}__attribute__((packed));

struct gf_get_package_iclist_out_item_1{
    uint32_t        attireid;
    uint32_t        id;
    uint32_t        duration;
    uint32_t        gettime;
    uint32_t        timelag;
}__attribute__((packed));

struct gf_get_package_iclist_out_item_2{
    uint32_t        itemid;
    uint32_t        count;
}__attribute__((packed));

struct gf_set_nick_in{
	char	nick[NICK_LEN];
	uint32_t use_item;
}__attribute__((packed));

struct gf_get_nick_out{
    char    nick[NICK_LEN];
}__attribute__((packed));

struct gf_set_role_state_in
{
	uint32_t state;
}__attribute__((packed));

struct gf_set_open_box_times_in
{
	uint32_t times;
}__attribute__((packed));

struct gf_check_update_trade_info_in_header 
{
	uint32_t itm_cnt;
	uint32_t attire_cnt;
}__attribute__((packed));

struct gf_check_update_trade_info_in_item_1
{
	uint32_t item_id;
	uint32_t item_cnt;
}__attribute__((packed));

struct gf_check_update_trade_info_in_item_2
{
	uint32_t attire_id;
	uint32_t unique_id;
}__attribute__((packed));

struct gf_insert_tmp_info_in
{
	uint32_t type;
	uint32_t id;
	uint32_t value;
}__attribute__((packed));

struct gf_forbiden_add_friend_flag_in
{
	uint32_t flag;
}__attribute__((packed));
/*
struct gf_query_forbiden_friend_flag_in
{
	uint32_t mimi_id;
	uint32_t flag;
}__attribute__((packed));
*/
struct gf_query_forbiden_friend_flag_out
{
	uint32_t err_code;
	uint32_t mimi_id;
	uint32_t flag;
}__attribute__((packed));


struct gf_donate_count_out
{
	uint32_t red;
	uint32_t black;
}__attribute__((packed));

struct gf_add_buff_in
{
	uint32_t mutex_type;
	uint32_t buff_type;
	uint32_t duration;
	uint32_t start_tm;
}__attribute__((packed));

struct gf_add_buff_out_header
{
	uint32_t buff_type;
	uint32_t duration;
	uint32_t mutex_type;
	uint32_t start_tm;
}__attribute__((packed));

struct gf_update_buff_list_in_header
{
	uint32_t count;
}__attribute__((packed));

struct gf_update_buff_list_in_item
{
	uint32_t buff_type;
	uint32_t duration;
	uint32_t mutex_type;
	uint32_t start_tm;
}__attribute__((packed));

struct gf_del_buff_on_player_in 
{
	uint32_t buff_type;
}__attribute__((packed));

struct gf_notify_use_item_in
{
	uint32_t item_id;
	uint32_t item_cnt;
}__attribute__((packed));

struct gf_notify_user_get_shop_item_in
{
	uint32_t item_id;
	uint32_t item_cnt;
}__attribute__((packed));

struct get_user_partial_userpart{
	uint32_t        userid;
	uint32_t        role_regtime;
	uint32_t         role_type;
	uint32_t        power_user;
	uint32_t        lv;
    char            nick[NICK_LEN];
	//added by cws 20120510
    uint8_t         datas[40];

    //	uint32_t        skill_point;
    uint32_t        vip;
}__attribute__((packed));

struct gf_get_role_detail_info_in{
	userid_t  	userid;
	userid_t	role_time;
}__attribute__((packed));


struct gf_get_role_detail_info_out{
	userid_t		userid;
	uint32_t        level;
	uint32_t        exp;
	//uint32_t        hp;
	//uint32_t        mp;
	uint32_t        honour;
	uint32_t        fight;
	uint32_t        win;
	uint32_t        fail;
	uint32_t        winning_streak;
	uint32_t        achieve_title;
    uint8_t         datas[40];
}__attribute__((packed));

struct get_user_partial_attirecnt{
	uint32_t		attire_cnt;
}__attribute__((packed));

struct gf_get_user_partial_info_in{
	userid_t  	userid;
	userid_t	role_time;
}__attribute__((packed));

struct gf_get_user_partial_info_out_header{
	get_user_partial_userpart      userpart;
    uint32_t    x_value;
    uint32_t    start_time;
    uint32_t    end_time;
	get_user_partial_attirecnt     attirecnt;
}__attribute__((packed));


struct gf_get_user_partial_info_out_item{
	uint32_t        id;
	uint32_t        gettime;
	uint32_t        endtime;
	uint32_t        attirelv;
}__attribute__((packed));

struct gf_get_role_list_out_item_1{
	uint32_t 		role_time;
	uint32_t		role_type;
	uint32_t		del_tm;
	uint32_t		Ol_last;
	uint32_t		level;
	uint32_t		exp;
	char			nick[NICK_LEN];
	uint32_t		clothes_count;
}__attribute__((packed));


struct gf_get_logic_del_role_list{
	uint32_t 		role_time;
	uint32_t		del_tm;
}__attribute__((packed));

struct gf_attire_id_list{
	uint32_t  attireid;
	uint32_t  id;
}__attribute__((packed));


struct gf_attire_id_duration_list{
	uint32_t  attireid;
	uint32_t  id;
	uint32_t  duration;
	uint32_t  attirelv;
	uint32_t  gettime;
	uint32_t  timelag;
}__attribute__((packed));

struct gf_get_role_list_out_item_2{
	uint32_t  attireid;
	uint32_t  id;
	uint32_t  duration;
	uint32_t  attirelv;
}__attribute__((packed));

struct gf_get_role_list_ex_out_item_1{
	uint32_t 		role_time;
	uint32_t		role_type;
	uint32_t		level;
	uint32_t		status;
	char			nick[NICK_LEN];
	
//	uint32_t 		vip;
//	uint32_t		Ol_count;
//	uint32_t		Ol_today;
//	uint32_t		Ol_last;
//	uint32_t		Ol_time;

}__attribute__((packed));

//typedef gf_attire_id_list gf_get_role_list_ex_out_item_2;

struct stru_msg_item{
	uint32_t	msglen;
	char		msg[OFFLINE_MSG_MAX_LEN];
}__attribute__((packed));

struct gf_add_offline_msg_in_header{
	uint32_t	msglen;
}__attribute__((packed));

#define msg_list_head_len 8
struct stru_msg_list{
	uint32_t len;
	uint32_t count;
	char	 buf[MSG_LIST_BUF_SIZE ];
} __attribute__((packed)) ;

struct stru_role_info{
	uint32_t role_regtime;
	uint32_t role_type;
	uint32_t power_user;
	uint32_t Ol_count;
	char nick[NICK_LEN];
    uint32_t nick_change_tm;
	uint32_t skill_point;
    uint32_t map_id;
    uint32_t xpos;
    uint32_t ypos;
	uint32_t level;
	uint32_t exp;
	uint32_t alloter_exp;
	uint32_t hp;
	uint32_t mp;
	uint32_t xiaomee;
    uint32_t honour;
    uint32_t fight;
    uint32_t win;
    uint32_t fail;
    uint32_t winning_streak;
	uint32_t fumo_points;
	uint32_t fumo_points_today;
	uint32_t fumo_tower_top;
	uint32_t fumo_tower_used_tm;
	uint8_t  uniqueitem[UNIQUE_ITEM_LEN];
	uint8_t  itembind[ITEM_BIND_LEN];
    uint32_t double_exp_time;
    uint32_t day_flag;
    uint32_t max_times_chapter;
	uint32_t show_state;
	uint32_t open_box_times;
	uint32_t strengthen_cnt;
	uint32_t achieve_point;
	uint32_t last_update_tm;
	uint32_t achieve_title;
	uint32_t forbiden_add_friend_flag;
	uint32_t home_active_point;
	uint32_t home_last_tm;
	uint32_t vitality_point;
	uint32_t team_id;
    uint8_t god_guard[80];
}__attribute__((packed));


struct gf_del_attire_in{
	uint32_t attireid;
	uint32_t index_id;
}__attribute__((packed));

struct gf_del_attire_out{
	uint32_t attireid;
	uint32_t index_id;
}__attribute__((packed));


struct gf_add_attire_in{
	uint32_t attireid;
	uint32_t gettime;
	uint32_t usedflag;
	uint32_t duration;
	uint32_t max_bag_grid_count;
}__attribute__((packed));


struct gf_add_role_in{
	uint32_t role_type;//
	uint32_t level;//
	uint32_t hp;
	uint32_t mp;
	char     nick[NICK_LEN];//
    uint32_t parentid;
	uint32_t parent_role_tm;
}__attribute__((packed));

struct gf_add_role_out{
	uint32_t role_num;
	uint32_t user_existed;

	uint32_t parentid;
	uint32_t parent_role_tm;
}__attribute__((packed));


struct gf_set_role_int_value_in{
	char column_name[32];
	uint32_t	value;
}__attribute__((packed));

struct gf_exchange_coins_fight_value_in {
	uint32_t type; //   1: coins -> fight_value   2: fight_value -> coins
	uint32_t coins;
	uint32_t fight_value;
};

struct gf_exchange_coins_fight_value_out {
	uint32_t coins;
	uint32_t fight_value;
};

struct gf_set_role_itembind_in{
	uint8_t  buf[ITEM_BIND_LEN];
}__attribute__((packed));

struct gf_set_role_itembind_out{
	uint8_t  buf[ITEM_BIND_LEN];
}__attribute__((packed));

struct gf_get_role_itembind_out{
	uint8_t  buf[ITEM_BIND_LEN];
}__attribute__((packed));

struct gf_get_double_exp_data_out{
    uint32_t day_flag;
    uint32_t dexp_tm;
}__attribute__((packed));

struct gf_set_double_exp_data_in{
    uint32_t day_flag;
    uint32_t dexp_tm;
}__attribute__((packed));

struct gf_set_max_times_chapter_in{
    uint32_t now_chapter;
}__attribute__((packed));

struct gf_set_item_int_value_in{
	char column_name[32];
	uint32_t    itemid;
	uint32_t	value;
}__attribute__((packed));

struct gf_add_item_in{
	uint32_t    itemid;
	uint32_t	add_num;
	uint32_t    max_item_bag_grid_count;
}__attribute__((packed));

struct gf_del_item_in{
	uint32_t    itemid;
	uint32_t	del_num;
}__attribute__((packed));

struct gf_del_item_out{
	uint32_t    itemid;
	uint32_t	del_num;
}__attribute__((packed));

struct gf_del_items_in_header{
	uint32_t    count;
}__attribute__((packed));

struct gf_del_items_in_item{
	uint32_t    itemid;
	uint32_t 	count;
}__attribute__((packed));

struct gf_bitch_sell_items_in_header{
	uint32_t    type;
	uint32_t    coins;
	uint32_t    cnt;
}__attribute__((packed));

struct gf_bitch_sell_items_in_item{
	uint32_t    itemid;
	uint32_t 	count;
}__attribute__((packed));


struct gf_get_friendlist_type_in{
	uint32_t friend_type;
}__attribute__((packed));

struct gf_buy_attire_in_header{
	uint8_t  flag;
	uint32_t cost;
	uint32_t max_bag_grid_count;
	uint32_t count;
}__attribute__((packed));


struct gf_buy_attire_in_item{
	uint32_t attireid;
	uint32_t gettime;
	uint32_t attire_rank;
	uint32_t duration;
}__attribute__((packed));


struct gf_buy_attire_out_header{
    uint8_t  flag;
 	uint32_t coins_left;
	uint32_t count;
}__attribute__((packed));

struct gf_buy_attire_out_item{
	uint32_t attireid;
	uint32_t id;
}__attribute__((packed));

struct gf_buy_goods_in{
    uint32_t type;
    uint32_t goodsid;
    uint32_t itemid;
    uint32_t uniqueid;
    uint32_t cnt;
    uint32_t duration;
    uint32_t attirelv;
    uint32_t gettime;
    uint32_t endtime;
    uint32_t coins;
    uint32_t max_bag;
}__attribute__((packed));
typedef gf_buy_goods_in gf_buy_goods_out;

struct gf_sell_goods_in{
	uint32_t type;
	uint32_t shop_start_tm;
    uint32_t shop_id;
    uint32_t goodsid;
    uint32_t itemid;
    uint32_t uniqueid;
	uint32_t attire_lv;
    uint32_t cnt;
    uint32_t coins;
}__attribute__((packed));
struct gf_sell_goods_out {
    uint32_t err_code;
    uint32_t type;
    uint32_t shop_id;
    uint32_t goodsid;
    uint32_t itemid;
    uint32_t uniqueid;
    uint32_t cnt;
    uint32_t coins;
}__attribute__((packed));

struct gf_move_item_warehouse_to_bag_in
{ 
	uint32_t item_id;
    uint32_t count;
    uint32_t cost;
	uint32_t max_item_bag_grid_count;
}__attribute__((packed));

struct gf_move_item_warehouse_to_bag_out
{
	uint32_t item_id;
    uint32_t count;
    uint32_t coins_left;
}__attribute__((packed));


struct gf_move_item_bag_to_warehouse_in
{ 
        uint32_t item_id;
        uint32_t count;
        uint32_t cost;
		uint32_t max_warehouse_grid_count;
}__attribute__((packed));

struct gf_move_item_bag_to_warehouse_out
{
        uint32_t item_id;
        uint32_t count;
        uint32_t coins_left;
}__attribute__((packed));


struct gf_move_clothes_item_bag_to_warehouse_in
{
	uint32_t item_id;
	uint32_t id;
	uint32_t cost;
	uint32_t max_warehouse_grid_count;
}__attribute__((packed));

struct gf_move_clothes_item_bag_to_warehouse_out
{
    uint32_t old_id;
    uint32_t item_id;
    uint32_t id;
    uint32_t get_time;
    uint32_t attire_rank;
    uint32_t duration;
	uint32_t end_time;
	uint32_t attire_lv;
    uint32_t coins_left;
}__attribute__((packed));


struct gf_move_clothes_item_warehouse_to_bag_in
{
    uint32_t item_id;
    uint32_t id;
    uint32_t cost;
	uint32_t max_bag_grid_count;
}__attribute__((packed));

struct gf_move_clothes_item_warehouse_to_bag_out
{
    uint32_t old_id;
    uint32_t item_id;
    uint32_t id;
    uint32_t get_time;
    uint32_t attire_rank;
    uint32_t duration;
	uint32_t end_time;
	uint32_t attire_lv;
    uint32_t coins_left;
}__attribute__((packed));

struct gf_buy_item_in{
	uint32_t itemid;
	uint32_t count;
	uint32_t max;
	uint32_t cost;
	uint32_t max_item_bag_grid_count;
}__attribute__((packed));

struct gf_buy_item_out{
 	uint32_t coins_left;
	uint32_t itemid;
	uint32_t count;
}__attribute__((packed));

struct gf_sell_item_in{
	uint32_t	item_id;
	uint32_t	item_cnt;
	uint32_t	price;
}__attribute__((packed));

struct gf_sell_item_out{
	uint32_t	item_id;
	uint32_t	item_cnt;
	uint32_t	left_coins;
}__attribute__((packed));

struct gf_sell_attire_in{
	uint32_t	attire_id;
	uint32_t	index_id;
	uint32_t	price;
}__attribute__((packed));

struct gf_sell_attire_out{
	uint32_t	attire_id;
	uint32_t	index_id;
	uint32_t	left_coins;
}__attribute__((packed));

struct gf_repair_attire_in{
	uint32_t	attire_id;
	uint32_t	index_id;
	uint32_t	per_price;
	uint32_t	max_duration;
}__attribute__((packed));

struct gf_repair_all_attire_in_header{
	uint32_t	attire_cnt;
}__attribute__((packed));

struct gf_repair_all_attire_in_item{
	uint32_t	attire_id;
	uint32_t	index_id;
	uint32_t	per_price;
	uint32_t	max_duration;
}__attribute__((packed));

struct gf_repair_all_attire_out_header{
	uint32_t	left_coins;
	uint32_t	attire_cnt;
}__attribute__((packed));

struct gf_repair_all_attire_out_item{
	uint32_t	attire_id;
	uint32_t	index_id;
	uint32_t	duration;
}__attribute__((packed));

struct gf_repair_attire_out{
	uint32_t	attire_id;
	uint32_t	index_id;
	uint32_t	left_coins;
	uint32_t	duration;
}__attribute__((packed));

/*compose attire*/
struct gf_compose_attire_in_header{
    uint32_t method_id;
	uint32_t max_bag_grid_count;
	uint32_t coins;
	uint32_t is_clothes;
	uint32_t max_num;
    uint32_t attire_id;
    uint32_t gettime;
    uint32_t rank;
    uint32_t duration;
	uint32_t del_item;
	uint32_t random_item;
    uint32_t item_cnt;
}__attribute__((packed));

struct gf_compose_attire_in_item{
	uint32_t is_clothes;
    uint32_t item_id;
    uint32_t cnt;
}__attribute__((packed));

struct gf_compose_attire_out{
    uint32_t left_coins;
	uint32_t is_clothes;
    uint32_t attire_id;
    uint32_t unique_id;
	uint32_t duration;
    uint32_t formula;
	uint32_t del_item;
	uint32_t random_item;
    uint32_t item_cnt;
}__attribute__((packed));

/*decompose attire*/
struct gf_decompose_attire_in_header{
    uint32_t attire_id;
    uint32_t item_cnt;
	uint32_t max_item_bag_grid_count;
}__attribute__((packed));

struct gf_decompose_attire_in_item{
    uint32_t item_id;
    uint32_t cnt;
}__attribute__((packed));


struct gf_battle_use_item_in{
	uint32_t itemid;
	uint32_t count;
	uint32_t market_type;
}__attribute__((packed));

struct gf_battle_use_item_out{
	uint32_t itemid;
	uint32_t count;
}__attribute__((packed));

struct gf_pick_up_item_in_header{
	uint32_t itemid;
	uint32_t type;
	uint32_t max_num;
	uint32_t duration;
	uint32_t max_item_bag_grid_count;
	uint32_t unique_pick_cnt;
	uint8_t	 uniqueitem[];
}__attribute__((packed));


struct gf_pick_up_item_out{
    uint32_t itemid;
	uint32_t index_id;
}__attribute__((packed));


struct gf_pick_up_roll_item_in_header{
	uint32_t roll_id;
	uint32_t itemid;
	uint32_t type;
	uint32_t max_num;
	uint32_t duration;
	uint32_t max_item_bag_grid_count;
	uint32_t unique_pick_cnt;
	uint8_t  uniqueitem[];
}__attribute__((packed));


struct gf_pick_up_roll_item_out{
	uint32_t err_code;
	uint32_t roll_id;
	uint32_t itemid;
	uint32_t index_id;
}__attribute__((packed));



struct gf_set_role_uinque_itembit_in{
    uint8_t  uniqueitem[UNIQUE_ITEM_LEN];
}__attribute__((packed));

struct gf_set_role_base_info_in_header{
	uint32_t level;
	uint32_t exp;
    uint32_t alloter_exp;
    uint32_t dexp_tm;
	uint32_t coins;
	uint32_t skill_point;
	uint32_t fumo_points;
	uint32_t fumo_tower_top;
	uint32_t fumo_tower_used_tm;
	uint32_t attire_cnt;
}__attribute__((packed));

struct gf_set_role_base_info_in_item{
	uint32_t attire_index;//装备序列号，数据库自动生成
	uint32_t duration;//耐久
}__attribute__((packed));

struct gf_set_role_base_info_2_in_header{
	uint32_t level;
	uint32_t exp;
	uint32_t coins;
}__attribute__((packed));

struct  gf_set_role_pvp_info_in{
	uint32_t  honour;
	uint32_t  fight;
	uint32_t  win;
	uint32_t  fail;
	uint32_t  winning_streak;
}__attribute__((packed));

struct gf_set_role_stage_info_in{
	uint32_t	stageid;
	uint32_t	bossid;
	uint32_t	difficulty;
	uint32_t	stage_grade;
	uint32_t	total_dam;
}__attribute__((packed));

//task struct
struct gf_set_task_flg_in{
	uint32_t taskid;
	uint32_t flag;
}__attribute__((packed));

typedef stru_id gf_set_task_flg_out;
struct gf_set_task_value_in_header{
	uint32_t taskid;
	uint32_t value_type;
	uint8_t  binary_arr[];
}__attribute__((packed));

typedef stru_count gf_get_task_finished_out_header;

struct gf_get_task_finished_out_item{
	uint32_t taskid;
}__attribute__((packed));

struct gf_del_outmoded_task_in_header {
    uint32_t task_num;
}__attribute__((packed));

struct gf_del_outmoded_task_in_item {
    uint32_t task_id;
}__attribute__((packed));

typedef stru_count gf_get_task_in_hand_out_header;

struct gf_get_task_in_hand_out_item{
	uint32_t taskid;
	uint8_t  ser_value[TASK_SER_VALUE_LEN];
	uint8_t  as_value[TASK_AS_VALUE_LEN];
}__attribute__((packed));

struct gf_get_task_cancel_out_item{
    uint32_t taskid;
}__attribute__((packed));

typedef stru_count gf_get_all_task_out_header;

struct gf_get_all_task_out_item{
	uint32_t taskid;
	uint32_t task_flg;
    //added by cws 20120502
    uint32_t task_tm; 
	//uint8_t  ser_value[TASK_SER_VALUE_LEN];
	//uint8_t  as_value[TASK_AS_VALUE_LEN];
}__attribute__((packed));

struct gf_get_task_list_out_header{
	uint32_t done_task_num;
	uint32_t doing_task_num;
    uint32_t cancel_task_num;
}__attribute__((packed));

struct gf_set_task_value_out{
	uint32_t value_type;
}__attribute__((packed));

struct gf_task_swap_item_in_item_1{
	uint32_t type;
	uint32_t id;
	uint32_t count;
}__attribute__((packed));

struct gf_task_swap_item_in_item_2{
	uint32_t type;
	uint32_t id;
	uint32_t count;
	uint32_t duration;
	uint32_t lifetime;
	uint32_t max;
}__attribute__((packed));

struct gf_task_swap_item_in_header{
	uint32_t res_limit;
	uint32_t del_count;
	uint32_t add_count;
	uint32_t max_item_bag_grid_count;
}__attribute__((packed));

struct gf_task_swap_item_out_header{
	uint32_t left_coins;
	uint32_t exp;
	uint32_t allocator_exp;
	uint32_t skill_pt_left;
	uint32_t fumo_pt_left;
	uint32_t honor;
	uint32_t attire_cnt;
}__attribute__((packed));

struct gf_task_swap_item_out_item{
	uint32_t attireid;
	uint32_t index_id;
	uint32_t gettime;
	uint32_t endtime;
}__attribute__((packed));

//skill struct
struct gf_add_skill_points_in{
	uint32_t skill_book_id;
	uint32_t skill_points_add;
}__attribute__((packed));

struct gf_add_skill_with_no_book_in{
	uint32_t skill_id;
	uint32_t skill_lv;
}__attribute__((packed));


struct gf_add_skill_points_out{
	uint32_t skill_book_id;
	uint32_t left_points;
}__attribute__((packed));

struct gf_add_skill_with_no_book_out{
	uint32_t skill_id;
}__attribute__((packed));


struct gf_del_skill_in{
	uint32_t skill_id;
}__attribute__((packed));

struct gf_learn_new_skill_in{
	uint32_t skill_id;
	uint32_t skill_lv; //default level
	uint32_t skill_point;//skill points needed
	uint32_t item_id;//item needed
}__attribute__((packed));

struct gf_learn_new_skill_out{
	uint32_t skill_id;
	uint32_t skill_lv;
	uint32_t left_point;//skill points needed
	uint32_t item_id;//item needed
}__attribute__((packed));

struct gf_upgrade_skill_in{
	uint32_t skill_id;
	uint32_t skill_lv; //to skill lv
	uint32_t skill_point; // needed by upgrade
	uint32_t coins; // needed by upgrade
}__attribute__((packed));

struct gf_upgrade_skill_out{
	uint32_t skill_id;
	uint32_t skill_lv;
	uint32_t left_coins;
	uint32_t use_point;
	uint32_t left_point; 
}__attribute__((packed));

struct gf_reset_skill_in {
	uint32_t used_sp;
}__attribute__((packed));


struct gf_reset_skill_out_header {
	uint32_t left_skill_points;
}__attribute__((packed));

struct gf_get_skill_list_out_header {
	uint32_t left_sp;
	uint32_t count;
};

struct gf_get_skill_list_out_item{
	uint32_t skill_id;
	uint32_t skill_lv;
	uint32_t skill_point;
}__attribute__((packed));


typedef stru_count gf_set_skill_bind_key_in_header;

struct gf_set_skill_bind_key_in_item{
	uint32_t skill_id;
	uint32_t bind_key;
}__attribute__((packed));

typedef stru_count gf_set_skill_bind_key_out_header;

struct gf_set_skill_bind_key_out_item{
	uint32_t skill_id;
	uint32_t skill_lv;
	uint32_t bind_key;
}__attribute__((packed));

typedef stru_count gf_get_skill_bind_key_out_header;

struct gf_get_skill_bind_key_out_item{
	uint32_t skill_id;
	uint32_t skill_lv;
	uint32_t bind_key;
}__attribute__((packed));

struct gf_skill_bind_stru{
	uint32_t skill_id;
	uint32_t bind_key;
}__attribute__((packed));

struct gf_game_input_in_header{
	uint32_t	coins;
	uint32_t	exp;
	uint32_t	item_cnt;
}__attribute__((packed));

struct gf_game_input_in_item{
	uint32_t item_id;
	uint32_t count;
	uint32_t max_count;
	uint32_t lifetime;
	uint32_t item_lv;
}__attribute__((packed));

struct gf_game_input_out_header{
	uint32_t	coins_left;
	uint32_t	coins_adj;
	uint32_t	exp;
	uint32_t	item_cnt;
}__attribute__((packed));

struct gf_game_input_out_item{
	uint32_t item_id;
	uint32_t count;
	uint32_t max_count;
	uint32_t lifetime;
	uint32_t item_lv;
}__attribute__((packed));

struct gf_attire_set_int_value_in{
	uint32_t	indexid;
	char column_name[32];
	uint32_t	value;
}__attribute__((packed));

//begin summon
struct gf_get_summon_list_out_header{
    uint32_t count;
}__attribute__((packed));

struct sum_skill_t{
    uint32_t    skillid;
    uint32_t    skilllv;
}__attribute__((packed));

struct gf_get_summon_list_out_item{
    uint32_t mon_tm;
    uint32_t mon_type;
    char mon_nick[NICK_LEN];
    uint32_t exp;
    uint32_t lv;
    uint32_t fight_value;
    uint32_t status;
	int add_per;
    //uint8_t skill_id[50];
    uint32_t skill_cnt;
    sum_skill_t skill_data[max_summon_skills];
}__attribute__((packed));

struct gf_hatch_summon_in{
    uint32_t item_id;
    uint32_t mon_type;
	uint32_t pre_summon_tm;
    uint32_t status;
    char mon_nick[NICK_LEN];
}__attribute__((packed));

struct gf_hatch_summon_out{
	uint32_t pre_summon_tm;
    uint32_t mon_tm;
    uint32_t mon_type;
    uint32_t lv;
    uint32_t fight_value;
    char mon_nick[NICK_LEN];
    uint32_t status;
    uint32_t item_id;
}__attribute__((packed));

struct gf_get_last_summon_nick_out {
	char nick[NICK_LEN];
}__attribute__((packed));

struct gf_feed_summon_in{
    uint32_t mon_tm;
    uint32_t add_val;
    uint32_t max_val;
    uint32_t item_id;
}__attribute__((packed));

struct gf_feed_summon_out{
    uint32_t mon_tm;
	uint32_t total_val;
    uint32_t item_id;
}__attribute__((packed));

struct gf_set_summon_nick_in{
    uint32_t mon_tm;
    char  nick[NICK_LEN];
}__attribute__((packed));

struct gf_set_summon_nick_out{
    uint32_t mon_tm;
}__attribute__((packed));

struct gf_change_summon_fight_in{
    uint32_t mon_tm;
	uint32_t flag;
}__attribute__((packed));

struct gf_change_summon_fight_out{
    uint32_t mon_tm;
	uint32_t flag;
}__attribute__((packed));

struct gf_set_summon_property_in{
    uint32_t mon_tm;
    uint32_t lv;
    uint32_t exp;
    uint32_t fight_val;
}__attribute__((packed));

struct gf_set_summon_skills_in{
    uint32_t mon_tm;
    uint32_t src_id;
    //uint32_t new_id;
    //uint8_t     skills[50];
    sum_skill_t skill_info;
}__attribute__((packed));

typedef gf_set_summon_skills_in gf_set_summon_skills_out;

struct gf_use_summon_skills_scroll_in{
    uint32_t mon_tm;
    uint32_t id;
    //uint32_t new_skill;
    uint32_t flag;
    //uint8_t  skills[50];
    sum_skill_t skill_info;
}__attribute__((packed));

typedef gf_use_summon_skills_scroll_in gf_use_summon_skills_scroll_out;

struct gf_set_summon_mutate_in{
	uint32_t evolve_need_item;
    uint32_t mon_tm;
    uint32_t mon_type;
    uint32_t lv;
}__attribute__((packed));

struct gf_set_summon_mutate_out{
    uint32_t new_type;
}__attribute__((packed));

typedef stru_count gf_set_summon_type_in_header;
struct gf_set_summon_type_in_item{
    uint32_t    mon_tm;
    uint32_t    mon_type;
}__attribute__((packed));
struct gf_allocate_exp2summon_in{
    uint32_t    alloter_exp;
    uint32_t    mon_tm;
    uint32_t    mon_exp;
    uint32_t    lv;
}__attribute__((packed));
//end summon
// ----- Begin Numen
struct gf_get_numen_list_out_header {
    uint32_t    numen_cnt;
}__attribute__((packed));

struct numen_skill_t {
    uint32_t    id;
    uint32_t    lv;
}__attribute__((packed));

struct gf_get_numen_list_out_item {
    uint32_t    numen_type;
    char        name[NICK_LEN];
    uint32_t    status;
    uint32_t    skill_cnt;
    numen_skill_t   skills[];
}__attribute__((packed));

struct gf_invite_numen_in_header {
    uint32_t    call_item_id;
    uint32_t    numen_type;
    char        name[NICK_LEN];
    uint32_t    skill_cnt;

}__attribute__((packed));

struct gf_invite_numen_in_item {
    uint32_t    skill_id;
    uint32_t    skill_lv;
}__attribute__((packed));

struct gf_invite_numen_out {
    uint32_t    call_item_id;
    uint32_t    numen_type;
}__attribute__((packed));

struct gf_change_numen_status_in {
    uint32_t    numen_type;
    uint32_t    status;
}__attribute__((packed));

struct gf_make_numen_sex_in {
    uint32_t    sex_item_id;
    uint32_t    numen_type;
    uint32_t    action;//1--nothing; 2--level up; 3--add; 4--choose; 5--replace
    uint32_t    old_id;
    uint32_t    old_lv;
    uint32_t    new_id;
    uint32_t    new_lv;
}__attribute__((packed));

typedef gf_make_numen_sex_in gf_make_numen_sex_out; 

struct gf_change_numen_nick_in {
    uint32_t    numen_type;
    char        name[NICK_LEN];
}__attribute__((packed));

typedef gf_change_numen_nick_in gf_change_numen_nick_out;
// ----- End Numen

struct gf_logout_in{
    uint32_t    mapid;
    uint32_t    xpos;
    uint32_t    ypos;
	uint32_t	timediff;
}__attribute__((packed));

struct gf_add_killed_boss_in{
	uint32_t	boss_id;
}__attribute__((packed));

struct gf_get_killed_boss_out_header{
	uint32_t	cnt;
}__attribute__((packed));

struct gf_get_killed_boss_out_item{
	uint32_t	stage_id;
	uint8_t		grade[6];
}__attribute__((packed));

struct gf_get_killed_boss_kf_out_item{
	uint32_t	stage_id;
	uint8_t		grade[4];
}__attribute__((packed));


struct gf_get_db_buff_list_out_item{
	uint32_t	buff_type;
	uint32_t	duration;
	uint32_t 	mutex_type;
	uint32_t 	start_tm;
}__attribute__((packed));

struct gf_set_daily_action_in{
    uint32_t real_type;
    uint32_t flag;
	uint32_t toplimit;
}__attribute__((packed));

struct gf_set_daily_action_out{
    uint32_t type;
}__attribute__((packed));
struct gf_get_daily_action_out_header{
	uint32_t 		fumo_old_place;
    uint32_t        count;
}__attribute__((packed));

struct gf_get_daily_action_out_item{
    uint32_t        type;
    uint32_t        count;
}__attribute__((packed));


#define GF_MAX_INVI_CODE_LEN 16
struct gf_check_invite_code_in{
	char	  invite_code[GF_MAX_INVI_CODE_LEN];
}__attribute__((packed));

struct gf_use_invite_code_in{
	char	  invite_code[GF_MAX_INVI_CODE_LEN];
}__attribute__((packed));

struct gf_get_invit_code_in{
	uint32_t  num;
}__attribute__((packed));

struct gf_get_invit_code_out_header{
	uint32_t cnt;
}__attribute__((packed));

struct gf_get_invit_code_out_item{
	uint8_t code[16];
}__attribute__((packed));

typedef stru_count gf_get_warehouse_item_list_out_header; 

struct gf_get_warehouse_item_list_out_item
{
	uint32_t        itemid;
	uint32_t        count;
}__attribute__((packed));

typedef stru_count gf_get_warehouse_clothes_item_list_out_header;

struct gf_get_warehouse_clothes_item_list_out_item
{
	uint32_t	id;
	uint32_t	attireid;
	uint32_t	get_time;
	uint32_t	attire_rank;
	uint32_t	duration;
	uint32_t    end_time;
	uint32_t    attire_lv;
}__attribute__((packed));

struct gf_set_account_forbid_in{
    uint32_t    forbid_flag;
    uint32_t    deadline;
}__attribute__((packed));
//vip start
struct gf_set_vip_in{
    uint32_t    vip_is;// VIP标志，0：销户，1：首次开通VIP，2：销户后重新开通，3：续时, 4:取消自动续费,此时is_auto_charge应为0 
    uint32_t    vip_month_cnt;
    uint32_t    end_tm;
    uint32_t    auto_incr;
    uint32_t    start_tm;
    uint32_t    method;
    uint8_t     vip_type; // 0:非vip；bit1:普通vip；bit2:年费vip
}__attribute__((packed));

struct gf_get_user_vip_out{
    uint32_t    vip_is;
    uint32_t    x_value;
    uint32_t    vip_lv;
    uint32_t    vip_month_cnt;
    uint32_t    end_tm;
    uint32_t    auto_incr;
    uint32_t    start_tm;
    uint32_t    method;
}__attribute__((packed));


struct gf_set_base_svalue_in{
    uint32_t    x_val;
    uint16_t    chn;
}__attribute__((packed));

struct gf_set_gold_in{
    uint32_t    gold;
}__attribute__((packed));

typedef gf_set_vip_in other_gf_sync_vip_in;
typedef gf_set_base_svalue_in other_gf_sync_base_svalue_in;
typedef gf_set_gold_in other_gf_sync_gold_in;

struct other_gf_vip_sys_msg_in_header{
    uint32_t msglen;
    char    msg[];
}__attribute__((packed));

struct gf_set_amb_info_in {
    uint32_t parentuid;
}__attribute__((packed));

struct gf_get_amb_info_out {
	uint32_t parentuid;
}__attribute__((packed));

struct gf_set_vip_exinfo_in{
    uint32_t exbag;
    uint32_t exwarehouse;
}__attribute__((packed));
struct gf_user_self_define{
    uint32_t    forbid_flag;
    uint32_t    forbid_time;
    uint32_t    deadline;
    uint32_t    vip_is;
    uint32_t    x_value;
    uint32_t    start_time;
    uint32_t    end_time;
}__attribute__((packed));
//vip end

struct gf_set_cryptogram_gift_in{
    uint32_t    max_bag;
    uint32_t    itemid;
    uint32_t    itemtype;
    uint32_t    item_cnt;
}__attribute__((packed));

struct gf_set_cryptogram_gift_out{
    uint32_t    itemid;
    uint32_t    uniqueid;
    uint32_t    itemtype;
    uint32_t    item_cnt;
}__attribute__((packed));


struct gf_set_magic_gift_in_header{
    uint32_t    max_bag;
    uint32_t    count;
}__attribute__((packed));
struct gf_set_magic_gift_in_item{
    uint32_t    itemid;
    uint32_t    itemtype;
    uint32_t    item_cnt;
}__attribute__((packed));

typedef stru_count gf_set_magic_gift_out_header;
struct gf_set_magic_gift_out_item{
    uint32_t    itemid;
    uint32_t    uniqueid;
    //uint32_t    duration;
    uint32_t    item_cnt;
}__attribute__((packed));

struct gf_set_magic_box_in_header{
    uint32_t    max_bag_num;
    uint32_t    box_type;
    uint32_t    box_id;
    uint32_t    box_cnt;
}__attribute__((packed));

struct gf_set_magic_box_in_item{
    uint32_t    box_quality;
    uint32_t    save_flag;
    uint32_t    item_type;
    uint32_t    item_id;
    uint32_t    item_cnt;
    uint32_t    duration;
    uint32_t    uniqueid;
}__attribute__((packed));
typedef gf_set_magic_box_in_header gf_set_magic_box_out_header;
typedef gf_set_magic_box_in_item gf_set_magic_box_out_item;
//begin store
struct gf_prepare_add_product_in_header{
    uint32_t max_bag;
    uint32_t max_limit;
    uint32_t attire_cnt;
    uint32_t item_cnt;
}__attribute__((packed));
struct gf_prepare_add_product_in_item1{
    uint32_t attireid;
    uint32_t duration;
    int32_t timelag;
}__attribute__((packed));
struct gf_prepare_add_product_in_item2{
    uint32_t itemid;
    uint32_t cnt;
}__attribute__((packed));

struct gf_add_store_product_in_header{
    uint32_t max_bag;
    uint32_t attire_cnt;
    uint32_t item_cnt;
}__attribute__((packed));
typedef gf_prepare_add_product_in_item1 gf_add_store_product_in_item1;
typedef gf_prepare_add_product_in_item2 gf_add_store_product_in_item2;

struct gf_add_store_product_out_header{
    uint32_t attire_cnt;
    uint32_t item_cnt;
}__attribute__((packed));
struct gf_add_store_product_out_item1{
    uint32_t attireid;
    uint32_t uniquekey;
    uint32_t gettime;
    uint32_t timelag;
}__attribute__((packed));
struct gf_add_store_product_out_item2{
    uint32_t itemid;
    uint32_t cnt;
}__attribute__((packed));

struct gf_prepare_repair_product_in_header{
    uint32_t max_bag;
    uint32_t product_cnt;
}__attribute__((packed));
struct gf_prepare_repair_product_in_item{
    uint32_t productid;
    uint32_t attireid;
    uint32_t uniquekey;
    int32_t timelag;
}__attribute__((packed));
typedef gf_prepare_repair_product_in_header gf_repair_store_product_in_header;
typedef gf_prepare_repair_product_in_item gf_repair_store_product_in_item;

typedef stru_count gf_repair_store_product_out_header;
typedef gf_add_store_product_out_item1 gf_repair_store_product_out_item;

typedef stru_count gf_get_outdated_product_out_header;
struct gf_get_outdated_product_out_item{
    uint32_t    attireid;
    uint32_t    uniquekey;
    uint32_t    duration;
    uint32_t    attirelv;
}__attribute__((packed));

typedef stru_count gf_synchro_user_gfcoin_in;
typedef stru_count gf_query_user_gfcoin_out;

struct gf_coupons_exchange_in_header {
    uint32_t    trade_id;
    uint32_t    del_item_id;
    uint32_t    del_item_cnt;
    uint32_t    max_bag;
    uint32_t    add_cnt;
}__attribute__((packed));
struct gf_coupons_exchange_in_item {
    uint32_t    type;
    uint32_t    item_id;
    uint32_t    item_cnt;
    uint32_t    duration;
    uint32_t    lifetime;
}__attribute__((packed));

struct gf_coupons_exchange_out_header{
    uint32_t    trade_id;
    uint32_t    left_coupons;
    uint32_t    item_cnt;
}__attribute__((packed));
struct gf_coupons_exchange_out_item{
    uint32_t    type;
    uint32_t    id;
    uint32_t    cnt;
    uint32_t    uniqueid;
    uint32_t    gettime;
    uint32_t    endtime;
}__attribute__((packed));
//end store
struct gf_set_child_count_in{
    uint32_t childid;
	uint32_t chlid_role_tm;
}__attribute__((packed));

struct gf_set_child_count_out{
    uint32_t lv;
}__attribute__((packed));

struct gf_set_amb_status_in{
    uint8_t status;
}__attribute__((packed));

struct gf_set_amb_status_out{
    uint8_t status;
}__attribute__((packed));

struct gf_set_amb_reward_flag_in{
    uint8_t buf[40];
}__attribute__((packed));


struct gf_learn_secondary_pro_in
{
	uint32_t pro_type;
	uint32_t money;
}__attribute__((packed));

struct gf_learn_secondary_pro_out
{
	uint32_t pro_type;
	uint32_t left_coin;
}__attribute__((packed));

struct gf_material_data
{
	uint32_t item_id;
	uint32_t item_count;
}__attribute__((packed));

struct gf_random_data
{
	uint32_t item_id;
	uint32_t item_count;
}__attribute__((packed));

struct gf_secondary_pro_fuse_in_header
{
	uint32_t max_item_bag_count;
	uint32_t max_exp_limit;
	uint32_t pro_type;
	uint32_t add_exp;
	uint32_t id;
	//uint32_t create_item_id;
	//uint32_t create_item_count;
	uint32_t material_count;
	uint32_t random_count;
	gf_material_data  materials[];
}__attribute__((packed));

struct gf_secondary_pro_fuse_out
{
	uint32_t pro_type;
	uint32_t add_exp;
	uint32_t id;
	uint32_t create_item_id;
	uint32_t create_item_count;
	uint32_t material_count;
	gf_material_data  materials[];
}__attribute__((packed));

struct gf_secondary_pro_fuse_out_header
{
	uint32_t pro_type;
	uint32_t add_exp;
	uint32_t id;
	uint32_t material_count;
	uint32_t random_count;
}__attribute__((packed));

struct gf_secondary_pro_fuse_out_material
{
	uint32_t item_id;
	uint32_t item_count;
}__attribute__((packed));


typedef stru_count gf_get_secondary_pro_list_out_header;

struct gf_get_secondary_pro_list_out_type
{
	uint32_t pro;
	uint32_t exp;
}__attribute__((packed));


typedef stru_count gf_buy_item_limit_list_out_header;

struct gf_buy_item_limit_list_out_element
{
	uint32_t item_id;
    uint32_t limit_type;
    uint32_t last_time;
	uint32_t item_count;
}__attribute__((packed));


typedef stru_count gf_mail_head_list_out_header;


struct gf_mail_head_list_out_element
{
	uint32_t        mail_id;
	uint32_t       	mail_time;
   	uint32_t        mail_state;
	uint32_t        mail_templet;
	uint32_t        mail_type;	
	uint32_t        sender_id;
	uint32_t        sender_role_tm;
	char            mail_title[ MAX_MAIL_TITLE_LEN + 1];	
};//为了和ONLINE匹配这里就不吧结构给压实了

struct gf_mail_body_in
{
	uint32_t mail_id;
}__attribute__((packed));

struct gf_mail_body_out
{
	uint32_t mail_id;
	char     mail_content[MAX_MAIL_BODY_LEN +1 ];
	char     mail_numerical_enclosure[1024];
	char     mail_item_enclosure[1024];
	char     mail_equip_enclosure[1024];
};//为了和ONLINE匹配这里就不吧结构给压实了


struct gf_delete_mail_in
{
	uint32_t mail_id;
}__attribute__((packed));	

struct gf_delete_mail_out
{
	uint32_t mail_id;
}__attribute__((packed));

struct gf_mail_nums_enclosure
{
	uint32_t type;
	uint32_t number;
}__attribute__((packed));

struct gf_mail_items_enclosure
{
	uint32_t item_id;
	uint32_t item_count;
	uint32_t max_item_count;
}__attribute__((packed));

struct gf_mail_equips_enclosure
{
	uint32_t equip_id;
	uint32_t equip_get_time;
	uint32_t equip_rank;
	uint32_t equip_duration;
	uint32_t equip_lifetime;
}__attribute__((packed));

struct gf_take_mail_enclosure_in_header
{
	uint32_t mail_id;
	uint32_t max_item_bag_grid;
	uint32_t mail_nums_count;
	uint32_t mail_items_count;
	uint32_t mail_equips_count;
}__attribute__((packed));

struct gf_send_mail_in
{
	uint32_t sender_id;
	uint32_t receive_id;
	uint32_t mail_templet;
	uint32_t max_mail_limit;
	char     mail_title[MAX_MAIL_TITLE_LEN+1];
	char     mail_body[MAX_MAIL_BODY_LEN + 1];	
}__attribute__((packed));

struct gf_send_mail_out
{
	uint32_t        mail_id;
	uint32_t        mail_time;
	uint32_t        mail_state;
	uint32_t        mail_templet;
	uint32_t        mail_type;
	uint32_t        sender_id;
	uint32_t        receive_id;
	char            mail_title[ MAX_MAIL_TITLE_LEN + 1];
}__attribute__((packed));


struct gf_reduce_money_in
{
	uint32_t money;
}__attribute__((packed));

struct gf_send_system_mail_in
{
	uint32_t sender_id;
	uint32_t receive_id;
	uint32_t mail_templet;
	uint32_t max_mail_limit;
	char     mail_title[MAX_MAIL_TITLE_LEN+1];
	char     mail_body[MAX_MAIL_BODY_LEN + 1];
	char     mail_num_enclosure[1024];
	char     mail_item_enclosure[1024];
	char     mail_equip_enclosure[1024];
}__attribute__((packed));

struct  gf_get_hero_top_info_in
{
	uint32_t user_exp;
}__attribute__((packed));

struct gf_get_hero_top_info_out
{
	uint32_t sort;

}__attribute__((packed));

struct gf_set_hero_top_info_in
{
	uint32_t user_lv;
	uint32_t user_exp;
}__attribute__((packed));

struct gf_get_hunter_info_in {
	uint32_t role_type;
}__attribute__((packed));

struct gf_get_hunter_info_out
{
    uint32_t    layer;
    uint32_t    interval;
    uint32_t    order;
}__attribute__((packed));

typedef  gf_get_hunter_info_in gf_get_hunter_top_info_in;

struct gf_get_hunter_top_info_out_header
{
    uint32_t    layer;
    uint32_t    interval;
    uint32_t    order;
    uint32_t    cnt;
}__attribute__((packed));


struct gf_get_hunter_top_info_out_item
{
    uint32_t    userid;
    uint32_t    roletm;
    uint32_t    role_type;
    char        nick[NICK_LEN];
    uint32_t    layer;
    uint32_t    interval;
    uint32_t    userlv;
}__attribute__((packed));

struct  gf_get_hunter_top_info_for_web_in {
	uint32_t role_type;
	uint32_t top_num;
}__attribute__((packed));

struct gf_get_hunter_top_info_for_web_out_header
{
    uint32_t    cnt;
}__attribute__((packed));

struct gf_get_hunter_top_info_for_web_out_item
{
    uint32_t    userid;
    uint32_t    roletm;
    uint32_t    role_type;
    char        nick[NICK_LEN];
    uint32_t    layer;
    uint32_t    interval;
    uint32_t    userlv;
}__attribute__((packed));

struct gf_get_clothes_info_for_web_out_header
{
    uint32_t    cnt;
}__attribute__((packed));

struct gf_get_clothes_info_for_web_out_item
{
    uint32_t    attire_id;
	uint32_t 	attirelv;
	uint32_t	duration;
	uint32_t	equip_part;
}__attribute__((packed));


struct gf_get_hunter_old_top100_out_item
{
    uint32_t    userid;
    uint32_t    roletm;
	uint32_t    reward_flg;
}__attribute__((packed));


struct gf_set_hunter_top_info_in
{
    uint32_t    userid;
    uint32_t    roletm;
    uint32_t    role_type;
    char        nick[NICK_LEN];
    uint32_t    layer;
    uint32_t    interval;
    uint32_t    userlv;
}__attribute__((packed));

struct gf_add_shop_log_in
{
	uint32_t shop_start_tm;
	uint32_t type;
	uint32_t itemid;
	uint32_t uniquekey;
	uint32_t cnt;
	uint32_t price;
	uint32_t sell_tm;
}__attribute__((packed));

struct gf_get_shop_last_log_out_header
{
	uint32_t count;
}__attribute__((packed));

struct gf_get_shop_last_log_out_item
{
	uint32_t type;
	uint32_t itemid;
	uint32_t uniquekey;
	uint32_t attire_lv;
	uint32_t cnt;
	uint32_t price;
	uint32_t sell_tm;
}__attribute__((packed));

struct gf_add_donate_count_in
{
	uint32_t value;
}__attribute__((packed));

struct gf_set_buy_item_limit_data_in
{
	uint32_t item_id;
	uint32_t item_limit_type;
	uint32_t last_time;
	uint32_t item_count;
}__attribute__((packed));


struct gf_get_role_base_info_for_boss_out_header {
	uint32_t role_type;
	uint32_t level;
	uint32_t honor;
	char	nick[NICK_LEN];
	uint32_t clothes_num;
}__attribute__((packed));

struct gf_get_role_base_info_for_boss_out_item {
	uint32_t clothes_id;
	uint32_t level;
}__attribute__((packed));


struct gf_get_player_community_info_out {
	uint32_t team_id;
	uint32_t friend_cnt;
	uint32_t summon_cnt;
	uint32_t card_cnt;
	uint32_t achieve_cnt;
	uint32_t fumo_points;
}__attribute__((packed));


struct gf_add_item_attire_in {
	uint32_t type;
	uint32_t item_id;
	uint32_t extra_info;//装备为唯一ID, 普通物品为数量信息
	uint32_t max_item_bag_grid_count;
}__attribute__((packed));

struct gf_add_item_attire_out {
	uint32_t error_ret;
	uint32_t type;
	uint32_t item_id;
	uint32_t extra_info;
}__attribute__((packed));



struct gf_get_kill_boss_list_out_element{
	uint32_t stage_id;
	uint32_t boss_id;
	uint32_t kill_time;
	uint32_t pass_cnt;
}__attribute__((packed));


typedef stru_count gf_get_kill_boss_list_out_header;

struct gf_replace_kill_boss_in{
	uint32_t stage_id;
	uint32_t boss_id;
	uint32_t kill_time;
	uint32_t pass_cnt;
}__attribute__((packed));

struct gf_get_ap_toplist_in
{
	uint32_t begin_index;
	uint32_t end_index;
}__attribute__((packed));

struct gf_get_ap_toplist_out_element{
	uint32_t userid;
	uint32_t role_regtime;
	char     nick[NICK_LEN];
	uint32_t ap_point;
	uint32_t last_update_tm;	
}__attribute__((packed));

struct gf_get_ap_toplist_out_header
{
	uint32_t begin_index;
	uint32_t end_index;
	uint32_t count;
}__attribute__((packed));

struct gf_get_achievement_data_list_out_element{
	uint32_t achieve_type;
	uint32_t get_time;
}__attribute__((packed));

typedef stru_count gf_get_achievement_data_list_out_header;

struct gf_replace_achievement_data_in{
	uint32_t achieve_type;
	uint32_t get_time;
	uint32_t add_point;
}__attribute__((packed));


typedef stru_count gf_get_achieve_title_out_header;
struct gf_get_achieve_title_out_item {
    uint8_t  type;
    uint32_t subid;
    uint32_t get_time;
}__attribute__((packed));

struct gf_add_achieve_title_in_header{
    uint8_t  type;
    uint32_t  get_time;
    uint32_t cnt;
}__attribute__((packed));
struct gf_add_achieve_title_in_item {
    uint32_t titleid;
}__attribute__((packed));

struct gf_add_achieve_title_out {
    uint8_t type;
    uint32_t gettime;
}__attribute__((packed));

typedef stru_count gf_set_cur_achieve_title_in;
typedef stru_count gf_set_cur_achieve_title_out;


typedef stru_count gf_get_ring_task_list_out_header;
struct gf_get_ring_task_list_out_element
{
	uint32_t task_id;
	uint32_t task_type;
	uint32_t master_task_id;
	uint32_t task_group_id;
}__attribute__((packed));


typedef stru_count gf_get_ring_task_history_list_out_header;
struct gf_get_ring_task_history_list_out_element
{
	uint32_t task_id;
	uint32_t day_count;
	uint32_t last_tm;
}__attribute__((packed));

struct gf_replace_ring_task_in
{
	uint32_t task_id;
	uint32_t task_type;
	uint32_t master_task_id;
	uint32_t task_group_id;
}__attribute__((packed));

struct gf_delete_ring_task_in
{
	uint32_t task_id;
	uint32_t task_type;
}__attribute__((packed));


struct gf_replace_ring_task_history_in
{
	uint32_t task_id;
	uint32_t day_count;
	uint32_t last_tm;
}__attribute__((packed));


struct gf_join_contest_team_in {
	uint32_t team_id;
	uint8_t  nick_name[16];
} __attribute__((packed));

struct gf_join_contest_team_out {
	uint32_t team_id;
} __attribute__((packed));

struct gf_contest_donate_plant_in {
	uint32_t plant_cnt;
} __attribute__((packed));

typedef gf_contest_donate_plant_in gf_contest_donate_plant_out;

struct contest_team_info_t {
	uint32_t team_id;
	uint32_t leader_id;
	uint32_t leader_tm;
	uint8_t  leader_name[16];
	uint32_t member_cnt;
	uint32_t donate_cnt;
}__attribute__((packed));

struct gf_get_contest_team_info_header {
	uint32_t team_cnt;
}__attribute__((packed));


struct gf_donate_item_in_header {
	uint32_t type_cnt;
}__attribute__((packed));

struct gf_donate_item_in_item {
	uint32_t type;
	uint32_t cnt;
}__attribute__((packed));



struct gf_get_contest_donate_header {
	uint32_t self_team;
	uint32_t self_cnt;
	uint32_t top_cnt;
}__attribute__((packed));

struct donate_t {
	uint32_t uid;
	uint32_t role_tm;
	uint8_t nick_name[16];
	uint32_t donate_cnt;
}__attribute__((packed));

struct gf_get_self_contest_out {
	uint32_t team_id;
	uint32_t plant_cnt;
    uint32_t win_stage;
}__attribute__((packed));

struct gf_get_pvp_game_rank_in {
    uint32_t    rank; //1--初级榜单 //2--高级榜单
}__attribute__((packed));

struct gf_get_pvp_game_rank_out_header {
    uint32_t rank;
    uint32_t count;
}__attribute__((packed));

struct gf_get_pvp_game_rank_out_item {
    uint32_t    userid;
    uint32_t    roletm;
    uint8_t    nick[16];
    uint32_t    lv;
    uint32_t    score;
}__attribute__((packed));

struct gf_get_pvp_game_data_in {
    uint32_t    pvp_type;
}__attribute__((packed));

struct gf_get_pvp_game_data_out_header {
    uint32_t count;
}__attribute__((packed));

struct gf_get_pvp_game_data_out_item {
    uint32_t    pvp_type;
    uint32_t    do_times;
    uint32_t    win_times;
    uint32_t    flower;
    uint32_t    rank;
    uint32_t    segment;
}__attribute__((packed));

struct gf_set_pvp_game_data_in {
    uint32_t    userid;
    uint32_t    roletm;
    uint8_t    nick[16];
    uint32_t    lv;
    uint32_t    pvp_type;
    uint32_t    done;
    uint32_t    win;
	uint32_t    flower_cnt;
}__attribute__((packed));

struct gf_set_pvp_game_flower_in {
    uint32_t    userid;
    uint32_t    roletm;
    uint32_t    pvp_type;
    uint32_t    flower;
}__attribute__((packed));

struct gf_upgrade_item_in_header {
	uint32_t	upgrade_succ;
	uint32_t 	del_cnt;
	uint32_t	add_cnt;
};

struct gf_upgrade_item_in_item {
	uint32_t	type;
	uint32_t 	id;
	uint32_t	count;
	uint32_t	duration;
};

struct gf_swap_action_func_in_header {
	uint32_t id;
	uint32_t type;
	uint32_t add_times;
	uint32_t user_flg;
	uint32_t del_cnt;
	uint32_t add_cnt;
}__attribute__((packed));

struct gf_swap_action_func_in_item {
	uint32_t type;
	uint32_t id;
	uint32_t count;
	uint32_t uniqe_id;
	uint32_t duration;
}__attribute__((packed));

struct gf_swap_action_func_out_header {
	uint32_t id;
	uint32_t type;
	uint32_t add_times;
	uint32_t user_flg;
	uint32_t del_cnt;
	uint32_t add_cnt;
}__attribute__((packed));

struct gf_swap_action_func_out_item {
	uint32_t type;
	uint32_t id;
	uint32_t count;
	uint32_t uniqe_id;
	uint32_t duration;
}__attribute__((packed));

struct gf_get_swap_action_out_header {
	uint32_t cnt;
}__attribute__((packed));

struct gf_get_swap_action_out_item {
	uint32_t id;
	uint32_t type;
	uint32_t cnt;
	uint32_t tm;
}__attribute__((packed));
//added by cws 0608
struct gf_add_swap_in{
	uint32_t id;
	uint32_t type;
	uint32_t tm;
	uint32_t cnt;
}__attribute__((packed));
////
struct gf_clear_swap_action_in {
	uint32_t id;
}__attribute__((packed));

struct gf_set_client_buf_in {
	uint8_t buf[max_client_buf_len];
};

struct gf_get_client_buf_out {
	uint8_t buf[max_client_buf_len];
};

struct gf_team_member_reward_in_header {
    uint32_t win_stage;
    uint32_t count;
}__attribute__((packed));

struct gf_team_member_reward_in_item {
	uint32_t type;
	uint32_t id;
	uint32_t cnt;
}__attribute__((packed));

struct gf_team_member_reward_out_header {
    uint32_t win_stage;
    uint32_t count;
}__attribute__((packed));

struct gf_team_member_reward_out_item {
	uint32_t id;
	uint32_t cnt;
}__attribute__((packed));

struct gf_get_plant_list_out_header {
    uint32_t count;
}__attribute__((packed));

struct gf_get_plant_list_out_item {
	uint32_t plant_id; 
	uint32_t plant_tm; 
	uint32_t field_id; 
	uint32_t status;
	uint32_t status_tm;
	//uint32_t water_cnt;
	uint32_t water_tm;
	uint32_t fruit_cnt;
	uint32_t speed_per;
	uint32_t output_per;
}__attribute__((packed));


struct gf_get_summon_dragon_list_out_header {
    uint32_t count;
}__attribute__((packed));

struct gf_get_summon_dragon_list_out_item {
	uint32_t uid; 
	char nick[NICK_LEN]; 
	uint32_t nimbus; 
	uint32_t tm;
}__attribute__((packed));


struct gf_add_summon_dragon_in {
	uint32_t uid; 
	char nick[NICK_LEN]; 
	uint32_t nimbus; 
	uint32_t tm;
}__attribute__((packed));


struct gf_set_plant_status_in {
	uint32_t plant_id;
	uint32_t plant_tm;
	uint32_t field_id;
	uint32_t status;
	uint32_t status_tm;
	uint32_t water_tm;
	uint32_t fruit_cnt;
	uint32_t speed_per;
	uint32_t output_per;
}__attribute__((packed));

struct gf_set_plant_status_out {
	uint32_t plant_id;
	uint32_t plant_tm;
	uint32_t field_id;
	uint32_t status;
	uint32_t status_tm;
	uint32_t water_tm;
	uint32_t fruit_cnt;
	uint32_t speed_per;
	uint32_t output_per;
}__attribute__((packed));


struct gf_add_effect_to_all_plants_in {
	uint32_t speed_per;
	uint32_t output_per;
}__attribute__((packed));


struct gf_add_home_log_in {
	uint32_t op_uid;
	uint32_t op_utm;
	uint32_t role_type;
	char nick[16];
	uint32_t type;
	uint32_t access_type;
	uint32_t tm;
}__attribute__((packed));

struct gf_get_home_log_out_header {
	uint32_t count;
}__attribute__((packed));

struct gf_get_home_log_out_item {
	uint32_t op_uid;
	uint32_t op_utm;
	uint32_t role_type;
	char nick[16];
	uint32_t type;
	uint32_t access_type;
	uint32_t tm;
}__attribute__((packed));


struct gf_pick_fruit_in_header {
	uint32_t tm;
	uint32_t fruit_id;
	uint32_t cost_cnt;
}__attribute__((packed));

struct gf_pick_fruit_in_item{
	uint32_t type;
	uint32_t id;
	uint32_t count;
}__attribute__((packed));

struct gf_pick_fruit_out_header {
	uint32_t tm;
	uint32_t fruit_id;
	uint32_t cost_cnt;
}__attribute__((packed));

struct gf_pick_fruit_out_item {
	uint32_t give_type;
	uint32_t give_id;
	uint32_t count;
}__attribute__((packed));

struct gf_get_home_data_out{
	uint32_t home_level;
	uint32_t home_exp;
	uint32_t last_randseed_tm;
}__attribute__((packed));

struct gf_set_home_data_in {
	uint32_t home_level;
	uint32_t home_exp;
	uint32_t dec_fumo_point;
}__attribute__((packed));

struct gf_set_home_data_out {
	uint32_t home_level;
	uint32_t home_exp;
	uint32_t left_fumo;
}__attribute__((packed));

struct gf_set_home_update_tm_in {
	uint32_t last_randseed_tm;
}__attribute__((packed));


//
//struct gf_replace_home_exp_level_in{
//	uint32_t exp;
//	uint32_t level;
//	uint32_t fumo_point;
//}__attribute__((packed));
//
//struct gf_replace_home_exp_level_out{
//	uint32_t level;
//	uint32_t exp;
//	uint32_t fumo_point;
//}__attribute__((packed));
//
//struct gf_update_home_active_point_in{
//	uint32_t active_point;
//	uint32_t last_tm;
//}__attribute__((packed));

struct gf_set_role_vitality_point_in{
	uint32_t point;	
}__attribute__((packed));

struct gf_set_second_pro_exp_in{
	uint32_t pro_type;
	uint32_t pro_exp;
}__attribute__((packed));


struct gf_insert_card_in {
	uint32_t item_id;
	uint32_t card_type;
	uint32_t card_set;
}__attribute__((packed));

struct gf_insert_card_out {
	uint32_t item_id;
	uint32_t card_id;
	uint32_t card_type;
	uint32_t card_set;
}__attribute__((packed));

typedef gf_insert_card_out gf_erase_card_in;

typedef gf_insert_card_out gf_erase_card_out;

struct gf_get_card_list_out_header {
    uint32_t count;
}__attribute__((packed));

struct gf_get_card_list_out_item {
	uint32_t card_id;
	uint32_t card_type;
	uint32_t card_set;
}__attribute__((packed));

struct gf_fresh_summon_attr_in {
	uint32_t item_id;
	uint32_t mon_type;
	uint32_t mon_tm;
	int attr_per;
    uint32_t vip_sum;
    uint32_t mon_lv;
    uint32_t mon_exp;
}__attribute__((packed));

typedef gf_fresh_summon_attr_in gf_fresh_summon_attr_out;

struct gf_get_offline_info_out {
	uint32_t user_off_tm;
	uint32_t summon_off_tm;
	uint32_t off_coins;
	uint32_t off_fumo;
}__attribute__((packed));

struct gf_set_offline_info_in {
	uint32_t player_lv;
	uint32_t user_off_tm;
	uint32_t summon_off_tm;
	uint32_t off_coins;
	uint32_t off_fumo;
	uint32_t player_add_exp;
	uint32_t mon_tm;
	uint32_t mon_add_exp;
	uint32_t mon_add_lv;
	uint32_t reduce_coins;
	uint32_t reduce_fumo;
	uint32_t summon_end_flag;
}__attribute__((packed));

typedef gf_set_offline_info_in gf_set_offline_info_out;

struct gf_create_fight_team_in {
	char team_name[16];
	char captain_name[16];
}__attribute__((packed));

struct gf_create_fight_team_out {
	uint32_t team_id;
}__attribute__((packed));

struct gf_add_fight_team_member_in {
	uint32_t team_id;
	uint32_t inviter;
}__attribute__((packed));

struct gf_add_fight_team_member_out_header {
	uint32_t team_id;
	uint32_t inviter;
	uint32_t member_cnt;
}__attribute__((packed));

struct gf_add_fight_team_member_out_item {
	uint32_t uid;
	uint32_t role_tm;
	uint32_t join_tm;
	uint32_t level;
    uint32_t team_coins;
    uint32_t team_exp;
}__attribute__((packed));


struct gf_set_player_team_flg_in {
	uint32_t team_id;
}__attribute__((packed));

struct gf_set_player_team_flg_out {
	uint32_t team_id;
}__attribute__((packed));

struct gf_get_team_info_in {
	uint32_t team_id;
}__attribute__((packed));

struct gf_get_team_info_out_header {
	uint32_t team_id;
	uint32_t captain_uid;
	uint32_t captain_tm;
	char     captain_nick[16];
	uint32_t team_coin;
    uint32_t active_flag;
    uint32_t active_score;
	uint32_t score_index;
	char	team_name[16];
	uint32_t score;
	uint32_t team_exp;
	uint32_t last_tax_time;
	char    team_mcast[240];
	uint32_t member_cnt;

}__attribute__((packed));

struct gf_get_team_info_out_item {
	uint32_t id;
	uint32_t tm;
	uint32_t join_tm;
	uint32_t level;
	uint32_t team_coins;
	uint32_t team_exp;
}__attribute__((packed));


struct gf_del_team_member_in {
	uint32_t team_id;
	uint32_t del_uid;
	uint32_t del_role_tm;
}__attribute__((packed));

struct gf_del_team_member_out {
	uint32_t team_id;
	uint32_t del_uid;
	uint32_t del_role_tm;
}__attribute__((packed));


struct gf_del_team_in {
	uint32_t team_id;
}__attribute__((packed));

struct gf_del_team_out {
	//uint32_t team_id;
}__attribute__((packed));

struct gf_team_info_changed_in {
	uint32_t team_id;
	uint32_t type;
	uint32_t score;
}__attribute__((packed));

struct gf_team_active_score_change_in {
    uint32_t    teamid;
    uint32_t    type;
    uint32_t    active_flag;
    uint32_t    score;
}__attribute__((packed));

struct gf_get_team_top10_in {
}__attribute__((packed));

struct gf_get_team_top10_out_header {
	uint32_t cnt;
}__attribute__((packed));

struct gf_get_team_top10_out_item {
	uint32_t teamid;
	char team_name[16];
	uint32_t captain_uid;
	uint32_t captain_role_tm;
	char captain_name[16];
	uint32_t member_cnt;
	uint32_t score;
	uint32_t team_exp;
}__attribute__((packed));

struct gf_get_team_top100_out_item {
	uint32_t teamid;
	uint32_t captain_uid;
	uint32_t captain_role_tm;
	uint32_t score;
}__attribute__((packed));

struct gf_contribute_team_in {
    uint32_t    teamid;
    uint32_t    coin;
}__attribute__((packed));

struct gf_contribute_team_out {
    uint32_t    teamid;
    uint32_t    team_coin;
    //uint32_t    player_coin;
}__attribute__((packed));

struct gf_fetch_team_coin_in {
    uint32_t    teamid;
    uint32_t    coin;
}__attribute__((packed));

struct gf_fetch_team_coin_out {
    uint32_t    teamid;
    uint32_t    team_coin;
    //uint32_t    player_coin;
}__attribute__((packed));

struct gf_team_active_enter_in {
    uint32_t    teamid;
    uint32_t    need_coin;
    uint32_t    active_flag;
}__attribute__((packed));

typedef gf_team_active_enter_in gf_team_active_enter_out;

struct gf_get_team_active_top100_in {
    uint32_t    type; //1:楼兰杯2:天下第一比武大会
}__attribute__((packed));

struct gf_get_team_active_top100_out_header {
	uint32_t timestamp;
    uint32_t type;
	uint32_t cnt;
}__attribute__((packed));

struct gf_get_team_active_top100_out_item {
	uint32_t    id;
	char        name[16];
	uint32_t    captain_id;
	uint32_t    captain_tm;
	char        captain_name[16];
	uint32_t    member_cnt;
	uint32_t    active_score;
    uint32_t    active_index; //
	uint32_t    score;
	uint32_t    team_exp;
}__attribute__((packed));


struct gf_set_true_wusheng_info_in {
	uint32_t uid;
	uint32_t role_tm;
	uint32_t role_type;
	char  nick_name[16];
	uint32_t pass_time;
}__attribute__((packed));

struct gf_list_true_wusheng_info_out_header {
	uint32_t cnt;
}__attribute__((packed));

typedef gf_set_true_wusheng_info_in gf_list_true_wusheng_info_out_item;

struct gf_get_other_info_list_out_header {
	uint32_t cnt;
}__attribute__((packed));

struct gf_get_other_info_list_out_item {
	uint32_t type;
	uint32_t value;
}__attribute__((packed));

struct gf_set_other_info_in {
	uint32_t type;
	uint32_t value;
}__attribute__((packed));

struct gf_list_ranker_info_in {
	uint32_t ranker_id;
	uint32_t range_type;
	uint32_t ranker_range;
}__attribute__((packed));

struct gf_list_ranker_info_out_header {
	uint32_t ranker_id;
	uint32_t info_count;
}__attribute__((packed));

struct gf_list_ranker_info_out_item {
	uint32_t userid;
	uint32_t role_tm;
	uint32_t role_type;
	char nick_name[16];
	uint32_t score;
}__attribute__((packed));

struct gf_set_ranker_info_in {
	uint32_t ranker_id;
	uint32_t range_type;
	uint32_t userid;
	uint32_t role_tm;
	uint32_t role_type;
	char nick_name[16];
	uint32_t score;
}__attribute__((packed));

struct gf_set_power_user_in {
    uint32_t    power_user;
    uint32_t    skill_point;
    uint32_t    item_id_1;
    uint32_t    item_id_2;
}__attribute__((packed));

typedef gf_set_power_user_in gf_set_power_user_out;


///// 师徒
struct gf_get_master_out_header {
    uint32_t    cnt;
}__attribute__((packed));

struct gf_get_master_out_item {
    uint32_t    master_id;
    uint32_t    master_tm;
	uint32_t 	join_tm;
}__attribute__((packed));

struct gf_get_apprentice_out_header {
    uint32_t    cnt;
}__attribute__((packed));

struct gf_get_apprentice_out_item {
    uint32_t    uid;
    uint32_t    roletm;
    uint32_t    lv;
    uint32_t    tm;
    uint32_t    grade;
}__attribute__((packed));

struct gf_master_add_apprentice_in {
    uint32_t    action; // 1--master do 2--prentice do
    uint32_t    uid;
    uint32_t    roletm;
    uint32_t    tm;
    uint32_t    prentice_lv;
    uint32_t    grade;
}__attribute__((packed));

struct gf_master_add_apprentice_out {
    uint32_t    action; // 1-- add success; 0-- add fail
    uint32_t    uid;
    uint32_t    roletm;
    uint32_t    tm;
    uint32_t    prentice_lv;
    uint32_t    grade;
}__attribute__((packed));

struct gf_master_del_apprentice_in {
    uint32_t    action; // 1--master do 2--prentice do 
    uint32_t    uid;
    uint32_t    roletm;
}__attribute__((packed));

typedef gf_master_del_apprentice_in gf_master_del_apprentice_out;

struct gf_set_prentice_grade_in {
    uint32_t    prentice_id;
    uint32_t    prentice_tm;
    uint32_t    grade;
    uint32_t    is_or_operate; //0--& operate; 1--| operate
}__attribute__((packed));

struct gf_get_prentice_cnt_out {
    uint32_t    cnt;
}__attribute__((packed));


struct gf_set_reward_player_in {
    uint32_t    userid;
    uint32_t    roletm;
    uint32_t    reward_id;
}__attribute__((packed));

struct gf_get_reward_player_out_header {
    uint32_t    timestemp;
    uint32_t    cnt;
}__attribute__((packed));

struct gf_get_reward_player_out_item {
    uint32_t    id;
    uint32_t    userid;
    uint32_t    roletm;
    uint32_t    reward_id;
    uint32_t    reward_tm;
    uint32_t    reward_flag;
}__attribute__((packed));

struct gf_set_player_reward_flag_in {
    uint32_t    key;
}__attribute__((packed));


struct gf_get_decorate_list_out_header {
    uint32_t    cnt;
}__attribute__((packed));

struct gf_get_decorate_list_out_item {
    uint32_t    decorate_id;
    uint32_t    decorate_lv;
    uint32_t    decorate_tm;
    uint32_t    status;
}__attribute__((packed));

struct gf_set_decorate_in {
    uint32_t    decorate_id;
}__attribute__((packed));

struct gf_set_decorate_out {
    uint32_t    decorate_id;
    uint32_t    decorate_lv;
    uint32_t    decorate_tm;
}__attribute__((packed));

struct gf_add_decorate_lv_in_header {
    uint32_t    flag;
    uint32_t    decorate_id;
    uint32_t    fillingin;
    uint32_t    itemcnt;
    uint32_t    del_item_arr[];
}__attribute__((packed));

struct gf_set_team_member_level_in {
	uint32_t   team_id;
	uint32_t   userid;
	uint32_t   role_tm;
	uint32_t   level;
}__attribute__((packed));

typedef gf_set_team_member_level_in gf_set_team_member_level_out;

struct gf_change_team_name_in {
	uint32_t team_id;
	char  team_name[NICK_LEN];
}__attribute__((packed));
struct gf_change_team_mcast_in {
	uint32_t team_id;
	char mcast_info[MCAST_LEN];
}__attribute__((packed));

struct gf_safe_trade_item_in_item1{
    uint32_t    item_id;
    uint32_t    item_cnt;
}__attribute__((packed));

struct gf_safe_trade_item_in_item2{
    uint32_t    attire_id;
    uint32_t    unique_id;
    uint32_t    lv;
}__attribute__((packed));

struct gf_safe_trade_item_in_header {
    uint32_t    trade_id;
    uint32_t    action;
    uint32_t    xiaomee;
    uint32_t    item_cnt;
    uint32_t    attire_cnt;
}__attribute__((packed));

typedef gf_safe_trade_item_in_header gf_safe_trade_item_out_header;

struct gf_get_other_active_in {
	uint32_t active_id;
}__attribute__((packed));

struct gf_get_other_active_out {
	uint32_t active_id;
	char  info_buf[ACTIVE_BUF_LEN];
}__attribute__((packed));	

struct gf_set_other_active_in {
	uint32_t active_id;
	uint32_t reset_type;
	uint32_t out_tm;
	char info_buf[ACTIVE_BUF_LEN];
}__attribute__((packed));

struct gf_list_simple_role_info_in {
	uint32_t userid;
}__attribute__((packed));

struct gf_list_simple_role_info_out_header {
	uint32_t role_cnt;
}__attribute__((packed));

struct gf_list_simple_role_info_out_item {
	uint32_t userid;
	uint32_t role_regtime;
	uint32_t role_type;
	uint32_t level;
	char     nick_name[NICK_LEN];
}__attribute__((packed));

struct gf_get_other_active_list_out_header {
    uint32_t    cnt;
}__attribute__((packed));

struct gf_get_other_active_list_out_item {
	uint32_t active_id;
	char  info_buf[ACTIVE_BUF_LEN];
}__attribute__((packed));

struct gf_get_single_other_active_in {
    uint32_t    active_id;
    uint32_t    uid;
    uint32_t    rtm;
}__attribute__((packed));

struct gf_get_single_other_active_out {
	uint32_t active_id;
	char  info_buf[ACTIVE_BUF_LEN];
}__attribute__((packed));

struct gf_set_player_fate_in {
    uint32_t    money;
    uint32_t    p_exp;
    uint32_t    m_exp;
}__attribute__((packed));

typedef gf_set_player_fate_in gf_set_player_fate_out;


struct gf_search_team_info_in {
	uint32_t team_id;
	uint32_t captain_id;
}__attribute__((packed));

struct gf_search_team_info_out_header {
	uint32_t team_cnt;
}__attribute__((packed));

struct gf_search_team_info_out_item {
	uint32_t teamid;
	char team_name[16];
	uint32_t captain_uid;
	uint32_t captain_role_tm;
	char captain_name[16];
	uint32_t member_cnt;
	uint32_t score;
	uint32_t team_exp;
}__attribute__((packed));

struct gf_gain_team_exp_in {
	uint32_t team_id;
	uint32_t gain_exp;
}__attribute__((packed));	

struct gf_gain_team_exp_out {
	uint32_t new_exp;
}__attribute__((packed));

struct gf_group_t {
    uint32_t    _level;
    uint32_t    member[10];
}__attribute__((packed));

struct gf_set_role_god_guard_in {
    uint32_t    money;
    gf_group_t  group;
}__attribute__((packed));

struct gf_team_reduce_tax_in {
	uint32_t team_id;
	uint32_t reduce_coin;
	uint32_t update_tm;
}__attribute__((packed));

typedef gf_team_reduce_tax_in gf_team_reduce_tax_out;

struct gf_distract_clothes_strength_in {
	uint32_t keep_flag;
	uint32_t old_clothes_uid;
	uint32_t new_clothes_uid;
	uint32_t new_level;
	uint32_t stone_id;
	uint32_t reduce_coin;
}__attribute__((packed));

typedef gf_distract_clothes_strength_in gf_distract_clothes_strength_out;

struct gf_get_stat_info_out_header {
	uint32_t stat_cnt;
}__attribute__((packed));

struct gf_get_stat_info_out_item {
	uint32_t stat_id;
	uint32_t stat_cnt;
}__attribute__((packed));

struct gf_set_stat_info_in {
	uint32_t stat_id;
	uint32_t add_cnt;
}__attribute__((packed));
#endif
