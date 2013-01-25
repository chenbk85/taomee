
#ifndef MOLE2_CLI_PROTO_H
#define MOLE2_CLI_PROTO_H

#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

#include <arpa/inet.h>
#ifdef __cplusplus
extern "C"
{
#endif
#include <async_serv/async_serv.h>

#ifdef __cplusplus
}
#endif
//新的命令分发
#include <libtaomee++/proto/Ccmd_map.h>
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>
#include <libtaomee++/utils/strings.hpp>
#include "./proto/mole2_online.h" 
#include "./proto/mole2_online_enum.h" 



#include "benchapi.h"
#include "global.h"
#include "map.h"
#include "sprite.h"
#include "dbproxy.h"

typedef struct cached_pkg{
	uint16_t	len;
	uint8_t		pkg[];
}__attribute__((packed))cached_pkg_t;

enum {
	proto_cli_cmd_start					= 1000,
	proto_cli_login						= 1001,
	proto_cli_map_users					= 1002,//
	proto_cli_leave_map					= 1003,
	proto_cli_get_user_info				= 1005,//
	proto_cli_get_user_detail			= 1006,//
	proto_cli_set_play_mode 			= 1008,

	proto_cli_user_talk					= 1010,
	proto_cli_user_show					= 1011,
	proto_cli_set_flag					= 1012,
	proto_cli_vip_update				= 1013,
	proto_cli_chk_users_online			= 1014,
	proto_cli_chk_online_id				= 1016,
	proto_cli_chk_str_valid				= 1018,
	proto_cli_post_msg					= 1020,
	
	proto_cli_set_user_nick				= 1023,
	proto_cli_user_cure					= 1025,
	proto_cli_user_set_position			= 1026,
	proto_cli_notify_user_levelup		= 1028,
	proto_cli_set_user_color			= 1029,
	proto_cli_set_busy_state			= 1030,
	proto_cli_kick_user_offline			= 1032,
	proto_cli_reset_user_walk			= 1034,
	proto_cli_chk_teamate_enter_map		= 1035,
	proto_cli_allocate_exp				= 1036,
	proto_cli_keep_sprite_alive			= 1038,
	proto_cli_start_double_exp			= 1039,
	proto_cli_add_session				= 1040,

	proto_cli_add_friend				= 1051,
	proto_cli_rsp_friend_add			= 1052,
	proto_cli_del_friend				= 1053,
	proto_cli_bklist_add				= 1055,
	proto_cli_bklist_del				= 1056,
	proto_cli_get_bklist				= 1057,
	proto_cli_noti_friend_del			= 1058,
	proto_cli_get_invited				= 1059,
	proto_cli_noti_relation_up			= 1064,

	proto_cli_buy_cloth					= 1101,
	proto_cli_get_pakt_cloth			= 1102,
	proto_cli_cloth_disappear			= 1103,
	proto_cli_del_cloth					= 1104,
	proto_cli_get_body_clothes			= 1106,
	proto_cli_buy_normal_item			= 1108,
	proto_cli_get_normal_item_list		= 1109,
	
	proto_cli_sell_items				= 1112,
	proto_cli_del_normal_item			= 1113,
	proto_cli_use_medical_item			= 1114,
	proto_cli_re_birth					= 1115,
	proto_cli_use_handbook_item			= 1116,
	proto_cli_chk_team_item				= 1118,
	proto_cli_repair_cloth				= 1119,
	proto_cli_get_house_item_list		= 1121,
	proto_cli_item_in_out				= 1123,
	proto_cli_cloth_in_out				= 1124,
	proto_cli_del_house_item			= 1125,
	proto_cli_composite_item			= 1126,
	proto_cli_exchange_gift_normal		= 1127,
	proto_cli_exchange_gift_spec		= 1129,
	proto_cli_exchange_gift_pet			= 1130,
	proto_cli_composite_cloth			= 1131,

    proto_cli_add_item_feed_count       = 1133,
    proto_cli_get_sth_done              = 1134,
    proto_cli_set_sth_done              = 1135,
    
	proto_cli_get_monster_handbook  	= 1136,
	proto_cli_set_monster_handbook		= 1137,
	proto_cli_notify_get_boss_egg		= 1138,
	proto_cli_use_buff_item				= 1139,
	proto_cli_use_energy_item			= 1140,
	proto_cli_hangup_activity			= 1141,
	proto_noti_user_consume_item		= 1142,

	//new task version
	proto_cli_get_task_detail			= 1158,
	proto_cli_write_task_diary			= 1159,
	proto_cli_recv_task					= 1162,
	proto_cli_cancel_task				= 1164,
	proto_cli_set_task_clibuf			= 1165,
	proto_cli_get_task_svrbuf			= 1166,

	proto_cli_set_buff					= 1168,
	
	proto_cli_challenge					= 1300,
	proto_cli_accept_challenge			= 1301,
	proto_cli_noti_team_challeg			= 1302,
	proto_cli_noti_team_challeged		= 1303,
	proto_cli_challenge_npc				= 1304,
	proto_cli_noti_battle_created		= 1305,
	proto_cli_load_battle				= 1306,
	proto_cli_noti_battle_started		= 1307,
	proto_cli_battle_attack				= 1308,
	proto_cli_use_medical   			= 1309,
	proto_cli_battle_attacks_info		= 1310,
	proto_cli_battle_round_result		= 1311,
	proto_cli_round_info				= 1312,
	proto_cli_noti_user_fighted			= 1313,
	proto_cli_next_fight_step			= 1314,
	proto_cli_noti_user_out				= 1315,
	proto_cli_noti_load_rate 			= 1316,
	proto_cli_load_ready  				= 1317,
	proto_cli_sync_battle_end			= 1318,
	proto_cli_battle_end				= 1319,//need to send to watchers

	proto_cli_reject_challenge			= 1320,
	proto_cli_delete_batr_team			= 1322,
	proto_cli_btrtem_add_mbr			= 1323,
	proto_cli_sync_pkinfo				= 1324,
	proto_cli_apply_pk					= 1325,
	proto_cli_cancel_pk_apply			= 1326,
	proto_cli_btrtem_apply				= 1327,
	proto_cli_btrtem_noti_sb_app		= 1328,
	proto_cli_register_for_team			= 1329,
	proto_cli_btrtem_kick_mbr			= 1331,
	proto_cli_btrtem_mbr_leave			= 1332,
	proto_cli_rsp_team_info				= 1333,
	proto_cli_btrtem_enter_map			= 1334,
	proto_cli_cancel_challenge			= 1336,
	proto_cli_noti_user_change_pos		= 1337,
	proto_cli_use_beast_card			= 1338,
	proto_cli_battle_swap_pet			= 1339,
	proto_cli_user_offline				= 1340,
	proto_cli_noti_battle_accepted		= 1341,
	proto_cli_noti_tleader_action		= 1342,
	proto_cli_noti_team_state			= 1343,
	proto_cli_animation_finish			= 1344,//need to send to watchers
	proto_cli_btrsvr_conn_closed		= 1345,
	proto_cli_change_team_leader		= 1346,
	proto_cli_battle_leave_team			= 1347,
	proto_cli_clear_btr_info			= 1348,
	proto_cli_get_last_kill_beastgrp	= 1349,
	proto_cli_grp_challenged			= 1351,
	proto_cli_reset_attr				= 1586,

	proto_cli_add_npc_to_team			= 1400,
	proto_cli_text_notify				= 1501,

	proto_cli_set_pet_state				= 1551,
	proto_cli_set_pet_nick				= 1552,
	proto_cli_get_pet_info				= 1553,
	proto_cli_get_pet_list				= 1554,
	proto_cli_cure_pet					= 1555,
	proto_cli_cure_pet_injury			= 1556,
	proto_cli_get_user_pet_list			= 1557,
	proto_cli_pet_follow				= 1558,
	proto_cli_get_pet_inhouse			= 1559,
	proto_cli_update_pet_location		= 1560,
    proto_cli_del_pet                   = 1561,
    proto_cli_get_other_pet_info		= 1562,
    proto_cli_exchange_pet				= 1563,
    proto_cli_get_all_petcnt			= 1564,

	proto_cli_buy_skill					= 1600,
	proto_cli_pet_buy_skill				= 1601,
	proto_cli_get_skill_list			= 1602,
	proto_cli_del_skill					= 1604,
	proto_cli_pet_del_skill				= 1605,
	proto_cli_pet_add_base_attr			= 1606,
	proto_cli_unlock_pet_talent_skill	= 1607,
	//trade
	proto_cli_noti_shop_register		= 1641,
	proto_cli_noti_shop_change			= 1642,
	proto_cli_trade_sell_item			= 1643,

	proto_cli_check_rand_info			= 1702,
	proto_cli_get_vip_item				= 1703,

	proto_cli_register_bird_map			= 1800,
	proto_cli_get_bird_maps				= 1801,
	proto_cli_check_team_bird_map		= 1803,
	proto_cli_register_boss_map			= 1810,
	proto_cli_get_boss_maps				= 1811,
	proto_cli_check_team_boss_map		= 1813,
	proto_cli_get_map_state				= 1814,
	proto_cli_set_map_state				= 1815,

	proto_cli_get_handbook_info			= 1850,
	proto_cli_get_g_book				= 1851,
	proto_cli_get_g_book_xiaomee		= 1852,
	proto_cli_notice_team_info			= 1853,
//box
	proto_cli_open_box					= 1900,
	proto_cli_box_opened				= 1901,
	proto_cli_box_activated				= 1902,
	proto_cli_get_box_active			= 1904,
//maze
	proto_cli_get_maze_in_map		= 1909,
	proto_cli_maze_state_change		= 1910,
	proto_cli_maze_open_portal		= 1911,
	proto_cli_maze_goto_map			= 1912,
	proto_cli_get_shop_in_map		= 1913,
//mail
	proto_cli_send_mail					= 1950,
	proto_cli_get_mail_simple			= 1952,
	proto_cli_get_mail_content			= 1953,
	proto_cli_mail_set_flag				= 1955,
	proto_cli_mail_notify				= 1956,
	proto_cli_mail_sys_send				= 1957,
	proto_cli_mail_items_get 			= 1958,

	proto_cli_sys_info					= 1970,
    
    proto_cli_get_home_info             = 2000,
    proto_cli_expbox_change				= 2001,
    proto_cli_update_home_flag			= 2002,

	proto_cli_get_water_cnt				= 2050,
	proto_cli_use_water					= 2051,
	
	proto_cli_pic_srv_ip_port			= 2100,
	proto_cli_read_news					= 2150,

	proto_cli_check_magic_code			= 3001,
	proto_cli_use_magic_code			= 3002,

	proto_cli_mall_get_balance		= 3200,
	proto_cli_mall_get_items_price	= 3201,
	proto_cli_mall_get_item_detail	= 3202,
	proto_cli_mall_buy_item			= 3203,

	proto_cli_just_for_online			= 4095,
	proto_cli_version					= 4096,
	proto_cli_cmd_end					= 50000
};

enum {
	//base
	cli_err_client_not_proc 	= 200000,
	cli_err_user_offline		= 200003,
	cli_err_system_error		= 200006,
	cli_err_login_from_other	= 200007,
	cli_err_dirty_word			= 200008,
	cli_err_user_registerd		= 200009,
	cli_err_attr_invalid		= 200010,
	cli_err_system_busy			= 200012,
	cli_err_exp_not_enough		= 200014,
	cli_err_exp_too_much		= 200015,
	cli_err_attr_over_max		= 200016,
	cli_err_attr_not_enough		= 200017,
	cli_err_not_right_time		= 200019,
	cli_err_not_vip				= 200020,
	cli_err_need_rest			= 200021,
	cli_err_expfactor_top		= 200022,
	cli_err_not_right_time_2	= 200023,
	cli_err_not_right_prof 		= 200024,
	//team
	cli_err_other_not_leader	= 200051,
	cli_err_self_not_leader		= 200052,
	cli_err_alrdy_join_team		= 200053,
	cli_err_too_many_players	= 200054,
	
	cli_err_self_be_fighting	= 200056,
	cli_err_other_be_fighting	= 200057,
	cli_err_not_in_same_team	= 200058,
	cli_err_team_pos_invalid	= 200059,
	cli_err_not_in_same_map		= 200060,
	cli_err_battle_canceled		= 200061,
	cli_err_refuse_team_invite	= 200062,
	cli_err_user_busy			= 200063,
	cli_err_refuse_team_apply	= 200064,
	cli_err_switch_flag_invalid	= 200065,
	cli_err_not_be_fighting		= 200066,
	cli_err_team_wait_animation	= 200067,
	cli_err_self_chalk_other	= 200068,
	cli_err_other_chalk_other	= 200069,
	cli_err_not_beast_map		= 200070,
	cli_err_invalid_grpid		= 200071,
	cli_err_not_applying_pk		= 200072,
	cli_err_already_start_pk	= 200073,
	cli_err_applying_pk			= 200074,
	cli_err_fight_too_often		= 200075,
	cli_err_not_happen_monster	= 200076,
	cli_err_not_win_beastgrp	= 200077,
	cli_err_g_book_registerd	= 200078,
	cli_err_beast_challenged	= 200079,
	//task
	cli_err_ptask_not_fin		= 200106,
	cli_err_task_not_finished	= 200109,
	cli_err_task_have_finished	= 200110,
	cli_err_task_repeat_max		= 200111,
	cli_err_task_reach_max		= 200112,
	//item
	cli_err_item_id_invalid		= 200151,
	cli_err_item_cnt_max		= 200152,
	cli_err_item_cannot_buy		= 200153,
	cli_err_item_cannot_sell	= 200154,
	cli_err_item_cannot_del		= 200156,
	cli_err_havnot_this_item	= 200159,
	cli_err_cloth_id_invalid	= 200163,
	cli_err_cloth_not_existed	= 200164,
	cli_err_cloth_needno_repair	= 200165,
	cli_err_already_in_bag		= 200166,
	cli_err_cannot_composite	= 200167,
	cli_err_cannot_exchange		= 200168,
	cli_err_exchange_id_invalid	= 200169,
	cli_err_pkpt_day_max		= 200170,
	cli_err_feed_count_err      = 200171,
	cli_err_already_get_this	= 200174,
	cli_err_attr_mismatch		= 200175,
	cli_err_list_full			= 200176,
	cli_err_cnt_too_many		= 200177,

	/*! user */
	cli_err_xiaomee_not_enough	= 200200,
	cli_err_not_need_cure		= 200203,
	cli_err_level_not_fit		= 200204,
	cli_err_not_need_reset		= 200205,
	//skill
	cli_err_skill_have_got		= 200252,
	cli_err_skill_cnt_max		= 200253,
	cli_err_skill_not_have		= 200254,
	cli_err_skill_cannot_study	= 200255,
	//pet
	cli_err_pet_not_rdy_fight	= 200300,
	cli_err_pet_type_invalid	= 200302,
	cli_err_pet_standby_too_much= 200304,
	cli_err_pet_bag_full		= 200305,
	cli_err_pet_not_inbag		= 200306,
	cli_err_pet_not_standby		= 200307,
	cli_err_petid_mismatch		= 200301,
	
	/*! map */
	cli_err_mapid_invalid		= 200308,
	cli_err_not_birdmap			= 200309,
	cli_err_same_map			= 200310,
	cli_err_havnot_map_entry	= 200311,
	cli_err_chocobo_not_existed	= 200312,
    cli_err_pet_alreay_inbag    = 200313,
    cli_err_not_right_map		= 200314,
    cli_err_have_not_win_boss	= 200315,
    cli_err_pet_cnt_max			= 200316,
    cli_err_mapcopy_not_enough	= 200318,
	cli_err_not_same_map		= 200319,
	/*! rand item */
	cli_err_rand_id_invalid		= 200350,
	cli_err_rand_day_max		= 200351,
	cli_err_rand_all_max		= 200352,
	cli_err_rand_life_max		= 200353,
	cli_err_rand_week_max		= 200354,
	/*!  honor */
	cli_err_honor_not_existed	= 200400,
	cli_err_honor_id_invalid	= 200401,
	
	/*! box */
	cli_err_boxid_invalid		= 200450,
	cli_err_box_not_active		= 200451,
	cli_err_not_near_box		= 200452,
	cli_err_portal_unuseable	= 200453,
	cli_err_portal_have_closed	= 200454,
	cli_err_not_start_fish		= 200455,
	cli_err_not_for_fish		= 200456,
	cli_err_times_limit			= 200457,		
	/*! mail */
	cli_err_mail_len			= 200500,

	cli_err_energy_not_enough	= 200501,
	cli_err_pkpt_not_enough		= 200502,
	cli_err_not_registerd		= 200503,
	cli_err_beast_no_enough		= 200504,
	cli_err_ever_surveyed		= 200506,
	cli_err_vacancy_no_enough	= 200507,
	cli_err_hero_cup_new		= 200508,
	cli_err_skill_hang_max		= 200509,
	cli_err_not_engage_game		= 200510,

	
	/*! trade */
	cli_err_roomid_invalid		= 200600,
	cli_err_enter_fail			= 200601,
	cli_err_shop_state			= 200602,
	cli_err_open_shop_fail		= 200603,
	cli_err_is_opening_shop		= 200604,
	cli_err_open_too_often		= 200605,
	cli_err_price_too_high		= 200606,
	cli_err_cannot_get			= 200607,
	cli_err_not_add_item		= 200608,
};

enum {
	// we return errno from dbproxy by plusing 100000 to dberr (100000 + dberr)
	cli_err_base_dberr		= 100000,
	cli_err_base_codeerr		= 300000,
	cli_err_base_mallerr		= 400000
};

/**
  * @brief some constants
  */
enum {
	/*! max acceptable length in bytes for each client package */
	cli_proto_max_len	= 32 * 1024,
};

/**
  * @brief compare methods
  */
enum {
	/*! len must equal to another given len */
	cmp_must_eq		= 1,
	/*! len must be greater or equal to another given len */
	cmp_must_ge		= 2,
};

#define CHECK_CALLBACK_ERR(p_, base_, err_) \
		do { \
			if ( (err_) ) { \
				return send_to_self_error(p_, (p_)->waitcmd, base_ + (err_), 1); \
			} \
		} while (0)


#define CHECK_DBERR(p_, err_) CHECK_CALLBACK_ERR(p_,cli_err_base_dberr,err_)
#define CHECK_CODEERR(p_, err_) CHECK_CALLBACK_ERR(p_,cli_err_base_codeerr,err_)
#define CHECK_MALL_ERR(p_, err_)  CHECK_CALLBACK_ERR(p_,cli_err_base_mallerr,err_)


int init_cli_proto_handles(int rstart);
int dispatch(void* data, fdsession_t* fdsess, int first_tm);
void handle_cmd_busy_sprite();
void add_cmd_pending_head(sprite_t* p, void* data);

void response_proto_get_sprite(sprite_t* p, int cmd, int is_detail, sprite_t* req);
void response_proto_get_sprite2(sprite_t* p, int cmd, sprite_t* req, int completed);
int  response_proto_list_items(sprite_t* p, uint32_t id, uint32_t* itms, int count);
int  response_proto_list_items2(sprite_t* p, uint32_t id, uint8_t* buf, int count);
int  pkg_proto_throw_item(uint8_t* buf, sprite_t* p, uint32_t itemid, int x, int y);
int  notify_systime(sprite_t* p, map_id_t* maps);
int send_msg_to_self(sprite_t *p, int cmd, Cmessage *c_in, int completed);

static inline void
init_proto_head(void* buf, int cmd, int len)
{
#ifdef __cplusplus
	protocol_t* p = reinterpret_cast<protocol_t*>(buf);
#else
	protocol_t* p = buf;
#endif
	p->len = htonl(len);
	p->cmd = htons(cmd);
	p->ret = 0;
}

static inline void
response_proto_head(sprite_t *p, int cmd, int compet, int broadcast)
{
	uint8_t msg[sizeof (protocol_t)];

	init_proto_head(msg, cmd, sizeof (protocol_t));
	if (broadcast)
		send_to_map(p, msg, sizeof (protocol_t), compet,1);
	else
		send_to_self(p, msg, sizeof (protocol_t), compet);
}

static inline void
response_proto_head_not_complete(sprite_t *p, int cmd, int broadcast)
{
	uint8_t msg[sizeof (protocol_t)];

	init_proto_head(msg, cmd, sizeof (protocol_t));
	if (broadcast)
		send_to_map(p, msg, sizeof (protocol_t), 0,1);
	else
		send_to_self(p, msg, sizeof (protocol_t), 0);
}

static inline void
response_proto_str(sprite_t *p, int cmd, uint32_t len, const void* buf, int compet, int broadcast)
{
	memcpy(msg + sizeof (protocol_t), buf, len);
	init_proto_head(msg, cmd, sizeof (protocol_t) + len);
	if (broadcast)
		send_to_map(p, msg, sizeof (protocol_t) + len, 1,1);
	else
		send_to_self(p, msg, sizeof (protocol_t) + len, 1);
}

static inline void
response_proto_uint32(sprite_t* p, int cmd, uint32_t val, int compet, int broadcast)
{
	*(uint32_t*)(msg + sizeof(protocol_t)) = htonl(val);
	init_proto_head(msg, cmd, sizeof(protocol_t) + 4);
	if (broadcast) {
		send_to_map(p, msg, sizeof(protocol_t) + 4, compet,1);
	} else {
		send_to_self(p, msg, sizeof(protocol_t) + 4, compet);
	}
}

static inline void
response_proto_uint32_uint32(sprite_t* p, int cmd, uint32_t val1, uint32_t val2, int compet, int broadcast)
{
	*(uint32_t*)(msg + sizeof(protocol_t)) = htonl(val1);
	*(uint32_t*)(msg + sizeof(protocol_t) + 4) = htonl(val2);
	init_proto_head(msg, cmd, sizeof(protocol_t) + 8);
	if (broadcast) {
		send_to_map(p, msg, sizeof(protocol_t) + 8, compet, 1);
	} else {
		send_to_self(p, msg, sizeof(protocol_t) + 8, compet);
	}
}

static inline void
response_proto_tri_uint32(sprite_t* p, int cmd, uint32_t val1, uint32_t val2, uint32_t val3,int compet, int broadcast)
{
	*(uint32_t*)(msg + sizeof(protocol_t)) = htonl(val1);
	*(uint32_t*)(msg + sizeof(protocol_t) + 4) = htonl(val2);
	*(uint32_t*)(msg + sizeof(protocol_t) + 8) = htonl(val3);
	init_proto_head(msg, cmd, sizeof(protocol_t) + 12);
	if (broadcast) {
		send_to_map(p, msg, sizeof(protocol_t) + 12, compet, 1);
	} else {
		send_to_self(p, msg, sizeof(protocol_t) + 12, compet);
	}
}

static inline void
response_proto_uint8(sprite_t* p, int cmd, uint8_t val, int compet, int broadcast)
{
	*(uint8_t*)(msg + sizeof(protocol_t)) = val;
	init_proto_head(msg, cmd, sizeof(protocol_t) + 1);
	if (broadcast) {
		send_to_map(p, msg, sizeof(protocol_t) + 1, compet,1);
	} else {
		send_to_self(p, msg, sizeof(protocol_t) + 1, compet);
	}
}

static inline void
response_proto_head_mapid(map_id_t mapid, int cmd)
{
	init_proto_head(msg, cmd, sizeof(protocol_t));
	send_to_map3(mapid, msg, sizeof(protocol_t));
}

static inline void
response_proto_uint8_mapid(map_id_t mapid, int cmd, uint8_t val)
{
	*(uint8_t*)(msg + sizeof(protocol_t)) = val;
	init_proto_head(msg, cmd, sizeof(protocol_t) + 1);
	send_to_map3(mapid, msg, sizeof(protocol_t));
}

static inline void
response_proto_uint32_mapid(map_id_t mapid, int cmd, uint32_t val)
{
	*(uint32_t*)(msg + sizeof(protocol_t)) = htonl(val);
	init_proto_head(msg, cmd, sizeof(protocol_t) + 4);

	send_to_map3(mapid, msg, sizeof(protocol_t) + 4);
}

static inline void
response_proto_uint32_uint32_mapid(map_id_t mapid, int cmd, uint32_t val1, uint32_t val2)
{
	*(uint32_t*)(msg + sizeof(protocol_t)) = htonl(val1);
	*(uint32_t*)(msg + sizeof(protocol_t) + 4) = htonl(val2);
	init_proto_head(msg, cmd, sizeof(protocol_t) + 8);

	send_to_map3(mapid, msg, sizeof(protocol_t) + 8);
}

static inline uint32_t calc_seqno(uint32_t pkglen, uint32_t seqno, uint16_t cmd, uint8_t crc8_val)
{
	return seqno - seqno / 7 + 147 + pkglen % 21 + cmd % 13 + crc8_val;
}

static inline uint8_t calc_crc8(uint8_t* body, uint32_t body_len)
{
	uint8_t crc8_val = 0;
	for (uint32_t i = 0; i < body_len; i++) {
		crc8_val ^= (body[i] & 0xff);
	}
	return crc8_val;
}

static inline bool is_invaild_seqno(sprite_t* p, uint32_t head_len, uint32_t head_seqno, uint16_t head_cmd, uint8_t* body, uint32_t body_len,uint32_t &real_seq)
{
	uint8_t crc8_val = calc_crc8(body, body_len);
	uint32_t seqno_val = calc_seqno(head_len, p->seq, head_cmd, crc8_val);//p->seqno - p->seqno / 7 + 147 + head_len % 21 + head_cmd % 13;
	//DEBUG_LOG("---------- cmd=%u head_seq=%u len=%u p->seq=%u now=%u crc=%u",head_cmd,head_seqno,head_len,p->seq,seqno_val,crc8_val );
	real_seq = seqno_val;
	if (seqno_val != head_seqno) {
		return true;
	}
	return false;
}


#endif
