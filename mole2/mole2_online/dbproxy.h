/**
 *============================================================
 *  @file      dbproxy.hpp
 *  @brief    dbproxy related functions are declared here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef MOLE2_DBPROXY_H
#define MOLE2_DBPROXY_H

#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "svr_proto.h"
#include "./proto/mole2_db_enum.h"

/**
  * @brief command id for dbproxy
  */
enum db_cmd_t {
	proto_db_set_user_nick		 = 0x0903,
	proto_db_set_user_color		 = 0x0905,
	proto_db_add_xiaomee		 = 0x0907,
	proto_db_set_position		 = 0x0908,
	proto_db_add_base_attr		 = 0x090A,
	proto_db_cure_all			 = 0x090D,
	proto_db_get_sprite          = 0x0838,
	proto_db_add_hp_mp			 = 0x0939,
	proto_db_user_set_injury	 = 0x0975,
	proto_db_get_other_user_info = 0x08B8,
	proto_db_allocate_exp		 = 0x09BC,
	proto_db_set_energy			 = 0x09C1,
	proto_db_set_winbossid		 = 0x09C8,
	proto_db_get_offline_msg	 = 0x0821,
	proto_db_del_task		 	 = 0x09C6,
	proto_db_add_user_honor		 = 0x09CB,
	
	proto_db_get_pet_list		 = 0x089A,
	proto_db_set_pet_nick		 = 0x0911,
	proto_db_pet_cure			 = 0x0914,
	proto_db_pet_add_base_attr	 = 0x0916,

	proto_db_add_item_feed_count = 0x0919,
    proto_db_get_item_feed_count = 0x081C,

	proto_db_pet_cure_injury	 = 0x092D,
	proto_db_set_pet_hp_mp		 = 0x093A,
	proto_db_pet_set_injury		 = 0x0976,
	proto_db_pet_get_info	 	 = 0x0895,
	proto_db_pet_set_state		 = 0x0997,
	proto_db_pet_follow_set		 = 0x09A6,
	
	proto_db_add_friend 		 = 0x0923,
	proto_db_del_friend			 = 0x0924,
 	proto_db_bklist_add			 = 0x0927,
 	proto_db_bklist_del			 = 0x0928,
 	proto_db_get_bklist			 = 0x0829,

	proto_db_get_pakt_clothes	 = 0x0835,
	proto_db_add_cloth 			 = 0x0930,
	proto_db_cloth_in_out		 = 0x0933,
	proto_db_del_cloth			 = 0x0934,
	proto_db_set_clothes_where	 = 0x0937,
	proto_db_sell_cloths		 = 0x097B,
	proto_db_repair_cloth_list	 = 0x09AE,
	proto_db_get_house_cloth_list= 0x08B4,
	proto_db_set_cloth_duration	 = 0x09AD,

	proto_db_del_skill			 = 0x0943,
	proto_db_buy_skill			 = 0x0947,
	proto_db_pet_buy_skill		 = 0x0948,
	proto_db_pet_del_skill		 = 0x0949,
	proto_db_get_pet_inhouse	 = 0x08B0,
	proto_db_update_pet_location = 0x09B1,
	proto_db_exchange_pet		 = 0x09CA,

	proto_db_get_task_detail	 = 0x08AB,
	proto_db_write_task_diary	 = 0x09AC,
	proto_db_get_task_all		 = 0x08C3,
	proto_db_get_task_db		 = 0x08C2,
	proto_db_set_task_db		 = 0x09C0,
	proto_db_task_reward		 = 0x09C4,
	proto_db_set_task_svr_buf	 = 0x09C5,

	proto_db_add_normal_item	 = 0x0970,
	proto_db_add_item_list		 = 0x0972,
	proto_db_buy_normal_item	 = 0x0973,
	proto_db_get_house_item_list = 0x08B3,
	proto_db_item_in_out		 = 0x09B2,
	proto_db_add_house_item		 = 0x09B7,
	proto_db_get_item_day		 = 0x080C,

	proto_db_update_sprite		 = 0x097C,
	proto_db_user_levelup		 = 0x097D,
	proto_db_pet_levelup		 = 0x097E,

	proto_db_set_skill_lv		 = 0x0993,
	proto_db_set_prof			 = 0x0998,
	proto_db_add_pet_all		 = 0x0999,

	proto_db_register_bird_map	 = 0x099D,
	proto_db_get_bird_maps		 = 0x089B,
	proto_db_set_map_info		 = 0x099E,
	proto_db_register_boss_map	 = 0x09A9,

	proto_db_get_handbook_info	 = 0x089F,
	proto_db_set_item_handbook	 = 0x09A0,
	proto_db_set_beast_handbook	 = 0x09A1,
	proto_db_use_item_hb_item	 = 0x09A2,
	proto_db_use_beast_hb_item	 = 0x09A3,

	proto_db_update_user_honor	 = 0x09A5,

	proto_db_update_pk_info		 = 0x09B5,

    proto_db_get_home_info       = 0x08B6,
    proto_db_update_home_flag	 = 0x09C7,
	/*! mail */
    proto_db_send_mail			 = 0x09B9,
    proto_db_get_mail_simple	 = 0x08BB,
    proto_db_get_mail_content	 = 0x08BD,
	proto_db_del_mail			 = 0x09BE,
	proto_db_check_session		 = 0xA024,
	proto_db_add_session		 = 0xA122,
	proto_db_add_user_login_info = 0xA125,
	proto_db_mail_set_flag		 = 0x09BF,

	proto_db_try_vip			= 0x9B01,
	proto_db_set_expbox			 = 0x09CD,
	proto_db_day_add_ssid_cnt	 = 0x09CF,
	proto_db_sub_ssid_cnt		 = 0x09D1,

	proto_db_set_cli_buff		 = 0x09D3,

	proto_db_rand_info_check	 = 0xD520,
	proto_db_rand_info_update	 = 0xD521,
	proto_db_reply_question		 = 0xD427,
	
	proto_db_add_invited		 = 0xC1A0,
	proto_db_get_invited		 = 0xC0A1,
	proto_db_mail_items_get		 = 0x09BA,
	proto_db_login_report		 = 0xF105,
	proto_db_get_monster_hb_range =0x083B,
	proto_db_post_msg			 = 0xF130,
	proto_db_chat_msg			 = 0xF133,
};

/**
  * @brief errno returned from dbproxy
  */
enum db_err_t {
	dberr_sys_error      = 1001,
	dberr_db_error       = 1002,
	dberr_net_error      = 1003,
	dberr_net_timeout	 = 1017,
	/*! no such user id */
	dberr_no_uid         = 1105,
	dberr_session_err	 = 4331,
};

/**
  * @brief send a TCP request to db proxy
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int  send_request_to_db(sprite_t* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);

/**
  * @brief send a UDP request to db proxy
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
//int  send_udp_request_to_db(const player_t* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);

/**
  * @brief init UDP socket for login report
  */
//int	init_login_report_udp_socket();

/**
  * @brief init UDP socket for talk report
  */
//int	init_talk_report_udp_socket();

/**
  * @brief send a UDP login report to db
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_udp_login_report_to_db(const sprite_t* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);

int send_udp_post_msg_to_db(const sprite_t* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);

int send_udp_chat_msg_to_db(const sprite_t* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);

/**
  * @brief send a UDP talk report to db
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
//int send_udp_talk_report_to_db(const player_t* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);

/**
  * @brief handle package return from dbproxy
  * @param dbpkg package from dbproxy
  * @param pkglen length of dbpkg
  */
void handle_db_return(db_proto_t* dbpkg, uint32_t pkglen);

/**
  * @brief init handles to handle protocol packages from dbproxy
  * @return true if all handles are inited successfully, false otherwise
  */
int init_db_proto_handles(int rstart);

#endif // PAIPAI_DBPROXY_HPP_

