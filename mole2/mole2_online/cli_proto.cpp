


#include <fcntl.h>
#include <sys/mman.h>
//#include <libtaomee++/utils/strings.hpp>

#include "cli_proto.h"
#include "cli_login.h"
#include "battle.h"
#include "sns.h"
#include "items.h"
#include "pet.h"
#include "skill.h"
#include "honor.h"
#include "home.h"
#include "event.h"
#include "box.h"
#include "mail.h"
#include "picsrv.h"
#include "task_new.h"
#include "npc.h"
#include "maze.h"
#include "code.h"
#include "beast.h"
#include "activity.h"
#include "viponly.h"

#include "mall.h"

#include "version.h"
//新的命令分发
#include <libtaomee++/proto/Ccmd_map.h>
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>
#include <libtaomee++/utils/strings.hpp>
#include "./proto/mole2_online.h" 
#include "./proto/mole2_online_enum.h" 



//-------------------------------------------------------
//函数定义
#define PROTO_FUNC_DEF(proto_name)\
    int proto_name( sprite_t *p , Cmessage* c_in ) ;
#include "./proto/mole2_online_func_def.h"

//-------------------------------------------------------------
//对应的结构体
#include "./proto/mole2_online.h"
//-------------------------------------------------------------
//命令绑定
typedef   int(*P_DEALFUN_T)( sprite_t *p, Cmessage* p_in );




//命令map
Ccmd_map< Ccmd< P_DEALFUN_T> >  g_cli_cmd_map;
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    {cmdid, new (c_in), md5_tag,bind_bitmap ,proto_name },

Ccmd< P_DEALFUN_T> g_cli_cmd_list[]={
#include "./proto/mole2_online_bind_for_cli_request.h"
};

void init_cli_handle_funs()
{
    g_cli_cmd_map.initlist(g_cli_cmd_list, sizeof(g_cli_cmd_list)/sizeof(g_cli_cmd_list[0]));
}


/**
  * @brief set client protocol handle
  */
#define SET_CLI_HANDLE(op_, func_, len_, cmp_, no_guest_) \
		do { \
			if (rstart == 0 && cli_handles[op_].func != 0) { \
				ERROR_RETURN(("duplicate cmd=%u", op_), -1); \
			} \
			cli_handles[op_].func = func_; \
			cli_handles[op_].len = len_; \
			cli_handles[op_].cmp_method = cmp_; \
			cli_handles[op_].no_guest = no_guest_; \
		} while (0)


int init_cli_proto_handles(int rstart)
{
	SET_CLI_HANDLE(proto_cli_login, login_cmd, 20, cmp_must_eq, 0);
	SET_CLI_HANDLE(proto_cli_add_session, add_session_cmd, 8, cmp_must_eq, 0);
	SET_CLI_HANDLE(proto_cli_map_users, get_map_users_cmd, 8, cmp_must_eq, 0);
	SET_CLI_HANDLE(proto_cli_get_user_info, get_user_info_cmd, 4, cmp_must_eq, 0);
	SET_CLI_HANDLE(proto_cli_get_user_detail, get_user_detail_cmd, 4, cmp_must_eq, 0);
	SET_CLI_HANDLE(proto_cli_set_user_nick, set_user_nick_cmd, USER_NICK_LEN, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_set_user_color, set_user_color_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_user_talk, talk_cmd, 10, cmp_must_ge, 0);
	SET_CLI_HANDLE(proto_cli_user_show, show_cmd, 8, cmp_must_eq, 0);
	SET_CLI_HANDLE(proto_cli_set_flag, set_flag_cmd, 8, cmp_must_eq, 0);
	SET_CLI_HANDLE(proto_cli_chk_users_online, users_online_cmd, 8, cmp_must_ge, 1);
	SET_CLI_HANDLE(proto_cli_user_cure, user_cure_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_user_set_position, set_position_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_chk_online_id, check_online_id_cmd, 0, cmp_must_eq, 0);
	SET_CLI_HANDLE(proto_cli_chk_str_valid, chk_str_valid_cmd, MAX_STR_LEN, cmp_must_eq, 0);
	SET_CLI_HANDLE(proto_cli_set_busy_state, set_busy_state_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_allocate_exp, allocate_exp_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_post_msg, post_msg_cmd, 68, cmp_must_ge, 1);
	SET_CLI_HANDLE(proto_cli_set_play_mode,set_play_mode_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_start_double_exp, start_double_exp_cmd, 0, cmp_must_eq, 1);

	SET_CLI_HANDLE(proto_cli_add_friend, add_friend_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_rsp_friend_add, reply_add_friend_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_del_friend, del_friend_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_bklist_add, add_blacklist_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_bklist_del, del_blacklist_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_bklist, get_blacklist_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_invited, get_invited_cmd, 0, cmp_must_eq, 1);

	SET_CLI_HANDLE(proto_cli_noti_relation_up, noti_relation_up_cmd, 24, cmp_must_eq, 1);

	SET_CLI_HANDLE(proto_cli_buy_cloth, buy_cloth_cmd, 12, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_pakt_cloth, get_pakt_clothes_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_del_cloth, del_cloth_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_body_clothes, get_body_clothes_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_buy_normal_item, buy_normal_item_cmd, 12, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_normal_item_list, get_normal_item_list_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_sell_items, sell_normal_item_cmd, 4, cmp_must_ge, 1);
	SET_CLI_HANDLE(proto_cli_del_normal_item, del_normal_item_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_del_house_item, del_house_item_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_use_medical_item, use_medical_item_cmd, 12, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_re_birth, user_re_birth_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_reset_attr, reset_attr_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_use_handbook_item, use_handbook_item_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_chk_teamate_enter_map, chk_teamate_enter_map_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_chk_team_item, check_team_item_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_repair_cloth, repair_cloth_cmd, 12, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_house_item_list, get_house_item_list_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_item_in_out, item_in_out_cmd, 12, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_cloth_in_out, cloth_in_out_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_composite_item, composite_item_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_exchange_gift_normal, exchange_gift_normal_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_exchange_gift_spec, exchange_gift_spec_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_exchange_gift_pet, exchange_gift_pet_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_composite_cloth, composite_cloth_cmd, 8, cmp_must_eq, 1);

    SET_CLI_HANDLE(proto_cli_add_item_feed_count, add_item_feed_count_cmd, 4, cmp_must_eq, 1);
    SET_CLI_HANDLE(proto_cli_get_sth_done, get_sth_done_for_item_cmd, 4, cmp_must_eq, 1);
    SET_CLI_HANDLE(proto_cli_set_sth_done, set_sth_done_for_item_cmd, 4, cmp_must_eq, 1);
	
	SET_CLI_HANDLE(proto_cli_get_monster_handbook,get_monster_handbook_info_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_set_monster_handbook,set_monster_handbook_info_cmd, 6, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_use_buff_item, use_buff_item_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_use_energy_item, use_energy_item_cmd, 4, cmp_must_eq, 1);

	SET_CLI_HANDLE(proto_cli_get_task_detail, get_task_detail_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_write_task_diary, write_task_diary_cmd, TASK_DIARY_BUF_LEN + 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_recv_task, recv_task_cmd, 8 + TASK_CLI_BUF_LEN, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_cancel_task, cancel_task_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_set_task_clibuf, set_task_clibuf_cmd, 8 + TASK_CLI_BUF_LEN, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_task_svrbuf, get_task_svrbuf_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_set_buff, set_cli_buff_cmd, 516, cmp_must_eq, 1);
	
	SET_CLI_HANDLE(proto_cli_challenge, challenge_battle_cmd, 12, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_challenge_npc, challenge_npc_battle_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_accept_challenge, accept_battle_cmd, 12, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_reject_challenge, reject_battle_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_load_battle, load_battle_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_load_ready, load_battle_result_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_round_info, round_info_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_battle_attack, battle_attack_cmd, 20, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_use_medical, battle_use_medical_cmd, 12, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_cancel_challenge, cancel_battle_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_use_beast_card, battle_use_beast_card_cmd, 12, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_battle_swap_pet, battle_swap_pet_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_del_pet, del_pet_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_apply_pk, apply_pk_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_cancel_pk_apply, cancel_pk_apply_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_last_kill_beastgrp, get_last_kill_beastgrp_cmd, 0, cmp_must_eq, 1);

	SET_CLI_HANDLE(proto_cli_btrtem_add_mbr, add_mbr_to_btr_team_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_btrtem_apply, apply_for_join_btr_team_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_btrtem_kick_mbr, kick_outof_btr_team_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_btrtem_mbr_leave, leave_btr_team_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_noti_tleader_action, notify_teamleader_action_cmd, 16, cmp_must_ge, 1);
	SET_CLI_HANDLE(proto_cli_animation_finish, animation_finish_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_change_team_leader, change_team_leader_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_register_for_team, register_for_team_cmd, 16, cmp_must_eq, 1);
	//SET_CLI_HANDLE(proto_cli_add_npc_to_team, add_npc_to_team_cmd, 4, cmp_must_eq, 1);
	//SET_CLI_HANDLE(proto_cli_get_map_grp, get_map_grp_cmd, 0, cmp_must_eq, 1);

	SET_CLI_HANDLE(proto_cli_set_pet_state, set_pet_state_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_set_pet_nick, set_pet_nick_cmd, 20, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_pet_info, get_pet_info_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_pet_list, get_pet_list_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_user_pet_list, get_user_pet_list_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_cure_pet, cure_pet_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_cure_pet_injury, cure_pet_injury_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_pet_follow, set_pet_follow_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_pet_inhouse, get_pet_inhouse_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_update_pet_location, update_pet_location_cmd, 16, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_exchange_pet, exchange_pet_cmd, 8, cmp_must_ge, 1);
	SET_CLI_HANDLE(proto_cli_get_all_petcnt, get_all_petcnt_cmd, 0, cmp_must_eq, 1);

	SET_CLI_HANDLE(proto_cli_buy_skill, buy_skill_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_pet_buy_skill, buy_pet_skill_cmd, 16, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_skill_list, get_skill_list_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_del_skill, del_skill_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_pet_del_skill, del_pet_skill_cmd, 12, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_pet_add_base_attr, add_pet_base_attr_cmd, 14, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_other_pet_info, get_other_pet_info_cmd, 8, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_unlock_pet_talent_skill, unlock_pet_talent_skill_cmd, 4, cmp_must_eq, 1);

	SET_CLI_HANDLE(proto_cli_check_rand_info, check_rand_info_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_vip_item, get_vip_item_cmd, 0, cmp_must_eq, 1);

	SET_CLI_HANDLE(proto_cli_register_bird_map, register_bird_map_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_check_team_bird_map, check_team_bird_map_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_bird_maps, get_bird_maps_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_register_boss_map, register_boss_map_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_boss_maps, get_boss_maps_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_check_team_boss_map, check_team_boss_map_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_map_state, get_map_state_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_set_map_state, set_map_state_cmd, 8, cmp_must_eq, 1);

	SET_CLI_HANDLE(proto_cli_get_handbook_info, get_handbook_info_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_g_book, get_g_book_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_g_book_xiaomee, get_g_book_xiaomee_cmd, 4, cmp_must_eq, 1);

	SET_CLI_HANDLE(proto_cli_open_box, open_box_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_box_active, get_box_active_cmd, 0, cmp_must_eq, 1);
//maze
	SET_CLI_HANDLE(proto_cli_get_maze_in_map, get_maze_in_map_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_shop_in_map, get_shop_in_map_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_maze_open_portal, open_maze_portal_cmd, 4, cmp_must_eq, 1);
/*! mail */
	SET_CLI_HANDLE(proto_cli_send_mail, send_mail_cmd, 76, cmp_must_ge, 1);
	SET_CLI_HANDLE(proto_cli_get_mail_simple, get_mail_simple_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_get_mail_content, get_mail_content_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_mail_set_flag, mail_set_flag_cmd, 8, cmp_must_ge, 1);

	SET_CLI_HANDLE(proto_cli_mail_sys_send, mail_sys_send_cmd, 4, cmp_must_ge, 1);
	SET_CLI_HANDLE(proto_cli_mail_items_get, mail_items_get_cmd, 4, cmp_must_eq, 1);
		
	SET_CLI_HANDLE(proto_cli_get_home_info, get_home_info_cmd, 4, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_pic_srv_ip_port, get_pic_srv_ip_port_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_update_home_flag, update_home_flag_cmd, 8, cmp_must_eq, 1);
	
	SET_CLI_HANDLE(proto_cli_get_water_cnt, get_water_cnt_cmd, 0, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_use_water, use_water_cmd, 4, cmp_must_eq, 1);

	SET_CLI_HANDLE(proto_cli_check_magic_code, check_magic_code_cmd, 32, cmp_must_eq, 1);
	SET_CLI_HANDLE(proto_cli_use_magic_code, use_magic_code_cmd, 36, cmp_must_ge, 1);

	SET_CLI_HANDLE(proto_cli_read_news, read_news_cmd, 4, cmp_must_eq, 1);

	init_cli_handle_funs();

	/*
	if(tm_load_dirty("./data/tm_dirty.dat") < 0){
		KERROR_LOG(0, "Failed to load drity word file!");
		return -1;
	}
	*/
	return 0;
}

void handle_cmd_busy_sprite()
{
	list_head_t *cur;
	list_head_t *next;
	
	list_for_each_safe(cur, next, &g_busy_sprite_list) {
		sprite_t *p = list_entry(cur, sprite_t, busy_sprite_list);
		//handle the cmd queue until blocked by slow db request
		while ( p->waitcmd == 0 ) { 
			//at least has a pending cmd, so no need to check emptiness
			cached_pkg_t* cmdbuf = (cached_pkg_t*)g_queue_pop_head(p->pending_cmd);
			KDEBUG_LOG(p->id, "BEGIN HANDLE ONE PENDING CMD\t[cmdid=%u, cmdlen=%u]", p->waitcmd, cmdbuf->len - 4);
			int err = dispatch(cmdbuf->pkg, p->fdsess, 0);
			g_slice_free1(cmdbuf->len, cmdbuf);
			if (!err) {
				if ( g_queue_is_empty(p->pending_cmd) ) {
					//remove the sprite from busy list
					list_del_init(&p->busy_sprite_list);
					break;
				}
			} else {
				KDEBUG_LOG(p->id, "CMD BUSY KICK USER OFFLINE");
				close_client_conn(p->fd);
				break;
			}
		}		
	}
}

void add_cmd_pending_head(sprite_t* p, void* data)
{
	protocol_t  pkg_head;
	protocol_t* pkg = (protocol_t*)data;

	pkg_head.len = ntohl(pkg->len);
	pkg_head.cmd = ntohs(pkg->cmd);
	pkg_head.id  = ntohl(pkg->id);
	pkg_head.ret = ntohl(pkg->ret);
	pkg_head.seq = ntohl(pkg->seq);

	KDEBUG_LOG(p->id, "CACHE PKG\t[len=%u cmd=%u waitcmd=%u]", pkg_head.len, pkg_head.cmd, p->waitcmd);
	// alloc space to hold the incoming package
	uint16_t len      = sizeof(cached_pkg_t) + pkg_head.len;
	cached_pkg_t* cbuf = (cached_pkg_t*)g_slice_alloc(len);
	cbuf->len = len;
	memcpy(cbuf->pkg, pkg, pkg_head.len);
	if ( g_queue_is_empty(p->pending_cmd) ) {
		//add to busy sprite list
		list_add_tail(&(p->busy_sprite_list), &g_busy_sprite_list);
	}
	// already in busy sprite list, then add to pending cmd queue
	g_queue_push_head(p->pending_cmd, cbuf);
	KDEBUG_LOG(p->id, "PENDING CMD\t[cmdid=%u, waitcmd=%u, cmdlen=%u]", pkg_head.cmd, p->waitcmd, pkg_head.len);
}





/**
  * @brief dispatches protocol packages from client
  * @param data package from client
  * @param fdsess fd session
  * @param first_tm true if we are processing a package from a client,
  *              false if from the pending-processing queue
  * @return value that was returned by the protocol handling function
  */
int dispatch(void* data, fdsession_t* fdsess, int first_tm)
{
	protocol_t  pkg_head;
	protocol_t* pkg = (protocol_t*)data;

	pkg_head.len = ntohl(pkg->len);
	pkg_head.cmd = ntohs(pkg->cmd);
	pkg_head.id  = ntohl(pkg->id);
	pkg_head.ret = ntohl(pkg->ret);
	pkg_head.seq = ntohl(pkg->seq);
	KDEBUG_LOG(pkg_head.id, "len=%u cmd=%u ret=%u %u ", pkg_head.len, pkg_head.cmd, pkg_head.ret, pkg_head.seq);
	//返回版本信息
	//KDEBUG_LOG(pkg_head.id, "xxxxxx pkg_head.cmd %u== proto_cli_version %u", pkg_head.cmd,proto_cli_version );
	if(pkg_head.cmd == proto_cli_version) {
		DEBUG_LOG("DO proto_cli_version ");
		//检查onlineid 是否一致
		if ((*(uint32_t*)(pkg+1))==get_server_id() ){
	   		const char * version_str=get_version_str( g_version_str) ;
       		return send_pkg_to_client(fdsess, version_str, 256);
		}else{
	   		const char * version_str=get_version_str("onlineid err" ) ;
       		return send_pkg_to_client(fdsess, version_str, 256);
		}
	}
	
	sprite_t* p = get_sprite_by_fd(fdsess->fd);
	
	// registered user login
	if ( ((pkg_head.cmd != proto_cli_login) && !p)//
			|| ((pkg_head.cmd == proto_cli_login) && p)
			|| (p && (p->id != pkg_head.id)) ) {
		KERROR_LOG(pkg_head.id, "pkg error: fd=%d cmd=%u p=%p %u", fdsess->fd, pkg_head.cmd, p, pkg_head.id);
		return -1;
	}
	//安全校验	
	if (first_tm && p){
		//if(pkg_head.cmd != proto_cli_login) {
			//uint32_t real_seq=0;
			//if(is_invaild_seqno(p, pkg_head.len, pkg_head.seq, pkg_head.cmd, 
						//pkg->body, pkg_head.len-sizeof(protocol_t),real_seq)){
				//ERROR_LOG("wrong seqno: uid=%u seq=%u %u ret=%u cmd=%u",
						//p->id, real_seq, pkg_head.seq, pkg_head.ret, pkg_head.cmd);
				//return -1;

			//}
		//}
		p->seq=pkg_head.seq;
	}
	// cache the incoming package if there is still a package under processing
	if (p && p->waitcmd != 0) {
		if (g_queue_get_length(p->pending_cmd) < 50) {
			KDEBUG_LOG(p->id, "CACHE PKG\t[len=%u cmd=%u waitcmd=%u]", pkg_head.len, pkg_head.cmd, p->waitcmd);
			// alloc space to hold the incoming package
			uint16_t len      = sizeof(cached_pkg_t) + pkg_head.len;
			cached_pkg_t* cbuf = (cached_pkg_t*)g_slice_alloc(len);
			cbuf->len = len;
			memcpy(cbuf->pkg, pkg, pkg_head.len);
			if ( g_queue_is_empty(p->pending_cmd) ) {
				//add to busy sprite list
				list_add_tail(&(p->busy_sprite_list), &g_busy_sprite_list);
			}
			// already in busy sprite list, then add to pending cmd queue
			g_queue_push_tail(p->pending_cmd, cbuf);
		KDEBUG_LOG(p->id, "PENDING CMD\t[cmdid=%u, waitcmd=%u, cmdlen=%u]", pkg_head.cmd, p->waitcmd, pkg_head.len);
			return 0;
		} else {
			WARN_LOG("wait for cmd=%u id=%u newcmd=%u", p->waitcmd, p->id, pkg_head.cmd);
			return send_to_self_error(p, p->waitcmd, cli_err_system_busy, 0);
		}
	}

	/*
	//检查序列号
	if (p && pkg_head.cmd != proto_cli_login && pkg_head.cmd != proto_cli_noti_relation_up) {
		int32_t seq = p->seq - p->seq / 7 + 140 + pkg_head.len % 26 + pkg_head.cmd % 21;
		seq = seq < 0 ? 1 : seq;
		//KERROR_LOG(pkg_head.id, "seq info\[cmd=%d need=%d my=%d get=%d len=%d]", pkg_head.cmd, seq, p->seq, pkg_head.seq, pkg_head.len);
		if (seq != pkg_head.seq) {
			KERROR_LOG(pkg_head.id, "seq not match\[%u %u %u]", pkg_head.cmd, p->seq, pkg_head.seq);
			return -1;
		}
		p->seq = seq;
	}
	*/
	
	// login protocol
	sprite_t player;
	memset(&player, 0, sizeof(sprite_t));
	if (pkg_head.cmd == proto_cli_login) {
		sprite_t* old = get_sprite(pkg_head.id);
		// multiple login
		if (old) {
			KDEBUG_LOG(old->id, "relogin invalid[%u]", pkg_head.id);
			return -1;
		}
		p          = &player;
		p->id      = pkg_head.id;
		p->fdsess  = fdsess;
		p->fd 		= fdsess->fd;
		p->waitcmd = 0;
		p->seq		=  pkg_head.seq;
	}

    Ccmd< P_DEALFUN_T> * p_cmd_item =g_cli_cmd_map.getitem(pkg_head.cmd );
    if (p_cmd_item ) {
	 	if ( g_log_send_buf_hex_flag==1)
		{
	        char outbuf[13000];
	        bin2hex(outbuf,(char*)data,pkg_head.len  ,2000);
	        DEBUG_LOG("CI-new[%u][%u][%u][%u][%u][%s]", pkg_head.len, pkg_head.cmd,pkg_head.id, pkg_head.ret, pkg_head.seq,outbuf  );
	    }



		//DEBUG_LOG("cli dispatch cmd %u", pkg_head.cmd);
		//还原对象
		p_cmd_item->proto_pri_msg->init( );

		byte_array_t in_ba ( ((char*)data)+sizeof(pkg_head ),
			pkg_head.len -sizeof(pkg_head));
		in_ba.set_is_bigendian(true);
		//失败
		if (!p_cmd_item->proto_pri_msg->read_from_buf(in_ba)) {
			DEBUG_LOG("client recover object fail");
			return -1;
		}

		//客户端多上传报文
		if (!in_ba.is_end()) {
			DEBUG_LOG("client !in_ba.is_end()");
			return  -1;
		}

		p->waitcmd = pkg_head.cmd;
	    return p_cmd_item->func(p, p_cmd_item->proto_pri_msg);
	

    }else{ //处理原有的命令
 		if ( g_log_send_buf_hex_flag==1)
		{
	        char outbuf[13000];
	        bin2hex(outbuf,(char*)data,pkg_head.len  ,2000);
	        DEBUG_LOG("CI-old[%u][%u][%u][%u][%u][%s]", pkg_head.len, pkg_head.cmd,pkg_head.id, pkg_head.ret, pkg_head.seq,outbuf  );
	    }


		if ((pkg_head.cmd <= proto_cli_cmd_start) || (pkg_head.cmd >= proto_cli_cmd_end) || (!cli_handles[pkg_head.cmd].func)) {
			KERROR_LOG(pkg_head.id, "invalid cmd=%u from fd=%d", pkg_head.cmd, fdsess->fd);
			return -1;
		}

		if (cli_handles[pkg_head.cmd].no_guest && IS_GUEST_ID(p->id)) {
			KERROR_LOG(p->id, "guest not allowed");
			return -1;
		}
		
	  

		uint32_t bodylen = pkg_head.len - sizeof(protocol_t);
		if ( ((cli_handles[pkg_head.cmd].cmp_method == cmp_must_eq) && (bodylen != cli_handles[pkg_head.cmd].len))
				|| ((cli_handles[pkg_head.cmd].cmp_method == cmp_must_ge) && (bodylen < cli_handles[pkg_head.cmd].len)) ) {
			KERROR_LOG(pkg_head.id, "invalid package cmd=%u len=%u needlen--=%u cmpmethod=%d",
						pkg_head.cmd, bodylen, cli_handles[pkg_head.cmd].len, cli_handles[pkg_head.cmd].cmp_method);
			return -1;
		}
		p->waitcmd = pkg_head.cmd;

		return cli_handles[pkg_head.cmd].func(p, pkg->body, bodylen);


	}


}

/**
  * @brief send result of protocol to client
  * @param p  the receive user
  * @param buffer package to client
  * @param len buffer len
  * @param completed mean this protocol completly processed,
  * @return 0 succ 1 failed
  */
int send_to_self(sprite_t *p, uint8_t *buffer, int len, int completed)
{	
#ifdef __cplusplus
		protocol_t* ppr = reinterpret_cast<protocol_t*>(buffer);
#else
		protocol_t* ppr = (protocol_t*)buffer;
#endif

	ppr->id = htonl (p->id);
    if (g_log_send_buf_hex_flag==1)
	{
        char outbuf[13000];
        bin2hex(outbuf,(char*)buffer,len,2000);
        DEBUG_LOG("CO[%u][%u][%u][%u][%s]",
			 ntohl(((protocol_t*)ppr)->len),  
				ntohs(((protocol_t*)ppr)->cmd),  
				ntohl(((protocol_t*)ppr)->id),  
				ntohl(((protocol_t*)ppr)->ret),  
				 outbuf 
				);
    }
	
			

    if (send_pkg_to_client(p->fdsess, buffer, len) == -1) {
		ERROR_RETURN(("failed to send pkg to client: uid=%u cmd=%u", p->id, ntohs(ppr->cmd)), -1);
	}

	if (completed && p->waitcmd != ntohs(ppr->cmd)) {
		WARN_LOG("p_waitcmd not equal to proto_cmd\t[%u %u %u]", p->id, p->waitcmd, ntohs(ppr->cmd));
	}
	if (completed) {
		p->waitcmd = 0;
		p->sess_len = 0;
	}

	//KDEBUG_LOG(p->id,"send_to_self cmd:%u",ntohs(ppr->cmd));
	p->last_sent_tm = get_now_tv()->tv_sec;
	return 0;	
}

//send 
int send_msg_to_self(sprite_t *p, int cmd, Cmessage *c_in, int completed)
{	
	char pkg_head[sizeof(protocol_t)];
	protocol_t* ppr = reinterpret_cast<protocol_t*>(pkg_head );

	ppr->id = htonl (p->id);
	ppr->len = htonl(sizeof(protocol_t));
	ppr->cmd = htons(cmd);
	ppr->ret = 0;

    if (send_msg_to_client_bigendian(p->fdsess, pkg_head,c_in ) == -1) {
		ERROR_RETURN(("failed to send pkg to client: uid=%u cmd=%u", p->id, ntohs(ppr->cmd)), -1);
	}

	if (completed && p->waitcmd != ntohs(ppr->cmd)) {
		WARN_LOG("p_waitcmd not equal to proto_cmd\t[%u %u %u]", p->id, p->waitcmd, ntohs(ppr->cmd));
	}
	if (completed) {
		p->waitcmd = 0;
		p->sess_len = 0;
	}

	p->last_sent_tm = get_now_tv()->tv_sec;

	return 0;	
}


/**
  * @brief send error result of protocol to client
  * @param p  the receive user
  * @param cmd the failed protocol
  * @param err the error info
  * @param completed mean this protocol completly processed,
  * @return 0 succ 1 failed
  */
int send_to_self_error(sprite_t* p, int cmd, int err, int completed)
{
	protocol_t pkg;

	pkg.cmd = htons(cmd);
	pkg.ret = htonl(err);
	pkg.len = htonl(sizeof(pkg));

	return send_to_self(p, (uint8_t *)&pkg, sizeof(pkg), completed);
}


