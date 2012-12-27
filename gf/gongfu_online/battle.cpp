#include <cstdio>
using namespace std;

#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>

using namespace taomee;

extern "C" {
#include <glib.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/utils.h>
#include <async_serv/service.h>
#include <async_serv/net_if.h>
}

#include <kf/memstream.hpp>

#include "common_op.hpp"
#include "message.hpp"
#include "player.hpp"
#include "task.hpp"
#include "task_logic.hpp"
#include "item.hpp"

#include "battle.hpp"
#include "mail.hpp"
#include "battle_switch.hpp"
#include "stage_config.hpp"
#include "global_data.hpp"
#include "switch.hpp"
#include "achievement.hpp"
#include "contest.hpp"
#include "fight_team.hpp"
#include "stat_log.hpp"
#include "player_attribute.hpp"
#include "master_prentice.hpp"
#include "chat.hpp"
#include "rank_top.hpp"
#include "other_active.hpp"
#include "temporary_team.hpp"

/**
  * @brief battle invitation response
  */
enum btl_inv_rsp_t {
	btl_inv_refuse				= 0,
	btl_inv_accept				= 1,
};

//---------------------------------------------------------

/**
  * @brief struct to hold ip and port of a battle server
  */
/*struct battle_server_t {
	char		svr_name[16];
	int16_t		id;
};*/

/**
  * @brief data type for handlers that handle protocol packages from battle server
  */
//typedef int (*btlsvr_hdlr_t)(player_t* p, btl_proto_t* pkg);

/**
  * @brief data type for handles that handle protocol packages from battle server
  */
/*struct btlsvr_handle_t {
	btlsvr_handle_t(btlsvr_hdlr_t h = 0, uint32_t l = 0, uint8_t cmp = 0)
		{ hdlr = h; len = l; cmp_method = cmp; }

	btlsvr_hdlr_t		hdlr;
	uint32_t			len;
	uint8_t				cmp_method;
};*/

/*! save handles for handling protocol packages from battle server */
//static btlsvr_handle_t btl_handles[10000];
/*! for packing protocol data and send to battle server */
//uint8_t			btlpkgbuf[btlproto_max_len];
/*! hold ip address and port of each battle servers */
//battle_server_t	battle_svrs[max_battle_svr_num];
/*! hold number of battle servers */
//int				battle_svr_cnt;
/*! hold fds to battle servers */
//int				battle_fds[max_battle_svr_num] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
//												   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
/*! hold stage info which limit for entering */
//stage_activity_limit_t stage_activety_limit[1000];

/*! hold specialstages*/
//std::map<uint32_t, specialstages_t*> special_stages_map_;

/*! Footprinting pvp battle */
//pvp_pos footprint[MAX_ARENA_NUM] = {{0},{0},{0}};
//---------------------------------------------------------------

static int  init_battle(player_t* invitor, uint32_t battle_mode, uint32_t stage_id = 0, uint32_t difficulty = 1, uint32_t enter_type = 0);
inline void start_battle(player_t* p);
static void leave_battle(player_t* p, uint8_t reason = 0);
static int pack_btl_team_info(uint8_t* buf, btl_team_attr_t& team_attr);
static int pack_initiate_battle_info(uint8_t* buf, player_t* p, uint32_t stage, uint32_t difficulty, uint32_t battle_mode, uint32_t btl_again);

//---------------------------------------------------------------

static int btlsvr_initiate_battle(player_t* p, uint32_t stage, uint32_t difficulty, uint32_t cmd, uint32_t battle_mode, uint32_t btl_again = 0);
inline int btlsvr_player_ready(player_t* p);
inline int btlsvr_sync_process(player_t* p, uint32_t process);
inline int btlsvr_player_move(player_t* p, uint32_t old_x, uint32_t old_y, uint32_t old_z,  uint32_t x, uint32_t y, uint32_t z, uint8_t dir, uint8_t mv_type, uint32_t client_tm, uint32_t client_msec);
inline int btlsvr_player_rsp_lag(player_t* p, uint32_t seq);
inline int btlsvr_player_attack(player_t* p, uint16_t cmd, uint32_t skill_id, uint32_t x, uint32_t y, uint32_t z, uint32_t client_tm, uint32_t client_msec);
inline int btlsvr_player_enter_map(player_t* p, uint32_t mid);
//---------------------------------------------------------------

void clear_btl_buff(player_t* p)
{
	BufSkillMap::iterator it = p->buf_skill_map->begin();
	for (; it != p->buf_skill_map->end(); ) {
		if (it->second.buf_type < max_nor_buff_type) {
			BufSkillMap::iterator ittmp = it;
			++it;
			p->buf_skill_map->erase(ittmp);
		} else {
			++it;
		}
	}

}

void do_btl_restart(player_t* p, uint32_t stage_id, uint32_t difficulty)
{
	int idx = sizeof(btl_proto_t);
	pack_h(btlpkgbuf, stage_id, idx);
	pack_h(btlpkgbuf, difficulty, idx);
	if (is_time_limit_2(p)) {
		p->half_exp = 1;
	} else {
		p->half_exp = 0;
	}
	pack_h(btlpkgbuf, p->half_exp, idx);
	idx += pack_task_to_btl(p, btlpkgbuf + idx, p->battle_grp->stage_id, difficulty);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_player_btl_restart);

	KDEBUG_LOG(p->id, "PVE RESTART\t[uid=%u %u]", p->id, p->battle_grp->stage_id);
	send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);

	if (p->battle_grp) {
		p->battle_grp->id = 0;
	}
}


/**
 * @brief unpack the player's buf skill info from battle
 * @param p the player
 * @param count the task number
 * @param body the info of task
 */
void unpack_buf_skill_info(player_t* p, uint32_t count, uint8_t* body)
{
	player_buf_skill_rsp_t* p_buf = reinterpret_cast<player_buf_skill_rsp_t*>(body);
//	p->buf_skill_map->clear();
	for ( uint32_t i = 0; i < count; i++ ) {
		if (!(p_buf->buf_duration)) {
			p->buf_skill_map->erase(p_buf->buf_type);
			TRACE_LOG("ERASE BUF: %u %u", p_buf->buf_type, p_buf->buf_duration);
		} else {
			BufSkillMap::iterator it = p->buf_skill_map->find(p_buf->buf_type);
			if (it != p->buf_skill_map->end()) {
				it->second.buf_duration = p_buf->buf_duration;
				it->second.per_trim = p_buf->per_trim;
				KDEBUG_LOG(p->id, "UPDATE BUF: app[%u] %u %u %u", 
                    p->app_mon, p_buf->buf_type, p_buf->buf_duration, p_buf->per_trim);
			}
		}
		p_buf ++;
	}
}

/**
 * @brief use plugin buy or sell item
 */
inline void do_stat_log_pvp_times(uint32_t ip, uint32_t cnt)
{
    uint32_t buf[2] = {0};
    buf[0] = ip;
    buf[1] = cnt;
    msglog(statistic_logfile, stat_log_pvp_times, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log cmdid:%x userid:%u", stat_log_pvp_times, ip);
}

/**
 * @brief use plugin buy or sell item
 */
inline void do_stat_log_pvp_fag(uint32_t ip, uint32_t fag)
{
    uint32_t buf[2] = {0};
    buf[0] = ip;
    buf[1] = fag;
    msglog(statistic_logfile, stat_log_pvp_fag, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log cmdid:%x userid:%u", stat_log_pvp_fag, ip);
}

int date2timestamp(const char* fromstr, const char* fmt, time_t &totime)
{
	struct tm tmp_time;
	strptime(fromstr, fmt, &tmp_time);
	totime = mktime(&tmp_time);
	return totime;
}

static int operating_activity_end(player_t* p, stage_data** pdata_out) 
{
	uint32_t stage_id = 0;
	uint32_t is_free = 0;
	uint32_t stage_diff = 0;
	//int idx = 0;
	//unpack_h(p->session, stage_id, idx);
	//unpack_h(p->session, is_free, idx);
	//unpack_h(p->session, stage_diff, idx);
	if (!(p->battle_grp)) {
		return 0;
	}
	stage_id = p->battle_grp->stage_id;
	is_free = p->battle_grp->enter_type ? false : true;
	stage_diff = p->battle_grp->difficulty;
	p->sesslen = 0;
	stage_data_mgr* mgr = get_stage_data_mgr();
	if( (stage_id >= 0 && stage_id <= mgr->get_max_stage_id() ) )
	{
		stage_data* pdata = mgr->get_data_by_stage_id(stage_id, stage_diff);
		if(pdata != NULL)
		{
			if (stage_diff && pdata->stage_diff && stage_diff != pdata->stage_diff) {
				return 0;
			}
			if (is_free) {
				if (pdata->free_daily_id) {
					int i = pdata->free_daily_id - 1;
					if ((i < 0) || (static_cast<uint32_t>(i) >= max_restr_count) || 
						(g_all_restr[i].id != pdata->free_daily_id)) {
						return false;
					}
					restriction_t* p_restr = &g_all_restr[i];
					db_add_action_count(0, p->id, p->role_tm, p_restr->id, p_restr->restr_flag, 
						p_restr->toplimit, p_restr->userflg);
					p->daily_restriction_count_list[i] ++;
				}
			} else {
				if (pdata->unlimited_item.count) {
					*pdata_out = pdata;
					for (uint32_t i = 0; i < pdata->unlimited_item.count; i++) {
						if (pdata->unlimited_item.itemarr[i].itemid == 4) {
						//reduce fumo points
							if (p->fumo_points_total < pdata->unlimited_item.itemarr[i].count) {
								return false;
							}
							p->fumo_points_total -= pdata->unlimited_item.itemarr[i].count;
							do_stat_log_fumo_add_reduce((uint32_t)0, pdata->unlimited_item.itemarr[i].count);
							db_use_item_ex(0, p->id, p->role_tm, pdata->unlimited_item.itemarr[i].itemid
								, pdata->unlimited_item.itemarr[i].count, false);
						} else {
							p->my_packs->del_item(p, pdata->unlimited_item.itemarr[i].itemid, 
								pdata->unlimited_item.itemarr[i].count, channel_string_other);

							const GfItem* itm = items->get_item(pdata->unlimited_item.itemarr[i].itemid);
							if ( !itm ) {
								return false;
							}
							bool is_shop_item = itm->is_item_shop_sale();
							if (is_storage_relive_item(pdata->unlimited_item.itemarr[i].itemid)) {
								is_shop_item = true;
							}
							db_use_item_ex(0, p->id, p->role_tm, pdata->unlimited_item.itemarr[i].itemid
								, pdata->unlimited_item.itemarr[i].count, is_shop_item);
						}
					}
				}
			}
		}	
	}
	return 0;
}

/**
  * @brief tmp , just for operating activity
  */
static int operating_activity_limit(uint32_t stage, player_t* p, bool is_free = true, uint32_t stage_diff = 0) 
{
	p->sesslen = 0;
	pack_h(p->session, stage, p->sesslen);
	pack_h(p->session, is_free ? 1 : 0, p->sesslen);
	pack_h(p->session, stage_diff, p->sesslen);
	uint32_t ret_err = check_player_enter_stage( get_stage_data_mgr(), p, stage, stage_diff, is_free);	
	if(ret_err)
	{
		return ret_err;
	}
	
	return 0;
}

static int check_activity_time(uint32_t stage)
{
	return check_player_enter_stage_time(get_stage_data_mgr(),  stage);
}

void leave_room(player_t* p)
{
	KDEBUG_LOG(p->id, "L R :%u %u", p->id, p->battle_grp ? p->battle_grp->id : 0);
	send_entermap_rsp(p, 0);
	p->invisible  = 0;
	free_battle_grp(p->battle_grp, p);
	p->battle_grp = 0;
}

int enter_room(player_t* p, int fd_idx, battle_mode_t mode, uint8_t pos)
{
	if (init_battle(p, mode, 0) == -1) {
		return -1;
	}
	if (fd_idx != -1) {
		p->battle_grp->fd_idx = fd_idx;
	}

    p->battle_grp->foot_pos = pos;
	p->battle_grp->btl_type = battle_type_multi;
	return 0;
}
//---------------------------------------------------------------

#if 0
/**
  * @brief initiate a battle
  */
int initiate_battle_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	userid_t invitee_id;
	unpack(body, invitee_id, i);

	if (p->id == invitee_id) {
		return send_header_to_player(p, p->waitcmd, cli_err_btl_same_id, 1);
	}
	// TODO: verify battle mode

	player_t* invitee = get_player(invitee_id);
	if (invitee) {
		try {
			// TODO: what if p->btl was already not 0. make it a init_battle
			// use Memory Pool later
		} catch (...) {
			KDEBUG_LOG(p->id, "FAIL TO CREATE BATTLE\t[uid=%u]", p->id);
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}

		/* respond to invitor */
		send_header_to_player(p, p->waitcmd, 0, 1);

		KDEBUG_LOG(p->id, "INIT BATTLE\t[uid=%u invitee=%u]", p->id, invitee_id);

		// TODO:
		/* notify the invitee */
		i = sizeof(cli_proto_t);
		pack(pkgbuf, p->nick, sizeof(p->nick), i);
		init_cli_proto_head(pkgbuf, p, cli_proto_btl_invitation, i);
		return send_to_player(invitee, pkgbuf, i, 0);
	}

	return send_header_to_player(p, p->waitcmd, cli_err_user_offline, 1);
}

/**
 * @brief respond a battle invitation
 */
int rsp_battle_invit_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	// TODO:
	userid_t invitor_id;
	uint32_t accept;

	int i = 0;
	unpack(body, invitor_id, i);
	unpack(body, accept, i);

	if ((accept != btl_inv_refuse) && (accept != btl_inv_accept)) {
		ERROR_LOG("invalid reply: uid=%u accept=%u", p->id, accept);
		return -1;
	}

	player_t* invitor = get_player(invitor_id);
//	if ( (invitor == 0) || (invitor->btl == 0)
//			|| (invitor->btl->invitee() != p->id)
//			|| invitor->btl->is_battle_started() ) {
//		return send_header_to_player(p, p->waitcmd, cli_err_btl_not_exist, 1);
//	}

//	Battle* btl = invitor->btl;
//	if (accept == btl_inv_refuse) {
//		delete btl;
//	} else {
		// TODO:
//		btl->add_player(p);
//	}

	send_header_to_player(p, p->waitcmd, 0, 1);

	// send invitation response to the invitor
	i = sizeof(cli_proto_t);
	pack(pkgbuf, p->nick, sizeof(p->nick), i);
	pack(pkgbuf, accept, i);
	init_cli_proto_head(pkgbuf, p, cli_proto_battle_inv_rsp, i);
	send_to_player(invitor, pkgbuf, i, 0);

	if (accept == btl_inv_accept) {
//		btl->start_battle();
	}

	return 0;
}
#endif

/**
 * @ brief for pvp footprint
 */
int is_pvp_footprint_free(player_t* p, int pos, uint32_t foot_pos)
{
    if (foot_pos % 2 == 0) {
        return !(footprint[pos].right_pos);
    } else {
       return !(footprint[pos].left_pos);
    }
}

/**
 * @ brief for pvp footprint
 */
int enter_pvp_footprint(player_t* p, int pos, uint32_t foot_pos)
{
    if (foot_pos % 2 == 0) {
        if (footprint[pos].right_pos == 1) {
            return -1;
        } else {
            footprint[pos].right_pos = 1;
            footprint[pos].right_id = p->id;
        }
    } else {
        if (footprint[pos].left_pos == 1) {
            return -1;
        } else {
            footprint[pos].left_pos = 1;
            footprint[pos].left_id = p->id;
        }
    }
    //p->invisible = 1;
    //send_leave_map_rsp(p, 0);
    //leave_map(p);
    
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, p->id, idx);
    pack(pkgbuf, foot_pos, idx);
    init_cli_proto_head_full(pkgbuf, p->id, p->seqno, cli_proto_stand_pvp_point, idx, 0);
    send_to_map(p, pkgbuf, idx, 0);

	KDEBUG_LOG(p->id, "ENTER PVP FOOT\t[%u pos=%u foot_pos%u]", p->id, pos, foot_pos);
    return 0;
}

/**
 * @ brief for pvp footprint
 */
int leave_pvp_footprint(player_t* p, bool come_back)
{
    if (p->battle_grp != NULL && p->battle_grp->foot_pos) {
        int pos = p->battle_grp->foot_pos;
        if (pos > 0 && pos < 7) {
            int arena = (pos - 1) / 2;
            if (footprint[arena].right_pos + footprint[arena].left_pos == 1) {
                footprint[arena].room_id = 0;
            }
            if (pos % 2 == 0) {
                footprint[arena].right_pos = 0;
                footprint[arena].right_id = 0;
            } else {
                footprint[arena].left_pos = 0;
                footprint[arena].left_id = 0;
            }
			KDEBUG_LOG(p->id, "LEAVE PVP FOOT\t[%u pos=%u foot_pos%u]", p->id, pos, arena);
			if (come_back)
				send_leave_pvp_point_rsp(p);
			p->battle_grp->foot_pos = 0;
        }
        //p->invisible = 0;
        //enter_map(p, 30, 600, 700);
    }
    return 0;
}
/**
 * @brief Broadcast user leave pvp point
 */
int send_leave_pvp_point_rsp(player_t* p)
{
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, p->id, idx);
    pack(pkgbuf, p->xpos, idx);
    pack(pkgbuf, p->ypos, idx);
    init_cli_proto_head_full(pkgbuf, p->id, p->seqno, cli_proto_leave_pvp_point, idx, 0);
    send_to_map(p, pkgbuf, idx, 0);
    return 0;
}
/**
 * @brief cancel a battle
 */
int cancel_battle_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	battle_grp_t* grp = p->battle_grp;

	if (grp == 0) {
		return send_header_to_player(p, p->waitcmd, 0, 1);
	}

//	ERROR_LOG("PLyaer %u  CANCEL BTL %u", p->id, p->p_banner_info->cur_banner);


	if (is_battle_started(grp)) {
		p->waitcmd = 0;
//		return 0;
//		return send_header_to_player(p, p->waitcmd, cli_err_cannot_cancel_battle, 1);
	}


	//free footpoint
    KDEBUG_LOG(p->id, "CLEAR FOOTPRINT\t [%u]", p->battle_grp->foot_pos);
	KDEBUG_LOG(p->id, "CNCL BTL\t[uid=%u mod=%u]", p->id, grp->battle_mode);

	btlsvr_cancel_battle(p);

	// free memory
	if (p->battle_grp) {
		free_battle_grp(p->battle_grp, p);
		p->battle_grp = 0;
	}

	
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

int player_invite_pvp(player_t* p, uint8_t btl_type, uint32_t room_id, uint32_t pvp_lv = pvp_lv_0)
{
    if (btl_type == pvp_btl_type_invite && room_id == 0) { 
        if (enter_room(p, -1, btl_mode_lv_matching) != 0) {
			return send_header_to_player(p, p->waitcmd, 0, 1);
		}
	} else { //the player be invited logic
		KDEBUG_LOG(p->id, "Player be Invited to Btl Room %u PVP_lv %u", room_id, pvp_lv);
        uint32_t btl_id = (room_id & 0xffff0000) >> 16;
        int fd_idx = get_btl_fd_idx_by_server_id(btl_id);
        if (!is_fd_idx_valid(fd_idx)) {
            return send_header_to_player(p, p->waitcmd, cli_err_battle_pvp_room_not_exist, 1);
        } else {
            if (enter_room(p, fd_idx, btl_mode_lv_matching) != 0) {
                return send_header_to_player(p, p->waitcmd, cli_err_battle_pvp_room_not_exist, 1);
            }
        }
    }  

	p->battle_grp->pvp_btl_type = pvp_lv;

    int idx = sizeof(btl_proto_t);
	//room_id = 0xffff;
	pack_h(btlpkgbuf, btl_type, idx);
	pack_h(btlpkgbuf, room_id, idx);
	idx += pack_initiate_battle_info(btlpkgbuf + idx, p, pvp_lv, 1, btl_mode_lv_matching, false);

	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_lv_matching_battle);

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

/**
  * @brief Attend a random battle, to battle with a randomly chosen player team
  */
int tmp_team_pvp_start_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t member_id = 0, member_tm = 0;
	
	unpack(body, member_id, idx);
	unpack(body, member_tm, idx);
	
	KDEBUG_LOG(p->id, "TEMP TEAM BTL INVITE\t[%u %u]", member_id, member_tm);
	return temp_team_pvp_match(p);
}



/**
  * @brief Attend a random battle, to battle with a randomly chosen player
  */
int lv_matching_btl_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t btl_again = 0; 
	uint32_t pvp_lv = 0, room_id = 0;
    uint8_t btl_type = 0;
	
	unpack(body, btl_again, idx);
	unpack(body, pvp_lv, idx);
	unpack(body, btl_type, idx);
	unpack(body, room_id, idx);
	
	//pvp_lv = pvp_eve_of_16_fight_2;

	if (pvp_lv == pvp_16_contest) {
		ERROR_LOG("GROUP LV MATCHING %u", p->id);
	}

	if (p->offline_data && p->offline_data->player_start_tm)
	{
		return -1;
	}

	uint32_t cur_map_id = get_mapid_low32(p->cur_map->id);
	if (cur_map_id >= 43 && cur_map_id <= 52) {
		p->waitcmd = 0;
		return 0;
	}


	if (p->trade_grp) {
		return send_header_to_player(p, p->waitcmd, cli_err_in_trade_svr, 1);
	}
	if (p->battle_grp) {
		return send_header_to_player(p, p->waitcmd, cli_err_in_battle_svr, 1);
	}
	
	if ( btl_type == 0 && (pvp_lv >= pvp_lv_max || pvp_lv < pvp_lv_1)) {
		ERROR_LOG("pvp lv err :%u %u %u ", p->id, btl_type, pvp_lv);
		return -1;
	}


	if (pvp_lv == pvp_ghost_game) {
		uint32_t ret_err = operating_activity_limit(979, p, true);
		if (ret_err) {
			return send_header_to_player(p, p->waitcmd, ret_err, 1);
		}
        
	}


	if ( btl_type == 0 && (p->lv < 10 || (p->lv < 40 && pvp_lv == 2))) {
		return send_header_to_player(p, p->waitcmd, cli_err_less_lv, 1);
	}
	
	if (is_btl_time_limited(p) && pvp_lv != pvp_team_contest) {
		return send_header_to_player(p, p->waitcmd, cli_err_limit_time_btl, 1);
	}

	if (pvp_lv == pvp_monster_game) { // 比武大会淘汰赛
		uint32_t ret_err = operating_activity_limit(939, p, true);
		if (ret_err) {
			return send_header_to_player(p, p->waitcmd, ret_err, 1);
		}
    }

	if (pvp_lv == pvp_contest_advance || pvp_lv == pvp_contest_final) { // 排名争霸赛
		uint32_t ret_err = operating_activity_limit(941, p, true);
		if (ret_err) {
			return send_header_to_player(p, p->waitcmd, ret_err, 1);
		}

	}

	if (pvp_lv == pvp_team_score && get_swap_action_times(p, 1419) >= pvp_team_score_limit_cnt) {
		return send_header_to_player(p, p->waitcmd, cli_err_limit_time_btl, 1);
	}


	if (pvp_lv >= pvp_eve_of_16_fight_1 && pvp_lv <= pvp_eve_of_16_fight_4) {
		uint32_t god_ret = is_in_active_time_section(13);
		if (god_ret) {
		    return send_header_to_player(p, p->waitcmd, god_ret, 1);
		}
	}

	if (pvp_lv == pvp_eve_of_16_fight_1) {
		if (p->exploit < 400) {
			return send_header_to_player(p, p->waitcmd, cli_err_no_enough_exploit, 1);
		}
	} else if (pvp_lv == pvp_eve_of_16_fight_2) {
		if (p->exploit < 600) {
			return send_header_to_player(p, p->waitcmd, cli_err_no_enough_exploit, 1);
		}
	} else if (pvp_lv == pvp_eve_of_16_fight_3) {
		if (p->exploit < 800) {
			return send_header_to_player(p, p->waitcmd, cli_err_no_enough_exploit, 1);
		}
	} else if (pvp_lv == pvp_eve_of_16_fight_4) {
		if (p->exploit < 1000) {
			return send_header_to_player(p, p->waitcmd, cli_err_no_enough_exploit, 1);
		}
	}	
	if (!btl_again) {
		p->cur_continue_win = 0;
	}
	
	if (!btl_again && !btl_type) { //|| p->cur_continue_win == 0) {
		if (pvp_lv == pvp_lv_1 || pvp_lv == pvp_summon_mode) {
			if (p->coins < 200) {
				return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);
			}
		} else if (pvp_lv == pvp_lv_2 || pvp_lv == pvp_summon_mode_2) {
			if (p->coins < 300) {
				return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);
			}
		} else if (pvp_lv == pvp_monster_game) {
            uint32_t total_times = get_taotai_can_join_times(p);
            //if (total_times <= p->taotai_info->do_times) {
            if (total_times <= get_swap_action_times(p, 1068)) {
                //KDEBUG_LOG(p->id, "PVP GAME no qualification\t[%u %u]", total_times, p->taotai_info->do_times);
                TRACE_LOG("PVP GAME no qualification [%u] [%u %u]", p->id, total_times, p->taotai_info->do_times);
                return send_header_to_player(p, p->waitcmd, cli_err_cannot_take_in_taotai_game, 1);
            }
		} else if (pvp_lv == pvp_contest_final) {
            if (!get_final_can_join_times(p)) {
                return send_header_to_player(p, p->waitcmd, cli_err_cannot_take_in_final_game, 1);
            } 
        }
	}
	
	KDEBUG_LOG(p->id, "PVP ACK SW\t[%u %u %u %u %u]", p->id, btl_again, pvp_lv, btl_type, room_id);
    if (btl_type == 0) {
        return auto_join_pvp_room(p, p->lv, pvp_lv);
    } else {
        return player_invite_pvp(p, btl_type, room_id, pvp_lv);
    }
}

/**
  * @brief Attend a pve battle
  */
int pve_btl_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t stage;
	uint8_t difficulty;
	uint32_t enter_type;
	uint32_t common_flg;

	unpack(body, stage, idx);
	unpack(body, difficulty, idx);
	unpack(body, enter_type, idx);
	unpack(body, common_flg, idx);

	/* 改对象封装了对内存的PACK和UNPACK操作作了边界检查，防止内存MEMCPY溢出	
	net_stream.init(body,  bodylen);
	net_stream.get_uint32(&stage);
	net_stream.get_uint8(&difficulty);
	net_stream.get_uint32(&enter_type);
	*/
	if (p->offline_data && p->offline_data->player_start_tm)
	{
		return -1;
	}


	if (p->trade_grp) {
		return send_header_to_player(p, p->waitcmd, cli_err_in_trade_svr, 1);
	}

	if (p->battle_grp || p->watch_info) {
		return send_header_to_player(p, p->waitcmd, cli_err_in_battle_svr, 1);
	}
	
	if (is_btl_time_limited(p)) {
		if (!can_user_in_special_stages(p, stage, difficulty)) {
			return send_header_to_player(p, p->waitcmd, cli_err_limit_time_btl, 1);
		}
	}
	if (difficulty == 11 && stage > (p->fumo_tower_top + 1 + 800)) {
		WARN_LOG("CANNOT IN TOWER STAGE\t[%u %u %u]", p->id, stage, p->fumo_tower_top);
		return send_header_to_player(p, p->waitcmd, cli_err_battle_no_such_stage, 1);
	}

    //temp setting for god stage
    if (stage >= 702 && stage <= 704) {
        uint32_t god_ret = is_in_active_time_section(11);
        if (god_ret) {
            return send_header_to_player(p, p->waitcmd, god_ret, 1);
        }
    }


	int time_ret =  check_activity_time(stage);
	if (time_ret) {
        return send_header_to_player(p, p->waitcmd, time_ret, 1);
	}

	if (stage == 705) {
		enter_type = 0;
	}

	uint32_t ret_err = operating_activity_limit(stage, p, enter_type ? false : true, difficulty);
	if (ret_err) {
		return send_header_to_player(p, p->waitcmd, ret_err, 1);
	} 



	if (stage == 978 ) {
		uint32_t map_id = get_mapid_low32(p->cur_map->id);
		if (p->puzzle_data->right_cir != intelligence_test_room::in_total_cir_count || (map_id < 43 || map_id > 53))
		{
			ERROR_LOG("USER %u HASN'T FINSISH PUZZLE 25 CIR RIGHT!", p->id);
			p->waitcmd = 0;
			return 0;
		}
	}
	//九天连环
	if (stage >= 989 && stage <= 997) {
		uint32_t acitve_id = 6;
		uint32_t pass_stage = get_player_other_info_value(p, acitve_id);
		if (pass_stage + 989 < stage ) {
			ERROR_LOG("PLAYR CAN'T ERNTER STAGE %u %u", p->id, stage);
			return -1;
		}	
	}	

    //if (stage >= 935 && stage <=938) {
    //    if (is_contest_first_stage_over()) {
    //        return send_header_to_player(p, p->waitcmd, cli_err_contest_stage_over, 1);
    //    }
    //}

	if (init_battle(p, btl_mode_pve, stage, difficulty, enter_type) == -1) {
		return send_header_to_player(p, p->waitcmd, 0, 1);
	}
	KDEBUG_LOG(p->id, "PVE\t[uid=%u stg=%u %u %u]", p->id, stage, difficulty, enter_type);
	if (common_flg) {
		uint32_t buff_id = 0;
		if (common_flg == 1) {
			buff_id = 1219;
		} else if (common_flg == 2) {
			buff_id = 1222;
		} else if (common_flg == 3) {
			buff_id = 1220;
		} else if (common_flg == 4) {
			buff_id = 1221;
		}
		if (buff_id) {
			del_buff_on_player(p, 1219);
			del_buff_on_player(p, 1220);
			del_buff_on_player(p, 1221);
			del_buff_on_player(p, 1222);
			add_buff_to_player(p, buff_id, 0, 3600, 0);
		}
	}
	return btlsvr_initiate_battle(p, stage, difficulty, btl_pve_battle, btl_mode_pve);
}

/**
  * @brief create a battle team
  */
int create_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	btl_team_attr_t team_attr = { 0 };
	uint32_t stage;
	uint32_t difficulty;

	unpack(body, stage, idx);
	unpack(body, difficulty, idx);
	unpack(body, team_attr.limit_num, idx);
	unpack(body, team_attr.refuse_hot_join, idx);
	KDEBUG_LOG(p->id, "CT ROOM\t[stage:%u p:%u diff:%u num:%u]", stage, p->id, difficulty, team_attr.limit_num);

	//check conditions
	if (p->battle_grp || p->watch_info) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_not_started, 1);
	}

	uint32_t cur_map_id = get_mapid_low32(p->cur_map->id);
	if (cur_map_id >= 43 && cur_map_id <= 52) {
		p->waitcmd = 0;
		return 0;
	}


	
	if (is_btl_time_limited(p)) {
		if (!can_user_in_special_stages(p, stage)) {
			return send_header_to_player(p, p->waitcmd, cli_err_limit_time_btl, 1);
		}
	}

	if (difficulty == 11) {
		WARN_LOG("CANNOT IN TOWER STAGE\t[%u %u %u]", p->id, stage, p->fumo_tower_top);
		return send_header_to_player(p, p->waitcmd, cli_err_battle_no_such_stage, 1);
	}

	uint32_t ret_err = operating_activity_limit(stage, p, true, difficulty);
	if (ret_err) {
		return send_header_to_player(p, p->waitcmd, ret_err, 1);
	}

	// create p->btl, and init it
	if (enter_room(p, -1) != 0) {
		return send_header_to_player(p, p->waitcmd, 0, 1);
	}

	// send cmd to btlserver
	idx = sizeof(btl_proto_t);
	idx += pack_btl_team_info(btlpkgbuf + idx, team_attr);
	idx += pack_initiate_battle_info(btlpkgbuf + idx, p, stage, difficulty, btl_mode_pve, false);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_create_btl_team);

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

/**
  * @brief create a battle team
  */
int join_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (is_btl_time_limited(p)) {
		return send_header_to_player(p, p->waitcmd, cli_err_limit_time_btl, 1);
	}
	if (p->offline_data && p->offline_data->player_start_tm)
	{
		return -1;
	}

	uint32_t cur_map_id = get_mapid_low32(p->cur_map->id);
	if (cur_map_id >= 43 && cur_map_id <= 52) {
		p->waitcmd = 0;
		return 0;
	}



	if (p->trade_grp) {
		return send_header_to_player(p, p->waitcmd, cli_err_in_trade_svr, 1);
	}

	if (p->battle_grp || p->watch_info) {
		return send_header_to_player(p, p->waitcmd, cli_err_in_battle_svr, 1);
	}
	
	int idx = 0;
	uint32_t globe_room_id;
	uint32_t team_leader;
	unpack(body, globe_room_id, idx);
	unpack(body, team_leader, idx);
	
	uint32_t local_room_id = get_local_room_id(globe_room_id);
	uint32_t btl_id = get_btl_id(globe_room_id);
	
	KDEBUG_LOG(p->id, "JOIN BTL\t[%u btlid=%u rmid=%u]", p->id, btl_id, local_room_id);

	int fd_idx = get_btl_fd_idx_by_server_id(btl_id);
	
	if (!is_fd_idx_valid(fd_idx)) {
		TRACE_LOG("team id is invalid:%u %08x %u", p->id, get_local_room_id(globe_room_id), fd_idx);
		return send_header_to_player(p, p->waitcmd, cli_err_battle_room_not_exist, 1);
	}
	if (enter_room(p, fd_idx) != 0) {
		return send_header_to_player(p, p->waitcmd, 0, 1);
	}
	
	idx = sizeof(btl_proto_t);
	pack_h(btlpkgbuf, local_room_id, idx);
	pack_h(btlpkgbuf, team_leader, idx);
	idx += pack_initiate_battle_info(btlpkgbuf + idx, p, 0, 0, btl_mode_pve, false);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_join_btl_team);

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}


/**
  * @brief hot join a battle team
  */
int hot_join_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t stage_id;
	uint32_t difficulty;
	unpack(body, stage_id, idx);
	unpack(body, difficulty, idx);
	if (p->offline_data && p->offline_data->player_start_tm)
	{
		return -1;
	}

	if (is_btl_time_limited(p)) {
		return send_header_to_player(p, p->waitcmd, cli_err_limit_time_btl, 1);
	}

	if (p->battle_grp || p->watch_info) {
		return send_header_to_player(p, p->waitcmd, cli_err_in_battle_svr, 1);
	}
	
	if (difficulty == 11) {
		WARN_LOG("CANNOT IN TOWER STAGE\t[%u %u]", p->id, p->fumo_tower_top);
		return send_header_to_player(p, p->waitcmd, cli_err_battle_no_such_stage, 1);
	}

	KDEBUG_LOG(p->id, "HOT JOIN BTL GOTING ID\t[%u stage=%u diff=%u]", p->id, stage_id, difficulty);

	return auto_join_room(p, stage_id, difficulty);
}


/**
  * @brief ready a battle team
  */
int ready_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (!(p->battle_grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_not_started, 1);
	}
	int idx = 0;
	idx = sizeof(btl_proto_t);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_ready_btl_team);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

/**
  * @brief ready a battle team
  */
int cancel_ready_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (!(p->battle_grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_not_started, 1);
	}
	KDEBUG_LOG(p->id, "CL READY BTEAM\t[%u btlid=%u fdidx=%u]", p->id, p->battle_grp->id, p->battle_grp->fd_idx);
	int idx = 0;

	idx = sizeof(btl_proto_t);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_cancel_ready_btl_team);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

/**
  * @brief set a battle team hot join flag
  */
int set_btl_team_hot_join_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (!(p->battle_grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_not_started, 1);
	}
	
	int idx = 0;
	uint32_t flag;
	unpack(body, flag, idx);
	KDEBUG_LOG(p->id, "SET JOIN FLG\t[%u btlid=%u fdidx=%u flg=%u]", p->id, p->battle_grp->id, p->battle_grp->fd_idx, flag);
	idx = sizeof(btl_proto_t);
	pack_h(btlpkgbuf, flag, idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_set_btl_team_hot_join);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

/**
  * @brief the team leader kick user from game room
  */
int kick_user_from_room_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (!(p->battle_grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_not_started, 1);
	}

	int idx = 0;
	uint32_t userid;
	unpack(body, userid, idx);
	TRACE_LOG("%u kick %u", p->id, userid);
	idx = sizeof(btl_proto_t);
	pack_h(btlpkgbuf, userid, idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_kick_user_from_room);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}


/**
  * @brief start a battle team
  */
int start_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (!(p->battle_grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_not_started, 1);
	}

	int idx = 0;

	TRACE_LOG("%u %u ", p->id, p->battle_grp ? p->battle_grp->id : 0);
	idx = sizeof(btl_proto_t);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_start_btl_team);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);

}

/**
  * @brief leave a battle team
  */
int leave_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (p->battle_grp) {
		KDEBUG_LOG(p->id, "LEAVE BTEAM\t[%u btlid=%u fdidx=%u]", p->id, p->battle_grp->id, p->battle_grp->fd_idx);
		int idx = 0;
		
		idx = sizeof(btl_proto_t);
		init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_leave_btl_team);
		return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
	}
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

/**
  * @brief leave a battle team
  */
int invite_friend_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
    uint32_t type = 0;
	uint32_t invitee_id;
	uint32_t room_id;
	uint32_t stage_id;
	uint32_t difficulty;
	unpack(body, type, idx);
	unpack(body, invitee_id, idx);
	unpack(body, room_id, idx);
	unpack(body, stage_id, idx);
	unpack(body, difficulty, idx);
	TRACE_LOG("%u %u %u %u %u %u", p->id, type, invitee_id, room_id, stage_id, difficulty);

	DEBUG_LOG("Player %u INVITE PLATER %u PVP %u", p->id, invitee_id, stage_id);
	if (p->battle_grp) {
		player_t* to = get_player(invitee_id);
		if (to) {
			if (to->trade_grp) {
				send_rltm_notification(cli_proto_reply_btl_invite_notify, p->id, to, 
					type, sw_rt_nodify_reply_status_trd, 0);
			} else if (to->battle_grp) {
				send_rltm_notification(cli_proto_reply_btl_invite_notify, p->id, to, 
					type, sw_rt_nodify_reply_status_btl, 0);
			} else if (to->client_buf[4] && type == sw_rt_nodify_pvp && !is_player_have_friend(to, p->id)) {
					send_rltm_notification(cli_proto_reply_btl_invite_notify, p->id, to, 
						type, sw_rt_nodify_reply_set_conf_refuse, 0);
			} else {
				send_rltm_notification(cli_proto_reply_btl_invite_notify, p->id, to, 
						type, sw_rt_nodify_reply_status_nor, 0);
				send_rltm_notification(cli_proto_rltm_notify, invitee_id, p, type, 0, room_id, stage_id, difficulty);
			}
		} else {
			send_rltm_notification(cli_proto_rltm_notify, invitee_id, p, type, 0, room_id, stage_id, difficulty);
		}
		return send_header_to_player(p, p->waitcmd, 0, 1);
	}
	return send_header_to_player(p, p->waitcmd, cli_err_battle_not_started, 1);
}

/**
  * @brief reply friend to btl team
  */
int reply_friend_btl_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	uint32_t cur_map_id = get_mapid_low32(p->cur_map->id);
	if (cur_map_id >= 43 && cur_map_id <= 52) {
		p->waitcmd = 0;
		return 0;
	}

	int idx = 0;
    uint32_t type = 0;
	uint32_t invite_id;
	uint32_t accept;
	unpack(body, type, idx);
	unpack(body, invite_id, idx);
	unpack(body, accept, idx);
	send_rltm_notification(cli_proto_reply_btl_invite_notify, invite_id, p, type, accept, 0);
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

/**
  * @brief restart a pve battle
  */
int pve_btl_restart_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	battle_grp_t* grp = p->battle_grp;
	if ((grp == 0) || !is_battle_started(grp) || grp->battle_mode != btl_mode_pve) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);
	}
	if (grp->btl_type == battle_type_multi) {
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
	}
	if (is_btl_time_limited(p)) {
		return send_header_to_player(p, p->waitcmd, cli_err_limit_time_btl, 1);
	}
	do_btl_restart(p, 0, 0);


	return 0;
}

/**
  * @brief restart a jump battle
  */
int pve_btl_stage_jump_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t stage_id;
	unpack(body, stage_id, idx);

	if (stage_id < 801 || stage_id > 804) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);
	}
	battle_grp_t* grp = p->battle_grp;
	if ((grp == 0) || !is_battle_started(grp) || grp->battle_mode != btl_mode_pve) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);
	}

	if (is_btl_time_limited(p)) {
		return send_header_to_player(p, p->waitcmd, cli_err_limit_time_btl, 1);
	}
	do_btl_restart(p, stage_id, 11);
	return 0;
}


int sync_process_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	battle_grp_t* grp = p->battle_grp;
	if ((grp == 0) || !is_battle_started(grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);
	}

	int idx = 0;
	uint32_t process = 0;
	unpack(body, process, idx);

	return btlsvr_sync_process(p, process);
}


/**
  * @brief a player informs server that he/she is ready to fight
  * @return 0 on success, -1 on error
  */
int player_ready_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	battle_grp_t* grp = p->battle_grp;
	if ((grp == 0) || !is_battle_started(grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);
	}
	KDEBUG_LOG(p->id, "RDY BTL\t[uid=%u]", p->id);
	return btlsvr_player_ready(p);
}

/**
  * @brief move in a battle
  */
int player_move_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	battle_grp_t* grp = p->battle_grp;
	if ((grp == 0) || !is_battle_started(grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);
	}

	uint32_t x, y, z;
	uint32_t old_x, old_y, old_z;
	uint8_t  dir, move_type;
	uint32_t client_tm;
	uint32_t client_msec;

	int i = 0;
	
	unpack(body, old_x, i);
	unpack(body, old_y, i);
	unpack(body, old_z, i);
	unpack(body, x, i);
	unpack(body, y, i);
	unpack(body, z, i);
	unpack(body, dir, i);
	unpack(body, move_type, i);
	unpack(body, client_tm, i);
	unpack(body, client_msec, i);

	uint32_t sys_tm = get_now_tv()->tv_sec;
	uint32_t tm_diff = client_tm > sys_tm ? client_tm - sys_tm : 0;
	if (tm_diff > 60) {
        do_stat_log_universal_interface_1(stat_log_used_boost_num, 1, 1);
		WARN_LOG("client may fast[%u %u]", p->id, tm_diff);
	}
	TRACE_LOG("%u:%u %u:%u  %u %u", old_x, x, old_y, y, dir, move_type);
	return btlsvr_player_move(p, old_x, old_y, old_z, x, y, z, dir, move_type, client_tm, client_msec);
}

/**
 * @brief response to a lag test package
 */
int player_rsp_lag_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	battle_grp_t* grp = p->battle_grp;
	if ((grp == 0) || !is_battle_started(grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);
	}

	/*if (grp->id == 0) {
		p->waitcmd = 0;
		return 0;
	}*/

	int idx = 0;
	uint32_t seq;
	unpack(body, seq, idx);

	return btlsvr_player_rsp_lag(p, seq);
}

int player_attack_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	battle_grp_t* grp = p->battle_grp;
	if ((grp == 0) || !is_battle_started(grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);
	}

	p->waitcmd = 0;

	int idx = 0;
	uint32_t skill_id, x, y, z, client_tm, client_msec;
	unpack(body, skill_id, idx);
	unpack(body, x, idx);
	unpack(body, y, idx);
	unpack(body, z, idx);
	unpack(body, client_tm, idx);
	unpack(body, client_msec, idx);
	
	//KDEBUG_LOG(p->id, "ATK\t[uid=%u sid=%u]", p->id, skill_id);
	return btlsvr_player_attack(p, btl_player_attack, skill_id, x, y, z, client_tm, client_msec);
}

int summon_unique_skill_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	battle_grp_t* grp = p->battle_grp;
	if ((grp == 0) || !is_battle_started(grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);
	}

	int idx = 0;
	uint32_t skill_id;
	unpack(body, skill_id, idx);
	//unpack(body, skill_lv, idx);
	
	idx = sizeof(btl_proto_t);
	pack_h(btlpkgbuf, skill_id, idx);
	//pack_h(btlpkgbuf, skill_lv, idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_summon_unique_skill);

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
	
}

/*int player_use_skill_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	battle_grp_t* grp = p->battle_grp;
	if ((grp == 0) || !is_battle_started(grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);
	}

	int idx = 0;
	uint32_t skill_id, x, y, z;
	unpack(body, skill_id, idx);
	unpack(body, x, idx);
	unpack(body, y, idx);
	unpack(body, z, idx);

	KDEBUG_LOG(p->id, "SKILL\t[uid=%u sid=%u]", p->id, skill_id);
	return btlsvr_player_attack(p, btl_player_use_skill, skill_id, x, y, z);
}*/

int player_pick_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	battle_grp_t* grp = p->battle_grp;
	if ((grp == 0) || !is_battle_started(grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);
	}

	int idx = 0;
	uint32_t unique_id;
	uint32_t mon_tm;
	unpack(body, unique_id, idx);
	unpack(body, mon_tm, idx);
	if (mon_tm)
	{
		if (!(p->fight_summon) ||
				mon_tm != p->fight_summon->mon_tm || 
				(p->fight_summon->mon_type / 10) != 108) {
			ERROR_LOG("[%u] ERROR MON_TM [%u]" ,p->id, mon_tm);
			return -1;
		}
	}
	uint32_t max_bag_grid_count = get_player_total_item_bag_grid_count(p);
	KDEBUG_LOG(p->id, "PICK\t[uid=%u unique_id=%u]", p->id, unique_id);
	return btlsvr_player_pick_item(p, btl_player_pick_item, unique_id, max_bag_grid_count, mon_tm);

}

int player_enter_map_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	battle_grp_t* grp = p->battle_grp;
	if ((grp == 0) || !is_battle_started(grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);
	}

	int idx = 0;
	uint32_t mid;
	unpack(body, mid, idx);
	return btlsvr_player_enter_map(p, mid);
}

int player_leave_btl_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	battle_grp_t* grp = p->battle_grp;
	if ((grp == 0) || !is_battle_started(grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);
	}

	KDEBUG_LOG(p->id, "L B\t[uid=%u mod=%u]", p->id, grp->battle_mode);
	return btlsvr_cancel_battle(p, 1);
}

int player_open_treasure_box_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	battle_grp_t* grp = p->battle_grp;
	if ((grp == 0) || !is_battle_started(grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);
	}

	int idx = 0;
	uint8_t	box_type = 0;
	uint8_t box_id = 0;
	unpack(body, box_type, idx);
	unpack(body, box_id, idx);
	
	uint32_t item_pack_max = get_player_total_item_bag_grid_count(p);
	idx = sizeof(btl_proto_t);
	pack_h(btlpkgbuf, static_cast<uint32_t>(item_pack_max), idx);
	pack_h(btlpkgbuf, static_cast<uint32_t>(box_type), idx);
	pack_h(btlpkgbuf, static_cast<uint32_t>(box_id), idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_player_open_treasure_box);

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int get_player_attr_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	battle_grp_t* grp = p->battle_grp;
	if ((grp == 0) || !is_battle_started(grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);
	}
	int idx = sizeof(btl_proto_t);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_get_player_attr);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int get_mon_attr_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	battle_grp_t* grp = p->battle_grp;
	if ((grp == 0) || !is_battle_started(grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);
	}
	int idx = 0;
	uint32_t monid;
	unpack(body, monid, idx);
	
	idx = sizeof(btl_proto_t);
	taomee::pack_h(btlpkgbuf, monid, idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_get_mon_attr);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int roll_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	battle_grp_t* grp = p->battle_grp;
	if ((grp == 0) || !is_battle_started(grp)) {
		return send_header_to_player(p, p->waitcmd, cli_err_battle_ended, 1);			    
	}
	int idx = 0;
	uint32_t battle_id;
	uint32_t roll_id;
	int32_t  roll_flag;
	unpack(body, battle_id, idx);
	unpack(body, roll_id, idx);
	unpack(body, roll_flag, idx);

	idx = sizeof(btl_proto_t);

	pack_h(btlpkgbuf, battle_id, idx);
	pack_h(btlpkgbuf, roll_id, idx);
	pack_h(btlpkgbuf, roll_flag, idx);

	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_player_roll_item);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}


//-------------------------------------------------------------------

/**
  * @brief Initiate a battle
  * @param p
  * @param battle_mode
  * @return 0 if battle inited successfully, 1 if no fightable monsters on hand, -1 otherwise
  */
static int init_battle(player_t* p, uint32_t battle_mode, uint32_t stage_id, uint32_t difficulty, uint32_t enter_type)
{
	battle_grp_t* grp = p->battle_grp;
	if (grp == 0) {
		// allocate a battle group
		p->battle_grp = alloc_battle_grp();
		grp = p->battle_grp;

		grp->battle_mode = battle_mode;
		grp->stage_id = stage_id;
		grp->fd_idx 	 = p->id % battle_svr_cnt;
		grp->difficulty = difficulty;
		grp->enter_type = enter_type;
        grp->end_reason = -1;

		KDEBUG_LOG(p->id, "INIT BTL\t[uid=%u mod=%u]", p->id, battle_mode);
		//notify home server if player at homeserver
		if(p->home_grp)
		{
			homesvr_player_enter_battle(p);
		}
		return 0;
	}
	WARN_LOG("you've join a battle: uid=%u btl_id=%u", p->id, grp->id);
	return -1;
}

/**
  * @brief start a battle
  * @param p
  */
inline void start_battle(player_t* p)
{
	battle_grp_t* grp = p->battle_grp;

	grp->flag    = set_bit_on(grp->flag, battle_started);
	p->invisible = 1;

	send_leave_map_rsp(p, 0);

	KDEBUG_LOG(p->id, "BTL START\t[uid=%u]", p->id);
}

/**
  * @brief leave a battle
  * @param p
  * @param reason reason that a battle is ended. 0: normal, 1: system error
  */
static void leave_battle(player_t* p, uint8_t reason)
{
	// send leave battle notification
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, reason, idx);
	pack(pkgbuf, p->dexp_time, idx);
	pack(pkgbuf, p->fumo_points_total, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_player_leave_btl, idx);
	send_to_player(p, pkgbuf, idx, 0);

    /*
	uint32_t pre_id = get_mapid_low32(p->cur_map->id);
	if (pre_id >= 43 && pre_id <= 48) {
		map_t * m = p->cur_map;
		--(m->player_num);
		list_del(&p->maphook);

		m = get_map(make_mapid(0, 1));
		list_add_tail(&(p->maphook), &(m->playerlist));
		++(m->player_num);
		p->cur_map = m;
		p->xpos = m->init_x;
		p->ypos = m->init_y;
	}*/
	if (p->cur_map->id == 40) {
		enter_map(p, 7, 150, 450);
	} else {
		send_entermap_rsp(p, 0);
	}
	p->invisible  = 0;

    if (p->trigger_event) {
        if (p->battle_grp->end_reason == end_btl_normal) {
            do_event_trigger_logic(p, p->trigger_event, p->trigger_times, p->battle_grp->end_reason);
        } else {
            send_block_result_to_client(p, p->trigger_event, p->trigger_times, p->battle_grp->end_reason);
            chg_app_buff_back(p, 3000);
            set_player_event_trigger(p, 0, 0, 0);
        }
        if (is_celebration_goods_full()) {
            chg_app_buff_back(p, 3000);
            set_player_event_trigger(p, 0, 0, 0);
            del_event_trigger_timer(p);
        }
    }


	// statements here to solve this problem.
	if (p->waitcmd != 0) {
		if ( (p->waitcmd == cli_proto_player_ready)
				|| ((p->waitcmd >= cli_proto_btl_action_start)
						&& (p->waitcmd <= cli_proto_btl_action_end)) ) {
			p->waitcmd = 0;
		}
	}

	KDEBUG_LOG(p->id, "L B S\t[rsn=%u]", reason);
	free_battle_grp(p->battle_grp, p);
	p->battle_grp = 0;
}

//-------------------------------------------------------------------
static int pack_btl_team_info(uint8_t* buf, btl_team_attr_t& team_attr)
{
	int idx = 0;
	pack_h(buf, team_attr.limit_num, idx);
	pack_h(buf, team_attr.refuse_hot_join, idx);
	//pack(buf, team_attr.nick, sizeof(team_attr.nick), idx);	
	return idx;
}

static int pack_skill_to_btl(player_t* p, uint8_t* buf)
{
    int idx = 0;
    //pack_h(buf, p->player_skill_map.size(), idx);
    PlayerSkillMap::iterator it = p->player_skill_map->begin();
    for ( ; it != p->player_skill_map->end(); ++it) {
        pack_h(buf, it->second.skill_id, idx);
        pack_h(buf, it->second.lv, idx);
        if ( it->second.key == 0) {
            pack_h(buf, static_cast<uint8_t>(0), idx);
        } else {
            pack_h(buf, static_cast<uint8_t>(1), idx);
        }
        TRACE_LOG("BTLSKILL skill [%u %u %u]",it->second.skill_id, it->second.lv, it->second.key);
    }
    return idx;
}

static int pack_initiate_battle_info(uint8_t* buf, player_t* p, uint32_t stage, uint32_t difficulty, uint32_t battle_mode, uint32_t btl_again)
{
	int idx = 0;
	pack_h(buf, stage, idx);
	pack_h(buf, difficulty, idx);
	pack_h(buf, p->role_type, idx);
	pack_h(buf, p->power_user, idx);
	pack_h(buf, p->role_tm, idx);
	if (is_achieve_amb_lv(p)) {
		TRACE_LOG("have achieve");
		pack_h(buf, 0, idx);
	} else {
		TRACE_LOG("not achieve");
		pack_h(buf, p->parentid, idx);
	}
	pack(buf, p->once_bit, sizeof(p->once_bit), idx);
	pack(buf, p->nick, sizeof(p->nick), idx);
	pack_h(buf, battle_mode, idx);

	pack_h(buf, p->using_achieve_title, idx);
	pack_h(buf, p->lv, idx);
	pack_h(buf, p->exp, idx);
	pack_h(buf, p->allocator_exp, idx);
	pack_h(buf, p->dexp_time, idx);
	if (is_time_limit_2(p)) {
		p->half_exp = 1;
	} else {
		p->half_exp = 0;
	}
	pack_h(buf, p->half_exp, idx);
	pack_h(buf, p->coins, idx);
	pack_h(buf, p->skill_point, idx);
	pack_h(buf, p->hp, idx);
	pack_h(buf, p->mp, idx);
	pack_h(buf, p->attire_gs, idx);
	pack_h(buf, p->cur_continue_win, idx);
	pack_h(buf, p->honor, idx);
	pack_h(buf, p->vip, idx);
	pack_h(buf, p->vip_level, idx);
	pack_h(buf, get_player_total_item_bag_grid_count(p), idx);
	pack_h(buf, p->max_conti_win_times, idx);
	pack_h(buf, static_cast<uint8_t>(btl_again), idx);
	pack_h(buf, p->fumo_tower_top, idx);
	pack_h(buf, p->fumo_tower_used_tm, idx);
	pack_h(buf, p->player_show_state, idx);
	pack_h(buf, p->taotai_info->do_times, idx);
	pack_h(buf, p->taotai_info->win_times, idx);
	if (can_get_team_attr_add(p)) {
		pack_h(buf, p->team_info.team_lv, idx);
	} else {
		pack_h(buf, 0, idx);
	}


	kill_boss_data * t = get_player_kill_boss_data(p, stage, difficulty);
	if (t) {
		pack_h(buf, t->pass_cnt_, idx);
	} else {
		pack_h(buf, 0, idx);
	}
	pack(buf, p->add_quality, sizeof(quality_t), idx);
	pack(buf, p->unique_item_bit, c_unique_item_bit_size, idx);

	pack_h(buf, p->clothes_num, idx);
	pack(buf, p->clothes, sizeof(p->clothes), idx);

	
	idx += pack_task_to_btl(p, buf + idx, stage, difficulty);

//	idx += pack_task_step_to_btl(p, buf + idx, stage, difficulty);

	idx += pack_fumo_to_btl(p, buf + idx);

//	idx += pack_task_monster_info(p, buf + idx, stage);
#ifdef DEV_SUMMON
	idx += pack_summon_to_btl(p, buf + idx, battle_mode, (stage != 916));
#endif

	idx += pack_fight_numen(p, buf + idx);

    uint32_t cnt = 0;
	uint32_t buff_cnt = 0;
	int buff_idx = idx;
	idx += 4;
	//pack_h(buf, cnt, idx);
    cnt = p->player_skill_map->size();
    pack_h(buf, cnt, idx);
	cnt = p->home_btl_pets->size();
	pack_h(buf, cnt, idx);
	
	idx += pack_buf_skill_to_btl(p, buf + idx, buff_cnt);	
    idx += pack_skill_to_btl(p, buf + idx);
	pack_h(buf, buff_cnt, buff_idx);
	for(uint32_t i =0; i< p->home_btl_pets->size(); i++)
	{
		pack_h(buf, (*p->home_btl_pets)[i], idx);
	}

	return idx;
}

int pack_join_btl_team_rsp(uint8_t* buf, player_t* p, btl_proto_t* bpkg)
{
	int idx = 0;
	uint32_t room_id;
	uint32_t btl_mode;
	uint32_t stage_id;
	uint32_t difficulty;
	uint32_t cnt;

	unpack_h(bpkg->body, btl_mode, idx);
	unpack_h(bpkg->body, room_id, idx);
	unpack_h(bpkg->body, stage_id, idx);
	unpack_h(bpkg->body, difficulty, idx);
	unpack_h(bpkg->body, cnt, idx);

	idx = 0;
	pack(buf, room_id, idx);
	pack(buf, stage_id, idx);
	pack(buf, difficulty, idx);
	pack(buf, cnt, idx);
	TRACE_LOG("%u %u %u %u %u", p->id, room_id, stage_id, difficulty, cnt);
	join_btl_rsp_item_t* p_rsp_item = (join_btl_rsp_item_t*)(bpkg->body + 20);
	for (uint32_t i = 0; i < cnt; i++) {
		if ( i > 10 ) {
			WARN_LOG("pack_join_btl_team_rsp: uid=%u  CNT=%u", p->id, cnt);
			break;
		}
		pack(buf, p_rsp_item->owner_id, idx);
		pack(buf, p_rsp_item->id, idx);
		pack(buf, p_rsp_item->role_tm, idx);
		pack(buf, p_rsp_item->role_type, idx);
		pack(buf, p_rsp_item->power_user, idx);
		pack(buf, p_rsp_item->show_state, idx);
		pack(buf, p_rsp_item->vip, idx);
		pack(buf, p_rsp_item->vip_lv, idx);
		pack(buf, p_rsp_item->team, idx);
		pack(buf, p_rsp_item->nick, sizeof(p_rsp_item->nick), idx);
		pack(buf, p_rsp_item->lv, idx);
		pack(buf, p_rsp_item->maxhp, idx);
		pack(buf, p_rsp_item->hp, idx);
		pack(buf, p_rsp_item->maxmp, idx);
		pack(buf, p_rsp_item->mp, idx);
		pack(buf, p_rsp_item->exp, idx);
		pack(buf, p_rsp_item->honor, idx);
		pack(buf, p_rsp_item->x, idx);
		pack(buf, p_rsp_item->y, idx);
		pack(buf, p_rsp_item->dir, idx);
		pack(buf, p_rsp_item->speed, idx);

		pack(buf, p_rsp_item->clothes_cnt, idx);
		//uint32_t *p_tmp = p_rsp_item->clothes_id;
        clothes_base_t *clothes_arr = p_rsp_item->clothes;
		for (uint32_t j = 0; j < p_rsp_item->clothes_cnt; j++) {
			//pack(buf, (*p_tmp), idx);
			//p_tmp++;
            pack(buf, clothes_arr[j].id, idx);
            pack(buf, clothes_arr[j].gettime, idx);
            pack(buf, clothes_arr[j].timelag, idx);
            pack(buf, clothes_arr[j].lv, idx);
		}

        uint8_t *p_tmp = (uint8_t *)(p_rsp_item->clothes) + sizeof(clothes_base_t) * p_rsp_item->clothes_cnt;
		uint16_t summon_cnt = *(uint16_t*)p_tmp;
		pack(buf, summon_cnt, idx);
        TRACE_LOG("uid=[%u] join btl summon cnt [%u]", p->id, summon_cnt);
		join_btl_summon_rsp_item_t* p_summ_item = (join_btl_summon_rsp_item_t*)(((uint16_t*)p_tmp) + 1);
		for (uint16_t j = 0; j < summon_cnt; j++) {
			pack(buf, p_summ_item->mon_tm, idx);
			pack(buf, p_summ_item->id, idx);
			pack(buf, p_summ_item->role_type, idx);
			pack(buf, p_summ_item->nick, sizeof(p_summ_item->nick), idx);
			pack(buf, p_summ_item->lv, idx);
			pack(buf, p_summ_item->fight_value, idx);
			pack(buf, p_summ_item->exp, idx);
			pack(buf, p_summ_item->anger_value, idx);

			p_summ_item++;
		}
		p_rsp_item = (join_btl_rsp_item_t*)(p_summ_item);
	}
	return idx;
}


/**
  * @brief tell battle server that 'p' is going to start a battle
  * @param stage the stage that a player wants to enter
  * @return 0 on success, -1 on error
  */
static int btlsvr_initiate_battle(player_t* p, uint32_t stage, uint32_t difficulty, uint32_t cmd, uint32_t battle_mode, uint32_t btl_again)
{
	int idx = sizeof(btl_proto_t);
	idx += pack_initiate_battle_info(btlpkgbuf + idx, p, stage, difficulty, battle_mode, btl_again);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, cmd);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

/**
  * @brief tell battle server that 'p' is ready to fight
  * @return 0 on success, -1 on error
  */
inline int btlsvr_player_ready(player_t* p)
{
	int idx = sizeof(btl_proto_t);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_player_ready);

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

/**
  * @brief sync clients process percent
  * @return 0 on success, -1 on error
  */
inline int btlsvr_sync_process(player_t* p, uint32_t process)
{
	int idx = sizeof(btl_proto_t);
	pack_h(btlpkgbuf, process, idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_sync_process);

	TRACE_LOG("PVP sync_process\t[uid=%u %u]", p->id, process);
	send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
	return send_header_to_player(p, p->waitcmd, 0, 1);

}
/**
  * @brief tell battle server that 'p' is moving
  * @return 0 on success, -1 on error
  */
inline int btlsvr_player_move(player_t* p, uint32_t old_x, uint32_t old_y, uint32_t old_z,  uint32_t x, uint32_t y, uint32_t z, uint8_t dir, uint8_t mv_type, uint32_t client_tm, uint32_t client_msec)
{
	int idx = sizeof(btl_proto_t);
	pack_h(btlpkgbuf, old_x, idx);
	pack_h(btlpkgbuf, old_y, idx);
	pack_h(btlpkgbuf, old_z, idx);
	pack_h(btlpkgbuf, x, idx);
	pack_h(btlpkgbuf, y, idx);
	pack_h(btlpkgbuf, z, idx);
	pack_h(btlpkgbuf, dir, idx);
	pack_h(btlpkgbuf, mv_type, idx);
	pack_h(btlpkgbuf, client_tm, idx);
	pack_h(btlpkgbuf, client_msec, idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_player_move);

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

/**
  * @brief respond a lag testing
  * @return 0 on success, -1 on error
  */
inline int btlsvr_player_rsp_lag(player_t* p, uint32_t seq)
{
	int idx = sizeof(btl_proto_t);
	pack_h(btlpkgbuf, seq, idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_player_rsp_lag);

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

inline int btlsvr_player_attack(player_t* p, uint16_t cmd, uint32_t skill_id, uint32_t x, uint32_t y, uint32_t z, uint32_t client_tm, uint32_t client_msec)
{
	int idx = sizeof(btl_proto_t);
	pack_h(btlpkgbuf, skill_id, idx);
	pack_h(btlpkgbuf, x, idx);
	pack_h(btlpkgbuf, y, idx);
	pack_h(btlpkgbuf, z, idx);
	pack_h(btlpkgbuf, client_tm, idx);
	pack_h(btlpkgbuf, client_msec, idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, cmd);

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

inline int btlsvr_player_enter_map(player_t* p, uint32_t mid)
{
	int idx = sizeof(btl_proto_t);
	pack_h(btlpkgbuf, mid, idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_player_enter_map);
	TRACE_LOG("%u %u", p->id, mid);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

//-------------------------------------------------------------------

#pragma pack(1)

struct cancel_btl_rsp_t {
	uint16_t	lv;
	uint32_t	exp;
	uint32_t	allocator_exp;
    uint32_t    dexp_time;
	uint8_t		once_bit[max_once_bit_size];
	uint32_t	coins;
	uint32_t	hp;
	uint32_t	mp;
	
//	uint32_t	continue_win;
	uint32_t	honor;
	uint32_t	exploit;
//	uint32_t	win_times;
//	uint32_t	lose_times;
//	uint32_t    max_conti_win_times;

	uint32_t	sp;
	uint32_t	fumo_points;
	uint32_t	fumo_tower_top;
	uint32_t	fumo_tower_used_tm;
	uint32_t    going_flag;
	//uint32_t	taotai_do;
	//uint32_t	taotai_win;
	uint8_t		unique_item_bit[c_unique_item_bit_size];
	uint32_t	avg_flg;
	//uint32_t	stage_id;

	player_clothes_info_t clothes[max_clothes_on];

	//summon info
	uint32_t	summon_tm;
	uint16_t	summon_lv;
	uint32_t	summon_exp;
	uint16_t	fight_value;

	//uint32_t	task_count;
	uint32_t	buf_skill_cnt;
	uint8_t		body[];
};

struct lv_match_rsp_t {
	uint32_t	btl_mode;
	userid_t	first_player;
    uint8_t     type;
	uint32_t    btl_id;
	uint32_t    ready;
};

struct player_tmp_team_pvp_rsp_t {
	uint32_t btl_mode;
	uint32_t btl_id;
};

struct noti_online_all_player_joined_ready_to_start_t {	
	uint32_t stage_id;	
	uint32_t btl_mode;
};


struct move_rsp_t {
	userid_t	uid;
	uint32_t	x;
	uint32_t	y;
	uint32_t	z;
	uint8_t		dir;
	uint8_t		mv_type;
	uint16_t	lag;
};

struct jump_rsp_t {
	userid_t	uid;
	uint32_t	x;
	uint32_t	y;
};

struct stop_rsp_t {
	userid_t	uid;
	uint32_t	x;
	uint32_t	y;
	uint8_t		dir;
};

struct attack_rsp_t {
	userid_t	uid;
	uint32_t	team;
	uint32_t	role_type;
	uint32_t	skill_id;
	uint32_t	skill_lv;
	uint32_t	x;
	uint32_t	y;
	uint32_t	z;
};

struct entermap_rsp_t {
	uint32_t	x;
	uint32_t	y;
	uint8_t		dir;
};

struct lvup_rsp_t {
	uint32_t	id;
	uint32_t	lv;
	uint32_t	hp;
	uint32_t	mp;
};

struct battle_over_pkg_t {
	uint32_t 		btl_mode;
	uint32_t		reason;
	userid_t		winner;
	uint32_t		stageid;
	uint32_t		bossid;
	uint32_t		difficulty;
	uint8_t			stage_grade;
	uint32_t		damage;
	uint32_t        pass_time;
	uint32_t        use_skill_cnt;
	uint32_t        be_attacked_cnt;
};

struct pick_item_rsp_t {
	uint8_t 	pick_flag;
	userid_t	uid;
	uint32_t	item_pick_id;
	uint32_t	item_id;
	uint32_t	unique_id;
	uint32_t    mon_tm;
};

struct box_item_info_t {
	uint32_t	quality;
	uint32_t	get_flag;
	uint32_t	item_type;
	uint32_t	item_id;
	uint32_t	item_cnt;
	uint32_t	item_duration;
	uint32_t	unique_index;
};

struct open_treasure_box_rsp_t {
	uint32_t	box_type;
	uint32_t	box_id;
	uint32_t	box_cnt;
	box_item_info_t	boxs[];
};

struct player_roll_item_rsp_t{
	uint32_t battle_id;
	uint32_t roll_id;
	int32_t roll_point;
};

struct player_get_roll_item_rsp_t{
	uint32_t roll_id;
	uint32_t player_id;
	uint32_t item_id;
	uint32_t unique_id;
	int32_t  sucess;
};

struct join_btl_team_rsp_t {
	uint32_t btl_mode;
	uint32_t room_id;
	uint32_t stage_id;
	uint32_t difficulty;
};

struct player_ready_rsp_t {
	uint32_t id;
	uint32_t all_ready;
};

struct auto_del_item_rsp_head_t{
	uint32_t count;
};

struct auto_del_item_rsp_item_t{
	uint32_t itemid;
	uint32_t itemcnt;
};


struct open_box_times_rsp_t{
	uint32_t times;
};
#pragma pack()
int btlsvr_open_box_times_callback(player_t* p, btl_proto_t* bpkg)
{
	open_box_times_rsp_t* req = reinterpret_cast<open_box_times_rsp_t*>(bpkg->body);
	p->open_box_times += req->times;

	db_set_player_open_box_times(p, p->open_box_times);

//	if(p->open_box_times == 1 && !is_player_achievement_data_exist(p, 300))
//	{
//		do_achieve_type_logic(p, 300, false);
//	}
//	if(p->open_box_times == 100 && !is_player_achievement_data_exist(p, 301))
//	{
//		do_achieve_type_logic(p, 301, false);					    
//	}
//	if(p->open_box_times == 500 && !is_player_achievement_data_exist(p, 302))
//	{
//		do_achieve_type_logic(p, 302, false);
//	}
	return 0;
}


int btlsvr_auto_del_item_callback(player_t* p, btl_proto_t* bpkg)
{
	auto_del_item_rsp_head_t* req = reinterpret_cast<auto_del_item_rsp_head_t*>(bpkg->body);
	auto_del_item_rsp_item_t* item = reinterpret_cast<auto_del_item_rsp_item_t*>(req + 1);	

	db_del_item_elem_t arr[100];
	uint32_t del_cnt = 0;
	for (uint32_t i = 0; i < req->count; i++) {
		if (i >= 100) {
			break;
		}
		auto_del_item_rsp_item_t *p_tmp = item + i;
		
		arr[i].item_id = p_tmp->itemid;
		arr[i].count = p_tmp->itemcnt;
		del_cnt ++;
	}
	
	player_del_items(p, arr, del_cnt, true);
	return 0;
}


int btlsvr_player_roll_item_callback(player_t* p, btl_proto_t* bpkg)
{
	player_roll_item_rsp_t* req =  reinterpret_cast<player_roll_item_rsp_t*>(bpkg->body);
	int idx = 0;
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, req->battle_id, idx);
	pack(pkgbuf, req->roll_id, idx);
	pack(pkgbuf, req->roll_point, idx);

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int btlsvr_player_get_roll_item_callback(player_t* p, btl_proto_t* bpkg)
{
	player_get_roll_item_rsp_t* req = reinterpret_cast<player_get_roll_item_rsp_t*>(bpkg->body);
	if(p->id == req->player_id && req->sucess == 1){
		if( req->unique_id == 0)
		{
			p->my_packs->add_item(p, req->item_id, 1, channel_string_pick, true, monster_drop);
		}
		else
		{
			p->my_packs->add_clothes(p, req->item_id, req->unique_id, 0, channel_string_pick);
		}
	}
	
	int idx = 0;
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, req->roll_id, idx);
	pack(pkgbuf, req->player_id, idx);
	pack(pkgbuf, req->item_id, idx);
	pack(pkgbuf, req->unique_id, idx);
	pack(pkgbuf, req->sucess, idx);

	init_cli_proto_head(pkgbuf, p, cli_proto_get_roll_item, idx);
	return send_to_player(p, pkgbuf, idx, 0);
}
/**
  * @brief callback for cancelling a battle
  */
int btlsvr_cancel_battle_callback(player_t* p, btl_proto_t* bpkg)
{
	cancel_btl_rsp_t* pkg = reinterpret_cast<cancel_btl_rsp_t*>(bpkg->body);

	CHECK_VAL_EQ(bpkg->len - sizeof(btl_proto_t), sizeof(cancel_btl_rsp_t) + 
					pkg->buf_skill_cnt * sizeof(player_buf_skill_rsp_t));

	p->lv   = pkg->lv;
	p->exp  = pkg->exp;
	p->allocator_exp = pkg->allocator_exp;
    p->dexp_time = pkg->dexp_time;
	memcpy(p->once_bit, pkg->once_bit, sizeof(pkg->once_bit));
	do_stat_log_coin_add_reduce(p, pkg->coins, channel_string_pick);
	p->coins  = pkg->coins;
	p->honor = pkg->honor;
	p->exploit += pkg->exploit;

	p->skill_point = pkg->sp;
	
	p->fumo_points_total += (pkg->fumo_points - p->fumo_points_today);
	p->fumo_points_today = pkg->fumo_points;

	do_stat_log_fumo_add_reduce( (pkg->fumo_points - p->fumo_points_today),(uint32_t)0);

	//cancel the going batlle player lose pvp
	uint32_t cur_map_id = get_mapid_low32(p->cur_map->id);
	if (pkg->going_flag) { 
		if ( p->battle_grp->pvp_btl_type >= pvp_red_blue_1 
				&& p->battle_grp->pvp_btl_type <= pvp_red_blue_3) {
			proc_player_banner_logic_after_btl_over(p, 0);
		}

		if (cur_map_id >= 1004 && cur_map_id <= 1013) {
			player_leave_team_contest_btl(p, 0);
		}
	}


	//p->taotai_info->win_times = pkg->taotai_win;
	
	memcpy(p->unique_item_bit, pkg->unique_item_bit, sizeof(p->unique_item_bit));

	TRACE_LOG("%u  %u  %u  %u  %u  %u  %u  %u  %u %u %u %u [%u %u]",
        p->cur_continue_win, p->honor, p->exploit, p->win_times,
		p->lose_times, p->max_conti_win_times, p->id, p->fumo_points_today, pkg->fumo_tower_top, 
		pkg->fumo_tower_used_tm, p->skill_point, p->fumo_points_total,
        p->taotai_info->do_times, p->taotai_info->win_times);

	//player's clothes
	memcpy(p->clothes, pkg->clothes, sizeof(p->clothes));

	//debug
	TRACE_LOG("player clothes cnt[%u %u]",p->id, p->clothes_num);
	for (uint32_t i = 0; i < p->clothes_num; i++) {
		TRACE_LOG("clothes[%u %u %u %u]", p->clothes[i].clothes_id, p->clothes[i].unique_id, 
				p->clothes[i].duration, p->clothes[i].lv);
	}

	//summon monster
	if (p->fight_summon && p->fight_summon->mon_tm == pkg->summon_tm) {
		if (p->fight_summon->lv != pkg->summon_lv) {
			p->fight_summon->lv = pkg->summon_lv;
			//calc summon attr info 
			summon_mon_t * p_mon = p->fight_summon;
			const summon_info_t* std_mon = get_summon(p_mon->mon_type);

			attr_data * data = new attr_data;
	   		calc_summon_attr(data, std_mon->attr_type, p_mon->attr_per, p_mon->lv);
			set_summon_attr(p_mon, data);
			set_summon_skills(p_mon, p_mon->mon_type);
			//set_summon_skills_ex(p_mon);
		}

		p->fight_summon->exp = pkg->summon_exp;
		p->fight_summon->fight_value = pkg->fight_value;

		TRACE_LOG("cancel summon[%u %u %u %u %u]",p->id, p->fight_summon->mon_tm, p->fight_summon->lv, 
					p->fight_summon->exp, p->fight_summon->fight_value);
	}

	if (pkg->buf_skill_cnt > 0) {
		unpack_buf_skill_info(p, pkg->buf_skill_cnt, pkg->body);
	} else {
		clear_btl_buff(p);
		TRACE_LOG("CLEAR ALL BUF SKILL");
	}

	if (p->battle_grp->battle_mode == btl_mode_lv_matching) {
		uint32_t uip = get_cli_ip(p->fdsess);
		do_stat_log_pvp_times(uip, 1);
		do_stat_log_pvp_fag(uip, pkg->avg_flg);

        if( p->taotai_info->win_times == 6 
            && p->battle_grp->pvp_btl_type == pvp_monster_game 
            && !is_player_achievement_data_exist(p, 108) ) {
//            do_achieve_type_logic(p, 108, false);
            //add_title_interface(p, enum_achieve_title);
        }

        if (p->taotai_info->win_times == 2 
            && p->battle_grp->pvp_btl_type == pvp_monster_game) {
            db_set_active_flag(p, 0, 2, 0);
        }
	}

    calc_and_save_player_attribute(p);

	leave_battle(p);
	return 0;
}

/**
  * @brief callback for level matching battle
  */
int btlsvr_lv_matching_battle_callback(player_t* p, btl_proto_t* bpkg)
{
	lv_match_rsp_t* pkg = reinterpret_cast<lv_match_rsp_t*>(bpkg->body);
	if (p->id != pkg->first_player) {
		//send_header_to_player(p, p->waitcmd, 0, 1);
        int idx = sizeof(cli_proto_t);
        pack(pkgbuf, pkg->type, idx);
        pack(pkgbuf, pkg->btl_id, idx);
		pack(pkgbuf, p->battle_grp->pvp_btl_type, idx);
        init_cli_proto_head_full(pkgbuf, p->id, 0, p->waitcmd, idx, 0);
        send_to_player(p, pkgbuf, idx, 1);
	}

    //save pos fact room id
    if (pkg->type == pvp_btl_type_foot) {
        if (p->battle_grp->foot_pos > 0 && p->battle_grp->foot_pos < 7) {
            footprint[(p->battle_grp->foot_pos - 1) / 2].room_id = pkg->btl_id;
        }
    }

	if (pkg->first_player) { /* battle started */
		if ((p->id == pkg->first_player) && (p->battle_grp->id == 0)) {
			WARN_LOG("player restarted a new level matching battle: uid=%u", p->id);
			return 0;
		}
		start_battle(p);
	}
	if (pkg->btl_id) {
		p->battle_grp->id = pkg->btl_id & 0x0000ffff;
	} else {
		p->battle_grp->id = bpkg->seq;
	}

	if (pkg->ready) {
        uint32_t pvp_type = get_contest_pvp_type(p->battle_grp->pvp_btl_type);
        if ( pvp_type < 3) {
            save_pvp_game_data(p, pvp_type, false);
        } else {
           // ERROR_LOG(" ERROR INFO uid=[%u] pvp_type=[%u]", p->id, pvp_type);
        }
		//save_advance_game_data(p, false);
	}
	return 0;
}
//------------------------- battle team --------------------
/**
  * @brief callback for create a battle team
  */
int btlsvr_create_btl_team_callback(player_t* p, btl_proto_t* bpkg)
{
	uint32_t btl_mode;
	uint32_t room_id;
	start_battle(p);
	int idx = 0;
	unpack_h(bpkg->body, btl_mode, idx);
	unpack_h(bpkg->body, room_id, idx);
	
	p->battle_grp->id = bpkg->seq;
	KDEBUG_LOG(p->id, "TEAM CREATED\t[uid=%u grpid=%u room_id=%u]", p->id, p->battle_grp->id, room_id);

//	stage_data* pdata_out = 0;
//	operating_activity_end(p, &pdata_out);
	
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, room_id, idx);
	pack(pkgbuf, 2, idx);
	init_cli_proto_head_full(pkgbuf, p->id, 0, p->waitcmd, idx, 0);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief callback for create a battle team
  */
int btlsvr_join_btl_team_callback(player_t* p, btl_proto_t* bpkg)
{
	p->battle_grp->id = bpkg->seq;
	start_battle(p);
	KDEBUG_LOG(p->id, "TEAM JOINED\t[uid=%u grpid=%u]", p->id, p->battle_grp->id);
	
	int idx = sizeof(cli_proto_t);
	idx += pack_join_btl_team_rsp(pkgbuf + idx, p, bpkg);

	init_cli_proto_head_full(pkgbuf, p->id, 0, p->waitcmd, idx, 0);
	return send_to_player(p, pkgbuf, idx, 1);
}

int send_hot_join_btl_team_2_btl(player_t* p, uint32_t room_id, uint32_t stage_id, uint32_t difficulty)
{
	int idx = sizeof(btl_proto_t);
	//room_id = 0xffff;
	pack_h(btlpkgbuf, static_cast<uint8_t>(0), idx);
	pack_h(btlpkgbuf, room_id, idx);
	idx += pack_initiate_battle_info(btlpkgbuf + idx, p, stage_id, difficulty, btl_mode_pve, false);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_hot_join_btl_team);

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int btlsw_auto_join_room_callback(player_t* p, battle_switch_proto_t* bpkg, uint32_t len)
{
	CHECK_VAL_EQ(len - sizeof(battle_switch_proto_t), 16);
	uint32_t btl_id;
	uint32_t room_id;
	uint32_t stage_id;
	uint32_t difficulty;
	int idx = 0;
	unpack_h(bpkg->body, btl_id, idx);
	unpack_h(bpkg->body, room_id, idx);
	unpack_h(bpkg->body, stage_id, idx);
	unpack_h(bpkg->body, difficulty, idx);
	
	int fd_idx = get_btl_fd_idx_by_server_id(btl_id);
	KDEBUG_LOG(p->id, "GET HOT JOIN ID\t[uid=%u btl_id=%u roomid=%08x fdidx=%u]", p->id, btl_id, room_id, fd_idx);
	if (!is_fd_idx_valid(fd_idx)) {
		KDEBUG_LOG(p->id, "team id is invalid:%u %08x %u", p->id, room_id, fd_idx);
		//return send_header_to_player(p, p->waitcmd, cli_err_battle_no_room, 1);
		if (enter_room(p, -1) != 0) {
			return send_header_to_player(p, p->waitcmd, 0, 1);
		}
		room_id = 0;
	} else {
		if (enter_room(p, fd_idx) != 0) {
			return send_header_to_player(p, p->waitcmd, 0, 1);
		}
	}

	return send_hot_join_btl_team_2_btl(p, room_id, stage_id, difficulty);
}

int send_hot_join_pvp_room_2_btl(player_t* p, uint32_t room_id, uint32_t pvp_lv, uint32_t btl_mode)
{
	int idx = sizeof(btl_proto_t);
	//room_id = 0xffff;
	pack_h(btlpkgbuf, static_cast<uint8_t>(0), idx);
	pack_h(btlpkgbuf, room_id, idx);
	//idx += pack_initiate_battle_info(btlpkgbuf + idx, p, get_pvp_stage_id(pvp_lv), 1, btl_mode_lv_matching, false);
	idx += pack_initiate_battle_info(btlpkgbuf + idx, p, pvp_lv, 1, btl_mode, false);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_lv_matching_battle);

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);

}

int send_tmp_team_hot_join_pvp_room_2_btl(player_t* p, uint32_t room_id, uint32_t pvp_lv, uint32_t btl_mode)
{
	int idx = sizeof(btl_proto_t);
	//room_id = 0xffff;
	pack_h(btlpkgbuf, static_cast<uint8_t>(0), idx);
	pack_h(btlpkgbuf, room_id, idx);
	pack_h(btlpkgbuf, get_tmp_team_leader(p)->userid, idx);
	pack_h(btlpkgbuf, p->temporary_team_id, idx);
	pack_h(btlpkgbuf, (uint32_t)(p->m_temporary_team_map->size()), idx);
	pack_h(btlpkgbuf, get_tmp_team_pvp_match_value(p), idx);

	//idx += pack_initiate_battle_info(btlpkgbuf + idx, p, get_pvp_stage_id(pvp_lv), 1, btl_mode_lv_matching, false);
	idx += pack_initiate_battle_info(btlpkgbuf + idx, p, pvp_lv, 1, btl_mode, false);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_player_tmp_team_pvp);

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);

}

/**
  * @brief callback for level matching battle
  */
int btlsvr_player_tmp_team_pvp_callback(player_t* p, btl_proto_t* bpkg)
{
	player_tmp_team_pvp_rsp_t* pkg = reinterpret_cast<player_tmp_team_pvp_rsp_t*>(bpkg->body);

	if (is_player_temporary_team_leader(p)) {
		KDEBUG_LOG(p->id, "tmp team leader join btl\t[%u btlid=%u ]", p->id, pkg->btl_id);
		if (pkg->btl_id) {
			p->battle_grp->id = pkg->btl_id & 0x0000ffff;
		} else {
			p->battle_grp->id = bpkg->seq;
		}
		
		mcast_tmp_team_pvp_rsp(p);
		
		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, p->temporary_team_id, idx);
		pack(pkgbuf, p->battle_grp->id, idx);
		init_cli_proto_head_full(pkgbuf, p->id, 0, p->waitcmd, idx, 0);
		return send_to_player(p, pkgbuf, idx, 1);
	} else {
		KDEBUG_LOG(p->id, "tmp team member join btl\t[%u btlid=%u ]", p->id, pkg->btl_id);
	}
	return 0;
}



int btlsw_auto_join_pvp_room_callback(player_t* p, battle_switch_proto_t* bpkg, uint32_t len)
{
	CHECK_VAL_EQ(len - sizeof(battle_switch_proto_t), 12);
	uint32_t btl_id;
	uint32_t room_id;
	uint32_t pvp_lv;
	battle_mode_t tmp_btl_mode;
	int idx = 0;
	unpack_h(bpkg->body, btl_id, idx);
	unpack_h(bpkg->body, room_id, idx);
	unpack_h(bpkg->body, pvp_lv, idx);
	if (p->waitcmd == cli_proto_tmp_team_pvp_start) {
		tmp_btl_mode = btl_mode_temp_team_pvp;
        if (!(p->temporary_team_id)) {
            p->waitcmd = 0;
            return 0;
        }
	} else {
		tmp_btl_mode = btl_mode_lv_matching;
	}
	int fd_idx = get_btl_fd_idx_by_server_id(btl_id);
	KDEBUG_LOG(p->id, "GET PVP RM ID\t[uid=%u btl_id=%u roomid=%08x pvp_lv=%u fdidx=%u]", p->id, btl_id, room_id, pvp_lv, fd_idx);
	if (!is_fd_idx_valid(fd_idx)) {
		KDEBUG_LOG(p->id, "team id is invalid:%u %08x %u", p->id, room_id, fd_idx);
		//return send_header_to_player(p, p->waitcmd, cli_err_battle_no_room, 1);
		if (enter_room(p, -1, tmp_btl_mode) != 0) {
			return send_header_to_player(p, p->waitcmd, 0, 1);
		}
		room_id = 0;
	} else {
		if (enter_room(p, fd_idx, tmp_btl_mode) != 0) {
			return send_header_to_player(p, p->waitcmd, 0, 1);
		}
	}

    //record pvp_pv
    p->battle_grp->pvp_btl_type = pvp_lv;

	if (tmp_btl_mode != btl_mode_temp_team_pvp) {
		return send_hot_join_pvp_room_2_btl(p, room_id, pvp_lv, tmp_btl_mode);
	} else {
		return send_tmp_team_hot_join_pvp_room_2_btl(p, room_id, pvp_lv, tmp_btl_mode);
	} 
}


/**
  * @brief callback for create a battle team
  */
int btlsvr_hot_join_btl_team_callback(player_t* p, btl_proto_t* bpkg)
{
	p->battle_grp->id = bpkg->seq;
	join_btl_team_rsp_t* p_rsp = (join_btl_team_rsp_t*)(bpkg->body);
	if (p_rsp->room_id) {
		start_battle(p);
		KDEBUG_LOG(p->id, "TEAM JOINED\t[uid=%u grpid=%u]", p->id, p->battle_grp->id);
	} else {
		free_battle_grp(p->battle_grp, 0);
		p->battle_grp = 0;
	}

	int idx = sizeof(cli_proto_t);
	idx += pack_join_btl_team_rsp(pkgbuf + idx, p, bpkg);
	
	init_cli_proto_head_full(pkgbuf, p->id, 0, p->waitcmd, idx, 0);
	return send_to_player(p, pkgbuf, idx, 1);
}


/**
  * @brief callback fuction for setting a battle team hot join flag 
  */
int btlsvr_set_btl_team_hot_join_callback(player_t* p, btl_proto_t* pkg)
{
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

/**
  * @brief callback for setting btl team ready
  */
int btlsvr_ready_btl_team_callback(player_t* p, btl_proto_t* bpkg)
{
	int idx = sizeof(cli_proto_t);
	init_cli_proto_head_full(pkgbuf, p->id, 0, p->waitcmd, idx, 0);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief callback for setting btl team unready 
  */
int btlsvr_cancel_ready_btl_team_callback(player_t* p, btl_proto_t* bpkg)
{
	int idx = sizeof(cli_proto_t);
	init_cli_proto_head_full(pkgbuf, p->id, 0, p->waitcmd, idx, 0);
	return send_to_player(p, pkgbuf, idx, 1);
}


/**
  * @brief callback for create a battle team
  */

int btlsvr_start_btl_team_callback(player_t* p, btl_proto_t* bpkg)
{
	int idx = sizeof(cli_proto_t);
	init_cli_proto_head_full(pkgbuf, p->id, 0, p->waitcmd, idx, 0);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief callback for leave btl team
  */

int btlsvr_leave_btl_team_callback(player_t* p, btl_proto_t* bpkg)
{
	leave_room(p);
	int idx = sizeof(cli_proto_t);
	init_cli_proto_head_full(pkgbuf, p->id, 0, p->waitcmd, idx, 0);
	return send_to_player(p, pkgbuf, idx, 1);
}



/**
  * @brief callback for the team leader kick user from game room
  */
int btlsvr_kick_user_from_room_callback(player_t* p, btl_proto_t* bpkg)
{
	userid_t id = *reinterpret_cast<userid_t*>(bpkg->body);
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, id, idx);
	init_cli_proto_head_full(pkgbuf, p->id, 0, p->waitcmd, idx, 0);
	return send_to_player(p, pkgbuf, idx, 1);
}


/**
  * @brief callback for create a battle team
  */
int btlsvr_nodi_user_be_kicked_callback(player_t* p, btl_proto_t* bpkg)
{
	userid_t id = *reinterpret_cast<userid_t*>(bpkg->body);
	leave_room(p);
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, id, idx);
	init_cli_proto_head_full(pkgbuf, p->id, 0, cli_proto_kick_user_from_room, idx, 0);
	return send_to_player(p, pkgbuf, idx, 0);
}




/**
  * @brief callback for pve battle
  */
int btlsvr_player_btl_restart_callback(player_t* p, btl_proto_t* bpkg)
{
	send_header_to_player(p, p->waitcmd, 0, 1);
	p->battle_grp->id = bpkg->seq;
	KDEBUG_LOG(p->id, "BTL RESTART\t[uid=%u grpid=%u]", p->id, p->battle_grp->id);
	return 0;
}

/**
  * @brief callback for pve battle
  */
int btlsvr_pve_battle_callback(player_t* p, btl_proto_t* bpkg)
{
	int idx = 0;

	stage_data* pdata_out = 0;
	//operating_activity_end(p, &pdata_out);
	TRACE_LOG("%u %p", p->id, pdata_out);
	idx = sizeof(cli_proto_t);
	if (pdata_out) {
		pack(pkgbuf, pdata_out->unlimited_item.count, idx);

		for (uint32_t i = 0; i < pdata_out->unlimited_item.count; i++) {
			pack(pkgbuf, pdata_out->unlimited_item.itemarr[i].itemid, idx);
			pack(pkgbuf, pdata_out->unlimited_item.itemarr[i].count, idx);
		}
	} else {
		pack(pkgbuf, 0, idx);
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);

	start_battle(p);
	p->battle_grp->id = bpkg->seq;
	return 0;
}

/**
  * @brief callback for player_ready_cmd
  */
int btlsvr_player_ready_callback(player_t* p, btl_proto_t* pkg)
{
	player_ready_rsp_t * rsp = reinterpret_cast<player_ready_rsp_t*>(pkg->body);

	//if (rsp->all_ready && p->battle_grp->pvp_btl_type == pvp_contest_advance) {
//		ERROR_LOG("PVP DO TIMES ++ USER %u PLAYER %u ready %u", p->id, rsp->id, rsp->all_ready);
	//	save_advance_game_data(p, false);
	//}
	
	stage_data* pdata_out = 0;
	operating_activity_end(p, &pdata_out);
	if (p->id == rsp->id) {
		int idx = 0;
		idx = sizeof(cli_proto_t);
		if (pdata_out) {
			pack(pkgbuf, pdata_out->unlimited_item.count, idx);

			for (uint32_t i = 0; i < pdata_out->unlimited_item.count; i++) {
				pack(pkgbuf, pdata_out->unlimited_item.itemarr[i].itemid, idx);
				pack(pkgbuf, pdata_out->unlimited_item.itemarr[i].count, idx);
			}
		} else {
			pack(pkgbuf, 0, idx);
		}
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	} else {
		int idx = sizeof(cli_proto_t);
		if (pdata_out) {
			pack(pkgbuf, pdata_out->unlimited_item.count, idx);

			for (uint32_t i = 0; i < pdata_out->unlimited_item.count; i++) {
				pack(pkgbuf, pdata_out->unlimited_item.itemarr[i].itemid, idx);
				pack(pkgbuf, pdata_out->unlimited_item.itemarr[i].count, idx);
			}
		} else {
			pack(pkgbuf, 0, idx);
		}
		init_cli_proto_head_full(pkgbuf, rsp->id, 0, cli_proto_player_ready, idx, 0);
		return send_to_player(p, pkgbuf, idx, 0);
	}
}

/**
  * @brief callback for player_move_cmd
  */
int btlsvr_player_move_callback(player_t* p, btl_proto_t* pkg)
{
	move_rsp_t* rsp = reinterpret_cast<move_rsp_t*>(pkg->body);

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->x, idx);
	pack(pkgbuf, rsp->y, idx);
	pack(pkgbuf, rsp->z, idx);
	pack(pkgbuf, rsp->dir, idx);
	pack(pkgbuf, rsp->mv_type, idx);
	pack(pkgbuf, rsp->lag, idx);
	if (p->id == rsp->uid) {
		init_cli_proto_head(pkgbuf, p, cli_proto_player_move, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	} else {
		init_cli_proto_head_full(pkgbuf, rsp->uid, 0, cli_proto_player_move, idx, 0);
		return send_to_player(p, pkgbuf, idx, 0);
	}
}

/**
  * @brief callback for player_jump_cmd
  */
int btlsvr_player_jump_callback(player_t* p, btl_proto_t* pkg)
{
	jump_rsp_t* rsp = reinterpret_cast<jump_rsp_t*>(pkg->body);

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->x, idx);
	pack(pkgbuf, rsp->y, idx);
	if (p->id == rsp->uid) {
		init_cli_proto_head(pkgbuf, p, cli_proto_jump, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	} else {
		init_cli_proto_head_full(pkgbuf, rsp->uid, 0, cli_proto_jump, idx, 0);
		return send_to_player(p, pkgbuf, idx, 0);
	}
}

/**
  * @brief callback for player_stop_cmd
  */
int btlsvr_player_stop_callback(player_t* p, btl_proto_t* pkg)
{
	stop_rsp_t* rsp = reinterpret_cast<stop_rsp_t*>(pkg->body);

	int len = pack_stand_pkg(pkgbuf, p, rsp->uid, rsp->x, rsp->y, rsp->dir);

	return send_to_player(p, pkgbuf, len, (p->id == rsp->uid));
}

/**
  * @brief callback for player_rsp_lag_cmd
  * @return 0 on success, -1 on error
  */
inline int btlsvr_player_rsp_lag_callback(player_t* p, btl_proto_t* pkg)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, *reinterpret_cast<uint16_t*>(pkg->body), idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);

	return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief callback for player_attack_cmd
  * @return 0 on success, -1 on error
  */
inline int btlsvr_player_attack_callback(player_t* p, btl_proto_t* pkg)
{
	/*
	attack_rsp_t* rsp = reinterpret_cast<attack_rsp_t*>(pkg->body);

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->team, idx);
	pack(pkgbuf, rsp->role_type, idx);
	pack(pkgbuf, rsp->skill_id, idx);
	pack(pkgbuf, rsp->skill_lv, idx);
	pack(pkgbuf, rsp->x, idx);
	pack(pkgbuf, rsp->y, idx);
	pack(pkgbuf, rsp->z, idx);
	//TRACE_LOG("attack:[%u %u %u %u]",rsp->team, rsp->role_type, rsp->skill_id, rsp->skill_lv);
	init_cli_proto_head_full(pkgbuf, rsp->uid, p->seqno, cli_proto_player_attack, idx, 0);

	return send_to_player(p, pkgbuf, idx, (p->id == rsp->uid));
	*/
	return 0;
}

/**
  * @brief callback for player_attack_cmd
  * @return 0 on success, -1 on error
  */
inline int btlsvr_summon_unique_skill_callback(player_t* p, btl_proto_t* pkg)
{
	int idx = sizeof(cli_proto_t);
	
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}
/**
  * @brief callback for player_use_skill_cmd
  * @return 0 on success, -1 on error
  */
/*inline int btlsvr_player_use_skill_callback(player_t* p, btl_proto_t* pkg)
{
	attack_rsp_t* rsp = reinterpret_cast<attack_rsp_t*>(pkg->body);

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->skill_id, idx);
	pack(pkgbuf, rsp->x, idx);
	pack(pkgbuf, rsp->y, idx);
	pack(pkgbuf, rsp->z, idx);
	init_cli_proto_head_full(pkgbuf, rsp->uid, p->seqno, cli_proto_player_use_skill, idx, 0);

	return send_to_player(p, pkgbuf, idx, (p->id == rsp->uid));
}*/

/**
  * @brief callback for player_use_item_cmd
  * @return 0 on success, -1 on error
  */
inline int btlsvr_player_use_item_callback(player_t* p, btl_proto_t* pkg)
{
	uint32_t item_id = *(reinterpret_cast<uint32_t*>(pkg->body));

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, item_id, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);

	send_to_player(p, pkgbuf, idx, 1);
	proc_use_item_achieve_logic(p, item_id, 1, btl_use_item);
	return 0;
}

/**
  * @brief callback for player_pick_item_cmd
  * @return 0 on success, -1 on error
  */
inline int btlsvr_player_pick_item_callback(player_t* p, btl_proto_t* pkg)
{
	pick_item_rsp_t* rsp = reinterpret_cast<pick_item_rsp_t*>(pkg->body);

    //add to player's pack
    if (rsp->pick_flag == 1 && rsp->uid == p->id) {
        if (rsp->unique_id == 0) {
            p->my_packs->add_item(p, rsp->item_id, 1, channel_string_pick, true, monster_drop);
            if (rsp->item_id >= 1500350 && rsp->item_id <= 1500354) {
                do_stat_log_universal_interface_1( 0x09524002, rsp->item_id - 1500350, 1);
            }
            KDEBUG_LOG(p->id, "ACTION REWARD\t[type=%u id=%u cnt=%u channel=%s]", 
                1, rsp->item_id, 1, channel_string_pick);
			if (rsp->item_id == 1500564) {
				send_got_gold_dragon_to_world(p);
			}
        } else {
            p->my_packs->add_clothes(p, rsp->item_id, rsp->unique_id, 0, channel_string_pick);
            KDEBUG_LOG(p->id, "ACTION REWARD\t[type=%u id=%u cnt=%u channel=%s]", 
                2, rsp->item_id, 0, channel_string_pick);
        }
    }

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->pick_flag, idx);
	//pack(pkgbuf, rsp->uid, idx);
	pack(pkgbuf, rsp->item_pick_id, idx);
	pack(pkgbuf, rsp->unique_id, idx);
	pack(pkgbuf, rsp->mon_tm, idx);
	init_cli_proto_head_full(pkgbuf, rsp->uid, p->seqno, cli_proto_player_pick_item, idx, 0);

	return send_to_player(p, pkgbuf, idx, (p->id == rsp->uid));
}

inline int btlsvr_player_enter_map_callback(player_t* p, btl_proto_t* pkg)
{
	entermap_rsp_t* rsp = reinterpret_cast<entermap_rsp_t*>(pkg->body);

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->x, idx);
	pack(pkgbuf, rsp->y, idx);
	pack(pkgbuf, rsp->dir, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);

	return send_to_player(p, pkgbuf, idx, 1);
}

int btlsvr_noti_player_lvup_callback(player_t* p, btl_proto_t* pkg)
{
	lvup_rsp_t* rsp = reinterpret_cast<lvup_rsp_t*>(pkg->body);
	
	if (rsp->id == p->id) {
		p->lv = rsp->lv;
        do_stat_log_role_lv_change(rsp->lv - 1, rsp->lv);
        calc_and_save_player_attribute(p);
		if(p->lv % 5 == 0 && p->lv < 45 && is_finished_task(p, 1231)){
			send_lv_up_mail(p, mail_templet_level_up); 
		}

        if (p->lv == 30 || p->lv == 40) {
            db_set_prentice_grade(p);
        }
					
		TRACE_LOG("set player lv from btl[%u %u %u]",p->id, p->lv, p->skill_point);
		if(p->lv >= min_hero_top_lv)
		{
			db_set_player_hero_top_info(p);
		}
		set_amb_info_after_lv_up(p);

		if (p->lv == 40) { // for invitee magic number
			get_invitee_player(p);
		}
	}
	int idx = sizeof(cli_proto_t);
	idx += pack_player_lvup_noti(pkgbuf + idx, rsp->lv, rsp->hp, rsp->mp);
	init_cli_proto_head_full(pkgbuf, rsp->id, 0, cli_proto_player_lvup_noti, idx, 0);

	return send_to_player(p, pkgbuf, idx, 0);
}

void process_pve_win_btl_over(player_t* p, battle_over_pkg_t* bopkg)
{

 	if(bopkg->stageid == 718 && bopkg->reason == end_btl_normal &&
		g_server_config[5].value)
	{
		send_50_lamp_win_message(p);
	}  

	//for high challenge stage
	if (bopkg->stageid == 954 && bopkg->reason == end_btl_normal) {
		uint32_t now = get_now_tv()->tv_sec;
		uint32_t t_id = bopkg->stageid + bopkg->difficulty * 10000;
		uint32_t cnt = 0;

		if (is_player_kill_boss_data_exist(p, t_id)) {
			kill_boss_data * t_data = get_player_kill_boss_data(p,
					bopkg->stageid, bopkg->difficulty);
			if (t_data) {
				if (t_data->pass_cnt_ < 9) {
					t_data->pass_cnt_++;
				} else {
					t_data->pass_cnt_ = 0;
				}	

				cnt = t_data->pass_cnt_;
			}
		} else {
			add_player_kill_boss_data(p, t_id, 0, now, 1);
			cnt = 1;
		}

		db_update_kill_boss_list(p, t_id, 0,  now, cnt);
	}

	//for true false wuseng
	if (bopkg->stageid == 977 && bopkg->reason == end_btl_normal) {
		//the wuseng active id is 4
		db_save_rank_info(p, 4, bopkg->pass_time);
		//tmp for nine circur dargon stages
	} else if (bopkg->stageid >= 989 && bopkg->stageid <= 997 
			&& bopkg->reason == end_btl_normal) {
		uint32_t have_pass_stage = get_player_other_info_value(p, 6);
		if (have_pass_stage + 989 == bopkg->stageid) {
			have_pass_stage ++;
			set_player_other_info(p, 6, have_pass_stage);
		}
	}

    //if (bopkg->reason == end_btl_normal && p->trigger_event) {
    //    do_event_trigger_logic(p, p->trigger_event, p->trigger_times, end_btl_normal);
    //}

	uint32_t difficulty = bopkg->difficulty;
	uint8_t  stage_grade = bopkg->stage_grade;
	TRACE_LOG("BTL OVE\t :%u %u %u", bopkg->stageid, difficulty, stage_grade);
	if (difficulty <= 6) {
		std::map<uint32_t, fumo_stage_info_t>::iterator it = 
			p->fumo_stage_map->find(bopkg->stageid);
		if (it != p->fumo_stage_map->end()) {
			if (it->second.grade[difficulty - 1] > stage_grade || it->second.grade[difficulty - 1] == 0) {
				it->second.grade[difficulty - 1] = stage_grade;
			}
		} else {
			p->fumo_stage_map->insert(std::map<uint32_t, fumo_stage_info_t>::value_type
				(bopkg->stageid, fumo_stage_info_t(bopkg->stageid, difficulty, stage_grade)));
		}
	}
	
	if (difficulty < 7 && p->battle_grp->btl_type == battle_type_single) {
		db_set_stage_info(p, bopkg->stageid, bopkg->bossid, bopkg->difficulty, 
			bopkg->stage_grade, bopkg->damage);
	}
	// Note : follow pve information

	// ---- (楼兰杯挑战赛)------
	if (difficulty >= 5 && difficulty <= 6) {
		notify_team_active_score_change(p, 1, 1, 1);
	}

	add_title_interface(p, pve_title);

	process_pve_win_achieve_logic(p, bopkg->stageid, p->battle_grp->btl_type, bopkg->stage_grade, difficulty);

	//for achieve hard core
	if (bopkg->use_skill_cnt == 0) {
		//收起刀落
		player_gain_achieve(p, 11);
	}

}
void process_pvp_lose_btl_over(player_t * p, battle_over_pkg_t * bopkg)
{
	if (p->battle_grp->pvp_btl_type == pvp_dragon_ship) {
		player_gain_item(p, give_type_normal_item, 1500608, 1, "activedragonship", false);
	}
}

void process_pvp_win_btl_over(player_t* p, battle_over_pkg_t* bopkg)
{
	if (bopkg->stageid == 939 || bopkg->stageid == 941) {
		//ERROR_LOG("PVP OVER WIN %u %u", p->id, p->battle_grp->pvp_btl_type);
		//save_advance_game_data(p, true);
		uint32_t pvp_type = get_contest_pvp_type(p->battle_grp->pvp_btl_type);
		if ( pvp_type < 3) {
			save_pvp_game_data(p, pvp_type, true);
			if (bopkg->stageid == 941) {
				notify_team_active_score_change(p, 2, 1, 3);
			}
		} else {
			ERROR_LOG(" ERROR INFO uid=[%u] pvp_type=[%u]", p->id, pvp_type);
		}

		if (p->taotai_info->win_times >= 6 &&  pvp_type == 0) {
//			if ( !is_player_achievement_data_exist(p, 108) )
//				do_achieve_type_logic(p, 108);
			do_special_title_logic(p, 101);
		}	
		if (p->taotai_info->win_times == 2 
			&& p->battle_grp->pvp_btl_type == pvp_monster_game ) {
			db_set_active_flag(p, 0, 2, 0);
		}
	} else {	
		p->cur_continue_win ++;
		p->win_times ++;
		if (p->max_conti_win_times < p->cur_continue_win) {
			p->max_conti_win_times = p->cur_continue_win ;
		}
	}

	if (p->battle_grp->pvp_btl_type == pvp_dragon_ship) {
		player_gain_item(p, give_type_normal_item, 1500608, 3, "activedragonship", false);
	}

	proc_pvp_win_achieve_logic(p, p->battle_grp->pvp_btl_type, bopkg->stage_grade);
	//for achieve hard core
	if (bopkg->use_skill_cnt == 0) {
		//小菜一碟 107
		player_gain_achieve(p, 107);
	}

	if (bopkg->be_attacked_cnt == 0) {
		//毫发位伤
		player_gain_achieve(p, 110);
	}

}

void process_tmp_pvp_win_btl_over(player_t* p, battle_over_pkg_t* bopkg)
{
	if (is_player_have_swap_action_times(p, 1450) || is_in_holiday()) {
		uint32_t left_cnt = get_player_swap_action_left_times(p, 1450);
		uint32_t add_cnt = left_cnt > 3 ? 3 : left_cnt;
	    if (is_in_holiday()) {
            add_cnt = 3;
        }
		uint32_t tmp_team_pvp_score = get_player_other_info_value(p, rank_top_type_tmp_team_score);
		KDEBUG_LOG(p->id, "have tmp team pvp score %u", tmp_team_pvp_score);
		set_player_other_info(p, rank_top_type_tmp_team_score, tmp_team_pvp_score + add_cnt);
		db_save_rank_info(p, 11, tmp_team_pvp_score + add_cnt);

		add_swap_action_times(p, 1450, add_cnt);
	}

	proc_pvp_win_achieve_logic(p, p->battle_grp->pvp_btl_type, bopkg->stage_grade);
}

void process_tmp_pvp_lose_btl_over(player_t* p, battle_over_pkg_t* bopkg)
{
	if (is_player_have_swap_action_times(p, 1450) || is_in_holiday()) {
		uint32_t tmp_team_pvp_score = get_player_other_info_value(p, rank_top_type_tmp_team_score);
		KDEBUG_LOG(p->id, "have tmp team pvp score %u", tmp_team_pvp_score);
		set_player_other_info(p, rank_top_type_tmp_team_score, tmp_team_pvp_score + 1);
		db_save_rank_info(p, 11, tmp_team_pvp_score + 1);

		add_swap_action_times(p, 1450, 1);
	}
}

void on_pve_all_player_joined_ready_to_start(player_t* p)
{
}

void on_tmp_team_pvp_all_player_joined_ready_to_start(player_t* p)
{
	start_battle(p);
}

void on_lv_match_all_player_joined_ready_to_start(player_t* p)
{	
	if (p->battle_grp->pvp_btl_type == pvp_eve_of_16_fight_1) 
	{
	//			ERROR_LOG("reduce exploit %u %u", p->id, p->exploit);		reduce_exploit(p, 400);	
	} else if (p->battle_grp->pvp_btl_type == pvp_eve_of_16_fight_2) {
	//			ERROR_LOG("reduce exploit %u %u", p->id, p->exploit);		reduce_exploit(p, 600);	
	} else if (p->battle_grp->pvp_btl_type == pvp_eve_of_16_fight_3) {
	//			ERROR_LOG("reduce exploit %u %u", p->id, p->exploit);		reduce_exploit(p, 800);	
	} else if (p->battle_grp->pvp_btl_type == pvp_eve_of_16_fight_4) {
	//			ERROR_LOG("reduce exploit %u %u", p->id, p->exploit);		reduce_exploit(p, 1000);	
	} else if (p->battle_grp->pvp_btl_type == pvp_team_score) {		
		add_swap_action_times(p, 1419);	
	}
}

int btlsvr_noti_online_all_player_joined_ready_to_start_callback(player_t* p, btl_proto_t* pkg)
{	
	//处理所有人已经进入战斗，准备开始加载资源前的逻辑，扣钱什么的统一在这里做处理	
	noti_online_all_player_joined_ready_to_start_t* bopkg = 		
		reinterpret_cast<noti_online_all_player_joined_ready_to_start_t*>(pkg->body);	
	//设置开始，人物离开地图    
	if (bopkg->btl_mode == btl_mode_lv_matching) {		
		on_lv_match_all_player_joined_ready_to_start(p);	
	} else if (bopkg->btl_mode == btl_mode_pve) {		
		on_pve_all_player_joined_ready_to_start(p);	
	} else if (bopkg->btl_mode == btl_mode_temp_team_pvp) {
		on_tmp_team_pvp_all_player_joined_ready_to_start(p);
	}
	return 0;
}


/**
  * @brief battle over, sync player's data and give bonus to the player
  */
int btlsvr_battle_over_callback(player_t* p, btl_proto_t* pkg)
{
	battle_over_pkg_t* bopkg = reinterpret_cast<battle_over_pkg_t*>(pkg->body);
	
	// send end battle notification
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, bopkg->winner, idx);
	pack(pkgbuf, bopkg->reason, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_battle_end, idx);
	send_to_player(p, pkgbuf, idx, 0);

    p->battle_grp->end_reason = bopkg->reason;

	if (p->battle_grp->pvp_btl_type >= pvp_red_blue_1 &&
		   	p->battle_grp->pvp_btl_type <= pvp_red_blue_3 ) {
		if (p->id == bopkg->winner) {
			proc_player_banner_logic_after_btl_over(p, 1);
		} else {
			proc_player_banner_logic_after_btl_over(p, 0);
		}
	}
	uint32_t cur_map_id = get_mapid_low32(p->cur_map->id);
	if (cur_map_id >= 1004 && cur_map_id < 1013) {
		if (p->id == bopkg->winner) {
			player_leave_team_contest_btl(p, 1);
		} else {
			player_leave_team_contest_btl(p, 0);
		}
	}

//	if (p->team_info.team_id) {
//		add_player_team_active_score(p, 1, 1);
//	}
//

	if (p->id == bopkg->winner) {
		if (bopkg->stageid > max_stage_num) {
			WARN_LOG("stage num more than max[%u %u %u]",p->id, bopkg->stageid, max_stage_num);
		} else {
			if (p->battle_grp && p->battle_grp->battle_mode == btl_mode_pve) {
				process_pve_win_btl_over(p, bopkg);	
			}
			
			if (p->battle_grp && p->battle_grp->battle_mode == btl_mode_lv_matching) {
				process_pvp_win_btl_over(p, bopkg);
			}

			if (p->battle_grp && p->battle_grp->battle_mode == btl_mode_temp_team_pvp) {
				process_tmp_pvp_win_btl_over(p, bopkg);
			}

		}
		update_task_step_info_by_pass_stage(p, bopkg->stageid);
	} else {
	//lose
		if (p->battle_grp && 
			(p->battle_grp->battle_mode == btl_mode_temp_team_pvp || p->battle_grp->battle_mode == btl_mode_lv_matching)) {
			p->cur_continue_win = 0;
			p->lose_times ++;
		}
		if (p->battle_grp && p->battle_grp->battle_mode == btl_mode_temp_team_pvp) {
			process_tmp_pvp_lose_btl_over(p, bopkg);
		} else {
			process_pvp_lose_btl_over(p, bopkg);
		}
	}

	if (p->battle_grp->pvp_btl_type == pvp_team_score) {
		player_add_team_exp_by_pvp(p, p->cur_continue_win);
	}
	return 0;
}

/**
  * @brief battle over, sync player's data and give bonus to the player
  */
int btlsvr_player_open_treasure_box_callback(player_t* p, btl_proto_t* pkg)
{
	open_treasure_box_rsp_t* rsp = reinterpret_cast<open_treasure_box_rsp_t*>(pkg->body);

	CHECK_VAL_EQ(pkg->len - sizeof(btl_proto_t), sizeof(open_treasure_box_rsp_t) + rsp->box_cnt * sizeof(box_item_info_t));
	TRACE_LOG("[%u %d %d %d]",p->id, rsp->box_type, rsp->box_id, rsp->box_cnt);

	// send to player
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, static_cast<uint8_t>(rsp->box_type), idx);
	pack(pkgbuf, static_cast<uint8_t>(rsp->box_id), idx);
	pack(pkgbuf, static_cast<uint16_t>(rsp->box_cnt), idx);
	TRACE_LOG("[%u %u %u %u]",p->id, rsp->box_type, rsp->box_id, rsp->box_cnt);
	for (uint32_t i = 0; i < rsp->box_cnt; i++) {
		box_item_info_t* p_info = &(rsp->boxs[i]);
		//add to pack
		if (p_info->get_flag == 1) {
			if (p_info->item_type == give_type_clothes) {
				p->my_packs->add_clothes(p, p_info->item_id, p_info->unique_index, p_info->item_duration, channel_string_box);
			} else if (p_info->item_type == give_type_normal_item) {
				p->my_packs->add_item(p, p_info->item_id, p_info->item_cnt, channel_string_box, true, monster_drop);
			}
		}
		if (rsp->box_type == 1) {
		//normal box
			do_stat_log_universal_interface_1(stat_log_get_nor_box, 0, 1);
		} else {
		// vip box
			do_stat_log_universal_interface_1(stat_log_get_vip_box, 0, 1);
		}
		pack(pkgbuf, static_cast<uint16_t>(p_info->quality), idx);
		pack(pkgbuf, static_cast<uint8_t>(p_info->get_flag), idx);
		pack(pkgbuf, static_cast<uint32_t>(p_info->item_id), idx);
		pack(pkgbuf, static_cast<uint32_t>(p_info->item_cnt), idx);
		pack(pkgbuf, static_cast<uint32_t>(p_info->item_duration), idx);
		pack(pkgbuf, static_cast<uint32_t>(p_info->unique_index), idx);
		TRACE_LOG("[%u %u %u %d]",p_info->quality, p_info->get_flag, p_info->item_id, p_info->item_cnt);

		KDEBUG_LOG(p->id, "ACTION REWARD\t[type=%u id=%u cnt=%u channel=%s]", 
				p_info->item_type, p_info->item_id, p_info->item_cnt, channel_string_box);
	}

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


//for test
int btlsvr_get_player_attr_callback(player_t* p, btl_proto_t* pkg)
{
	int idx = sizeof(btl_proto_t);
	send_to_player(p, pkg, pkg->len - idx, 1);

	TRACE_LOG("get player attr callback");
	return 0;
}

int btlsvr_get_mon_attr_callback(player_t* p, btl_proto_t* pkg)
{
	int idx = sizeof(btl_proto_t);
	send_to_player(p, pkg, pkg->len - idx, 1);

	TRACE_LOG("get mon attr callback");
	return 0;
}



struct yazhu_rsp_t {
	uint32_t btl_mode;
	uint32_t room_status;
	uint32_t uid; //观看者
	uint32_t yazhu_id;//押注对象
}__attribute__((packed));

int btlsvr_contest_watch_yazhu_callback(player_t * p, btl_proto_t * pkg)
{
	yazhu_rsp_t * rsp = reinterpret_cast<yazhu_rsp_t*>(pkg->body);
	if (rsp->room_status && rsp->yazhu_id == p->id ) { //有人押注你了
		save_pvp_game_flower(p, 2, 1);
	}

	if (rsp->uid == p->id) { //发给押注本人
		if (rsp->room_status) { //押注成功
			p->battle_grp->id = p->watch_info->room_id & 0x0000ffff;
			db_contest_yazhu(p);//扣除物品
			p->watch_info->room_status = rsp->room_status;
			int idx = sizeof(cli_proto_t);
			pack(pkgbuf, rsp->room_status, idx);
			pack(pkgbuf, rsp->uid, idx);
			pack(pkgbuf, rsp->yazhu_id, idx);
			init_cli_proto_head(pkgbuf, p, cli_proto_contest_pvp_yazhu, idx);
			return send_to_player(p, pkgbuf, idx, 1);
		} else { //押注失败
			p->waitcmd = 0;
			pvp_watch_data_t * watch_info = p->watch_info;
			if (watch_info) {
				destory_watch_info(p, watch_info);
				p->watch_info = 0;
			}

			if (p->battle_grp) {
				free_battle_grp(p->battle_grp, p);
				p->battle_grp = 0;
			}
		}
	} 

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->room_status, idx);
	pack(pkgbuf, rsp->uid, idx);
	pack(pkgbuf, rsp->yazhu_id, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_contest_pvp_yazhu, idx);
	return  send_to_player(p, pkgbuf, idx, 0);
}

int btlsvr_contest_cancel_yazhu_callback(player_t *p, btl_proto_t *pkg)
{
	int idx = sizeof(cli_proto_t);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);

	pvp_watch_data_t * info = p->watch_info;
	if (info) {
		destory_watch_info(p, info);
		p->watch_info = 0;
	}

	if (p->battle_grp) {
		free_battle_grp(p->battle_grp, p);
		p->battle_grp = 0;
	}

	return 0;
}

int btlsvr_contest_watch_over_callback(player_t *p, btl_proto_t * pkg)
{
	uint32_t winner = 0;
	int idx = 0;
	unpack_h(pkg->body, winner, idx);

	if (p->watch_info && p->watch_info->watch_id == winner) { //押注正确
		uint32_t type = 2;
		uint32_t add_num = 1; 

		int ifx = 0;
		uint32_t item_id = 1500364;
		uint32_t max_bag_grid_count = get_player_total_item_bag_grid_count(p); 
		if (!p->my_packs->check_enough_bag_item_grid(p, item_id, add_num)) {
			//物品包裹满 通过邮件插送
			char title[MAX_MAIL_TITLE_LEN + 1] = {0};
			char content[MAX_MAIL_CONTENT_LEN + 1] = {0};
			std::vector<mail_item_enclosure> item_vec;
			strncpy(title, char_content[1].content, sizeof(title) - 1);
			strcpy(content, char_content[2].content);
			//ERROR_LOG("mail: %s ===== %s", title, content);
			item_vec.push_back(mail_item_enclosure(item_id, add_num));
			db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_normal, 0, &item_vec, 0);
			item_vec.clear();
		} else {
			pack_h(dbpkgbuf, type, ifx);
			pack_h(dbpkgbuf, item_id, ifx);
			pack_h(dbpkgbuf, add_num, ifx);
			pack_h(dbpkgbuf, max_bag_grid_count, ifx);
			send_request_to_db(p, p->id, p->role_tm,
	   		dbproto_add_item, dbpkgbuf, ifx);
		}
	}

	pvp_watch_data_t * info = p->watch_info;
	if (info) {
		destory_watch_info(p, info);
		p->watch_info = 0;
	}

	if (p->battle_grp) {
		free_battle_grp(p->battle_grp, p);
		p->battle_grp = 0;
	}

	idx = sizeof(cli_proto_t);
	pack(pkgbuf, winner, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_contest_over_for_watch, idx);
	return  send_to_player(p, pkgbuf, idx, 0);
}

int btlsvr_extra_item_drop_proc_callback(player_t * p, btl_proto_t * pkg)
{
	struct extra_item_drop_rsp_t {
		uint32_t item_id;
	} __attribute__((packed)); 

	extra_item_drop_rsp_t *rsp = reinterpret_cast<extra_item_drop_rsp_t*>(pkg->body);	
	const GfItem * item = items->get_item(rsp->item_id);

	if (!item) {
			return 0;
	}

	uint32_t max_bag_grid_count = get_player_total_item_bag_grid_count(p);
	if (!p->my_packs->check_enough_bag_item_grid(p, rsp->item_id, 1)){
		//物品包裹满 通过邮件插送
		char title[MAX_MAIL_TITLE_LEN + 1] = {0};
		char content[MAX_MAIL_CONTENT_LEN + 1] = {0};
		std::vector<mail_item_enclosure> item_vec;
		strncpy(title, char_content[0].content, sizeof(title) - 1);
		strncpy(content, char_content[1].content, sizeof(content) - 1);
		//ERROR_LOG("mail: %s ===== %s", title, content);
		item_vec.push_back(mail_item_enclosure(rsp->item_id, 1));
		db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_normal, 0, &item_vec, 0);
		item_vec.clear();

	} else {
		int idx = 0;
		int add_num = 1;
		int type = 2;
		pack_h(dbpkgbuf, type, idx);
		pack_h(dbpkgbuf, rsp->item_id, idx);
		pack_h(dbpkgbuf, add_num, idx);
		pack_h(dbpkgbuf, max_bag_grid_count, idx);
		send_request_to_db(p, p->id, p->role_tm, dbproto_add_item, dbpkgbuf, idx);
	}

	return 0;
}


/**
  * @brief battle over, player revive
  */
/*int btlsvr_player_revive_callback(player_t* p, btl_proto_t* pkg)
{
	int idx = sizeof(cli_proto_t);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);

	return 0;
}*/

/**
  * @brief transmit packages from battle server to a player
  */
int btlsvr_transmit_only_callback(player_t* p, btl_proto_t* pkg)
{
	talk_msg_out_head_t* p_head = (talk_msg_out_head_t*)(pkg->body);
	TRACE_LOG("%u %u %s %u %u %u", taomee::bswap(p_head->from_id), taomee::bswap(p_head->from_tm), p_head->from_nick, taomee::bswap(p_head->to_id), taomee::bswap(p_head->type), taomee::bswap(p_head->msg_len));

	return send_to_player(p, pkg->body, pkg->len - sizeof(btl_proto_t), 0);
}

//-------------------------------------------------------------------

/**
  * @brief send a request to battle svr
  * @param p the player who launches the request to battle server
  * @param btlpkgbuf body of the request
  * @param len length of btlpkgbuf
  * @return 0 on success, -1 on error
  */
int send_to_battle_svr(player_t* p, const void* btlpkgbuf, uint32_t len, int index)
{
	if (battle_fds[index] == -1) {
		battle_fds[index] = connect_to_service(battle_svrs[index].svr_name, battle_svrs[index].id, 65535, 1); 
	}

	if ((battle_fds[index] == -1) || (len > btlproto_max_len)) {
		ERROR_LOG("send to battle server failed: fd=%d len=%d", battle_fds[index], len);

		assert(p->battle_grp);
	
		battle_grp_t* grp = p->battle_grp;
		if (is_battle_started(grp)) {
			leave_battle(p, 1);
		} else {
			free_battle_grp(grp, p);
			p->battle_grp = 0;
		}
		return send_header_to_player(p, cli_proto_battle_end, cli_err_battle_system_err, 1);
	}
	btl_proto_t* p_btl_head = (btl_proto_t*)btlpkgbuf;
	TRACE_LOG("fd=%d p=%u len=%d, cmd=%u", battle_fds[index], p ? p->id: 0, len, p_btl_head->cmd);

    return net_send(battle_fds[index], btlpkgbuf, len);
}

int btlsvr_summon_ai_change_callback(player_t *p, btl_proto_t * pkg)
{
	uint32_t type = 0;
	int idx = 0;
	unpack_h(pkg->body, type, idx);

	idx = sizeof(cli_proto_t);
	pack(pkgbuf, type, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1); 
}	

int btlsvr_player_detect_player_need_kick_callback(player_t *p, btl_proto_t * pkg)
{
	if (g_server_config[1].value) {
		WARN_LOG("%u be kicked", p->id);
		send_header_to_player(p, cli_proto_check_cheat, 0, 0);
		del_player(p);
	} else {
		WARN_LOG("%u not kicked", p->id);
	}
	return 0;
}

/**
  * @brief set battle server protocol handle
  */
#define SET_BTL_HANDLE(op_, len_, cmp_) \
		do { \
			if (btl_handles[btl_ ## op_ - btl_cmd_start].hdlr != 0) { \
				ERROR_LOG("duplicate cmd=%u name=%s", btl_ ## op_, #op_); \
				return false; \
			} \
			btl_handles[btl_ ## op_ - btl_cmd_start] = btlsvr_handle_t(btlsvr_ ## op_ ## _callback, len_, cmp_); \
		} while (0)

/**
  * @brief init handles to handle protocol packages from battle server
  * @return true if all handles are inited successfully, false otherwise
  */
bool init_btl_proto_handles()
{
	memset(btl_handles, 0, sizeof(btl_handles));
	/* operation, bodylen, compare method */
	SET_BTL_HANDLE(cancel_battle, sizeof(cancel_btl_rsp_t), cmp_must_ge);
	SET_BTL_HANDLE(lv_matching_battle, sizeof(lv_match_rsp_t), cmp_must_eq);
	SET_BTL_HANDLE(pve_battle, 4, cmp_must_eq);
	SET_BTL_HANDLE(create_btl_team, 8, cmp_must_eq);
	SET_BTL_HANDLE(join_btl_team, 12, cmp_must_ge);
	SET_BTL_HANDLE(hot_join_btl_team, 12, cmp_must_ge);
	SET_BTL_HANDLE(ready_btl_team, 0, cmp_must_eq);
	SET_BTL_HANDLE(start_btl_team, 0, cmp_must_eq);
	SET_BTL_HANDLE(leave_btl_team, 0, cmp_must_eq);
	SET_BTL_HANDLE(cancel_ready_btl_team, 0, cmp_must_eq);
	SET_BTL_HANDLE(nodi_user_be_kicked, 4, cmp_must_eq);
	SET_BTL_HANDLE(set_btl_team_hot_join, 0, cmp_must_eq);
	SET_BTL_HANDLE(kick_user_from_room, 4, cmp_must_eq);
	
	SET_BTL_HANDLE(player_btl_restart, 4, cmp_must_eq);
	SET_BTL_HANDLE(player_ready, sizeof(player_ready_rsp_t), cmp_must_eq);

	SET_BTL_HANDLE(player_move, sizeof(move_rsp_t), cmp_must_eq);
	SET_BTL_HANDLE(player_jump, sizeof(jump_rsp_t), cmp_must_eq);
	SET_BTL_HANDLE(player_stop, sizeof(stop_rsp_t), cmp_must_eq);
	SET_BTL_HANDLE(player_rsp_lag, 2, cmp_must_eq);
	SET_BTL_HANDLE(player_attack, sizeof(attack_rsp_t), cmp_must_eq);
	SET_BTL_HANDLE(summon_unique_skill, 0, cmp_must_eq);
	SET_BTL_HANDLE(player_use_item, 4, cmp_must_eq);
	SET_BTL_HANDLE(player_pick_item, sizeof(pick_item_rsp_t), cmp_must_eq);
	SET_BTL_HANDLE(player_enter_map, sizeof(entermap_rsp_t), cmp_must_eq);
	
	SET_BTL_HANDLE(player_clothes_duration, sizeof(clothes_duration_rsp_t), cmp_must_ge);
	SET_BTL_HANDLE(noti_player_lvup, 4, cmp_must_ge);

	SET_BTL_HANDLE(noti_online_all_player_joined_ready_to_start, sizeof(noti_online_all_player_joined_ready_to_start_t), cmp_must_eq);
	SET_BTL_HANDLE(battle_over, sizeof(battle_over_pkg_t), cmp_must_eq);
	SET_BTL_HANDLE(player_open_treasure_box, sizeof(open_treasure_box_rsp_t), cmp_must_ge);

	SET_BTL_HANDLE(player_roll_item, sizeof(player_roll_item_rsp_t), cmp_must_eq);
	SET_BTL_HANDLE(player_get_roll_item, sizeof(player_get_roll_item_rsp_t), cmp_must_eq);
	//for test
	SET_BTL_HANDLE(get_player_attr, sizeof(cli_proto_t), cmp_must_ge);
	SET_BTL_HANDLE(get_mon_attr, sizeof(cli_proto_t), cmp_must_ge);
	SET_BTL_HANDLE(change_mechanism, 12, cmp_must_eq); 
	SET_BTL_HANDLE(extra_item_drop_proc, 4, cmp_must_eq);
	SET_BTL_HANDLE(first_kill_boss, 12, cmp_must_eq);

	//for task
	SET_BTL_HANDLE(kill_monster, 8, cmp_must_eq);

	SET_BTL_HANDLE(contest_watch_yazhu, 16, cmp_must_eq);
	SET_BTL_HANDLE(contest_watch_over,  4, cmp_must_eq);
	SET_BTL_HANDLE(contest_cancel_yazhu, 0, cmp_must_eq);
    SET_BTL_HANDLE(auto_del_item, 4, cmp_must_ge);
	SET_BTL_HANDLE(open_box_times, 4, cmp_must_eq);
	SET_BTL_HANDLE(summon_ai_change, 4, cmp_must_eq);
	SET_BTL_HANDLE(player_detect_player_need_kick, 0, cmp_must_eq);

	SET_BTL_HANDLE(player_tmp_team_pvp, 8, cmp_must_eq);
	
	SET_BTL_HANDLE(transmit_only, sizeof(cli_proto_t), cmp_must_ge);

	return true;
}

/**
  * @brief uniform handler to process packages from battle server
  * @param data package data
  * @param len package length
  * @param bs_fd battle server fd
  */
void handle_battle_svr_return(btl_proto_t* data, uint32_t len, int bs_fd)
{
	if ((data->cmd > btl_player_jump) && (data->cmd < btl_player_move)) {
		KDEBUG_LOG(0, "BS\t[u=%u c=%u g=%u r=%u fd=%d]",
					data->id, data->cmd, data->seq, data->ret, bs_fd);
	}

	player_t* p = get_player(data->id);

	if ((p == 0) || (p->battle_grp == 0) || (battle_fds[p->battle_grp->fd_idx] != bs_fd)
			|| ((p->battle_grp->id != data->seq) && (!is_valid_fight_cmd(data->cmd) || (p->battle_grp->id != 0)))) {
		ERROR_LOG("battle ended already: uid=%u grpid=%u %u cmd=%u fd=%d %d",
				data->id, (p && p->battle_grp) ? p->battle_grp->id : 0, data->seq, data->cmd, (p && p->battle_grp) ? battle_fds[p->battle_grp->fd_idx] : 0, bs_fd);
		return;
	}
	if (data->ret) {
		// some error code needs to be handled specially
		if (data->ret < btl_err_max) {
			KDEBUG_LOG(p->id, "BATTLE SERVER ERROR\t[uid=%u bscmd=%u waitcmd=%u ret=%u]",
						p->id, data->cmd, p->waitcmd, data->ret);

			send_header_to_player(p, p->waitcmd, cli_err_btl_not_exist, 1);

			battle_grp_t* grp = p->battle_grp;
			if (is_battle_started(grp)) {
				leave_battle(p, 1);
			} else {
				free_battle_grp(grp, p);
				p->battle_grp = 0;
			}
		} else {
			if (!is_battle_started(p->battle_grp)) {
				free_battle_grp(p->battle_grp, p);
				p->battle_grp = 0;
			}
			send_header_to_player(p, p->waitcmd, data->ret, 1);
		}
		return;
	}

	int err = -1;
	int cmd = data->cmd - btl_cmd_start;
	uint32_t bodylen = data->len - sizeof(btl_proto_t);

	if (is_valid_fight_cmd(data->cmd)) {
		// TODO: invitee
		if (len >= (sizeof(btl_proto_t) + 4)) {
			//userid_t invitee = *reinterpret_cast<userid_t*>(data->body);
			//uint32_t battle_mode = *reinterpret_cast<userid_t*>(data->body + 4);
			uint32_t battle_mode = *reinterpret_cast<uint32_t*>(data->body);
			if (/*(invitee != p->battle_grp->invitee)
					|| */(battle_mode != p->battle_grp->battle_mode && battle_mode != 3)) {
				ERROR_LOG("battle ended already: uid=%u grpid=%u cmd=%u %u %u",
							data->id, data->seq, data->cmd, battle_mode, p->battle_grp->battle_mode);
				// TODO: tell battle server to end battle. need or needn't?
				return;
			}
		} else {
			ERROR_LOG("invalid package len=%u cmd=%u ret=%u uid=%u",
						len, data->cmd, data->ret, data->id);
			goto ret;
		}
	}

	if ( ((btl_handles[cmd].cmp_method == cmp_must_eq) && (bodylen != btl_handles[cmd].len))
			|| ((btl_handles[cmd].cmp_method == cmp_must_ge) && (bodylen < btl_handles[cmd].len)) ) {
		if ((bodylen != 0) || (data->ret == 0)) {
			ERROR_LOG("invalid package len=%u %u cmpmethod=%d ret=%u uid=%u %u",
						bodylen, btl_handles[cmd].len,
						btl_handles[cmd].cmp_method, data->ret, data->id, cmd);
			goto ret;
		}
	}

	err = btl_handles[cmd].hdlr(p, data);

ret:
	if (err) {
		close_client_conn(p->fd);
	}
}

//-------------------------------------------------------------------
/**
 * @brief load battle servers' config from btlsvr.xml file
 * @param cur the pointer to xml root node
 * @return 0 on success, -1 on error
 */
int load_battle_servers(xmlNodePtr cur)
{
	battle_svr_cnt = 0;
	memset(battle_svrs, 0, sizeof(battle_svrs));

	cur = cur->xmlChildrenNode; /* goto <Server> */
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Server"))) {
			if (battle_svr_cnt >= max_battle_svr_num) {
				ERROR_LOG("Too many battle servers: cnt=%d", battle_svr_cnt);
				return -1;
			}

			string svr_name;
			get_xml_prop(svr_name, cur, "SvrName");
			if (svr_name.length() >= 16) {
				ERROR_LOG("Invalid service name: %s idx=%d",
							svr_name.c_str(), battle_svr_cnt);
				return -1;
			}

			int16_t	svr_id;
			get_xml_prop(svr_id, cur, "SvrId");
			if (svr_id == 0) {
				ERROR_LOG("Invalid svr_id: %d idx=%d", svr_id, battle_svr_cnt);
				return -1;
			}
			strncpy(battle_svrs[battle_svr_cnt].svr_name, svr_name.c_str(), sizeof(battle_svrs[battle_svr_cnt].svr_name));
			battle_svrs[battle_svr_cnt].id = svr_id;

			++battle_svr_cnt;
		}
		cur = cur->next;
	}

	return 0;
}

/**
 * @brief reload battle svr config 
 */
void reload_battle_svr_config()
{
	battle_server_t former_svrs[max_battle_svr_num];
	memcpy(former_svrs, battle_svrs, sizeof(battle_svrs));

	try {
		load_xmlconf("./conf/btlsvr.xml", load_battle_servers);
	} catch (const exception& e) {
		INFO_LOG("Fail to Reload 'btlsvr.xml'. Err: %s", e.what());
		return;
	}

	for (size_t i = 0; i < array_elem_num(battle_fds); ++i) {
		if ( strcmp(former_svrs[i].svr_name, battle_svrs[i].svr_name)
				|| (former_svrs[i].id != battle_svrs[i].id) ) {
			KDEBUG_LOG(0, "RELOAD BTL SVR\t[idx=%llu name=%s %s id=%d %d]",
						static_cast<unsigned long long>(i),
						battle_svrs[i].svr_name, former_svrs[i].svr_name,
						battle_svrs[i].id, former_svrs[i].id);
			close(battle_fds[i]);
			battle_fds[i] = -1;
		}
	}
}

/**
  * @brief get index of a battle server identified by the given 'fd'
  * @param fd fd of a battle server
  * @return index of a battle server, or -1 if battle server identified by 'fd' is not found
  */
int get_btlsvr_fd_idx(int fd)
{
	for (int i = 0; i < battle_svr_cnt; ++i) {
		if (fd == battle_fds[i]) {
			return i;
		}
	}

	return -1;
}

uint32_t get_btl_fd_idx_by_room_id(uint32_t room_id)
{
	uint16_t btl_id = (room_id >> 16 & 0xffff);
	for (int i = 0; i < battle_svr_cnt; ++i) {
		if (battle_svrs[i].id == btl_id) {
			return i;
		}
	}
	return  0xffffffff;
}


int get_btl_fd_idx_by_server_id(uint16_t id)
{
	for (int i = 0; i < battle_svr_cnt; ++i) {
		if (battle_svrs[i].id == id) {
			return i;
		}
	}
	return  -1;
}

/**
  * @brief if user can in special stage
  * @return true can, false cannot
  */
bool can_user_in_special_stages(player_t* p, uint32_t stage_id, uint32_t diff)
{
	std::map<uint32_t, struct specialstages_t*>::iterator iter = special_stages_map_.find(stage_id);
	if (iter == special_stages_map_.end()) {
		return false;
	}
	if (iter->second->task_id) {
		if (p->going_tasks_map->find(iter->second->task_id) == p->going_tasks_map->end()) {
			return false;
		}
	}
	return true;
}

/**
  * @brief load special stages from the xml file
  * @return 0 on success, -1 on error
  */
int load_special_stages(xmlNodePtr cur)
{
	// load items from an xml file
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Stage"))) {
			struct specialstages_t* p_stage = reinterpret_cast<struct specialstages_t*>(malloc(sizeof(struct specialstages_t)));
			memset(p_stage, 0x00, sizeof(struct specialstages_t));
			get_xml_prop(p_stage->stage_id, cur, "ID");
			get_xml_prop(p_stage->task_id, cur, "Task");
			special_stages_map_.insert(std::map<uint32_t, struct specialstages_t*>::value_type(p_stage->stage_id, p_stage));
		}
		cur = cur->next;
	}
	//dump...
	for (std::map<uint32_t, struct specialstages_t*>::iterator iter = special_stages_map_.begin(); 
		iter != special_stages_map_.end(); ++iter) {
		TRACE_LOG("stage id [%u]  task id [%u]", iter->second->stage_id, iter->second->task_id);
	}
	return 0;
}

/**
  * @brief load special stages from the xml file
  * @return 0 on success, -1 on error
  */
  /*
int load_stage_activity_limit_stages(xmlNodePtr cur)
{
	// load items from an xml file
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Time"))) {
			uint32_t stage_id = 0;
			get_xml_prop(stage_id, cur, "StageID");
			struct stage_activity_limit_t* p_stage = &(stage_activety_limit[stage_id]);
			memset(p_stage, 0x00, sizeof(*p_stage));
			uint32_t week_day[2] = { 0 };
			if (get_xml_prop_arr_def(week_day, cur, "WeekDay", 0) != 2) {
				ERROR_LOG("invalid load_stage_activity_limit_stages: type=%u", stage_id);
				return -1;
			}
			char date_from[128] = "";
			char date_to[128] = "";
			char time_str[128] = "";
			time_t tmptime;
			get_xml_prop_raw_str_def(date_from, cur, "From", "");
			get_xml_prop_raw_str_def(date_to, cur, "To", "");
			if (!date_from[0] || !date_to[0]) {
			} else {
				sprintf(time_str, "%s 00:00:00", date_from);
				sprintf(time_str, "%s 23:59:59", date_to);
			}
			
			if (!date2timestamp(fromstr, "%Y-%m-%d %H:%M:%S", tmptime)) {
				ERROR_LOG("time format invalid[%s]", fromstr);
				return -1;
			}			
			special_stages_map_.insert(std::map<uint32_t, struct specialstages_t*>::value_type(p_stage->stage_id, p_stage));
		}
		cur = cur->next;
	}
	//dump...
	for (std::map<uint32_t, struct specialstages_t*>::iterator iter = special_stages_map_.begin(); 
		iter != special_stages_map_.end(); ++iter) {
		TRACE_LOG("stage id [%u]  task id [%u]", iter->second->stage_id, iter->second->task_id);
	}
	return 0;
}
*/

int player_change_mechanism_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	if (!p->battle_grp) {
		ERROR_LOG("player %u is not in a battle!", p->id);
		p->waitcmd = 0;
		return 0;
	}	
	uint32_t mechanism_id = 0;
	uint32_t status = 0;
	int idx = 0;
	unpack(body, mechanism_id, idx);
	unpack(body, status, idx);
	TRACE_LOG("player %u change mechanism status", p->id);

	idx = sizeof(btl_proto_t);
	taomee::pack_h(btlpkgbuf, mechanism_id, idx);
	taomee::pack_h(btlpkgbuf, status, idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_change_mechanism);

	p->waitcmd = 0;

	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int player_get_mechanism_drop_cmd(player_t *p, uint8_t *body, uint32_t bodylen)
{
	uint32_t mechanism_id = 0;
	uint32_t mechanism_role_type = 0;
	int idx = 0;
	unpack(body, mechanism_id, idx);
	unpack(body, mechanism_role_type, idx);
	
	uint32_t item_id = 0;
	uint32_t item_id2 = 0;
	switch(mechanism_role_type)
	{
		case 39001:
			item_id = 1500367;
		break;

		case 39002:
			item_id = 1500372;
		break;

		case 39003:
			item_id = 1500373;
		break;

		case 39004:
			item_id = 1500374;
		break;

		case 39005:
			item_id = 1500375;
		break;

		case 39006:
			item_id = 1500376;
		break;

		case 39007:
			item_id = 1500377;
		break;

		case 39008:
			item_id = 1500378;
		break;
		case 39009:
			item_id = 1500379;
			item_id2 = 1500381;
		break;
		case 39010:
			item_id = 1500380;
			item_id2 = 1500381;
		break;

		case 39012:
			item_id = 1500393;
		break;
		default:
			item_id = 0;
		break;
	}	
	p->waitcmd = 0;

	if( item_id && !p->my_packs->is_item_exist(item_id))return 0;
	
	if( item_id2 && !p->my_packs->is_item_exist(item_id2))return 0;
	/*
	if(item_id)
	{
		p->my_packs->del_item(p, item_id, 1);
		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, item_id, idx);
		init_cli_proto_head(pkgbuf, p, cli_proto_use_item, idx);
		send_to_player(p, pkgbuf, idx, 0);
		db_use_item(p, item_id, 1, false, false);
	}
	*/	
		
	if (p->battle_grp) {
		idx = sizeof(btl_proto_t);
		taomee::pack_h(btlpkgbuf, mechanism_id, idx);
		init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_get_mechanism_drop);
		return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
	}
	return 0;
}

int btlsvr_change_mechanism_callback(player_t *p, btl_proto_t * pkg)
{
	struct change_mechanism_rsp_t {
		uint32_t change_usr;
		uint32_t mechanism_id;
		uint32_t status;
	}__attribute__((packed));

	change_mechanism_rsp_t * rsp = reinterpret_cast<change_mechanism_rsp_t*>(pkg->body);
	/*
	if (rsp->change_usr == p->id) {
		p->waitcmd = 0;
	} */

//	ERROR_LOG("getange///////  call back.........");

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->mechanism_id, idx);
	pack(pkgbuf, rsp->status, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_player_change_mechanism, idx);
	send_to_player(p, pkgbuf, idx, 0); 
	return 0;
}

int btlsvr_first_kill_boss_callback(player_t *p, btl_proto_t * pkg)
{
	struct first_kill_boss_rsp_t{
		uint32_t stage_id;
		uint32_t boss_id;
		uint32_t kill_time;
	}__attribute__((packed));

	first_kill_boss_rsp_t * rsp = reinterpret_cast<first_kill_boss_rsp_t*>(pkg->body);
	if(rsp->stage_id == 10909 && rsp->boss_id != 13038){
		return 0;
	}

	if (rsp->stage_id == 10954 && rsp->boss_id != 13124) 
	{
		return 0;
	}

	//don't store the history and legend stage record
	if (rsp->stage_id > 50000) {
		return 0;
	}


    if(!is_player_kill_boss_data_exist(p, rsp->stage_id)){
        add_player_kill_boss_data(p, rsp->stage_id, rsp->boss_id, rsp->kill_time);

        db_update_kill_boss_list(p, rsp->stage_id, rsp->boss_id, rsp->kill_time, 0);
        //检查成就是否达成
//        for(uint32_t i = 1; i< 32; i++)
//        {
//            if( is_player_achievement_data_exist(p, i)) continue;
//            if( handles[i].id == 0 || handles[i].p_check_func == NULL || handles[i].p_do_func == NULL)continue;
//            if( handles[i].p_check_func(p, i)){
//                handles[i].p_do_func(p, i, false);
//            }
//        }

//        if (rsp->boss_id >= 13091 && rsp->boss_id <= 13094 && 
//            rsp->stage_id >= 10935 && rsp->stage_id <= 10938) {
//            //	ERROR_LOG("FIRST Kill boos %u %u ", rsp->boss_id, rsp->stage_id);
//
//            if (!is_player_achievement_data_exist(p, 107)) {
//                bool x = true;
//                for (int i = 0; i < 4; i++) {
//                    if (!is_player_kill_boss_data_exist(p, 935 + i, 1)) {
//                        x = false;
//                        break;
//                    }
//                }
//                if (x == true) {
//                    do_achieve_type_logic(p, 107, false);
//                }
//            }
//        }
    }

    /* NOTE: must follow achieve */
    //add_title_interface(p, pve_title);
	return 0;
}

struct pvp_room_rsp_t {
	uint32_t battle_id;
	uint32_t room_id;
	uint32_t status;
	uint32_t first_id;
	uint32_t first_tm;
	uint32_t first_type;
	uint8_t  first_nick[16];
	uint32_t second_id;
	uint32_t second_tm;
	uint32_t second_type;
	uint8_t second_nick[16];

}__attribute__((packed));

int btlsw_get_contest_list_callback(player_t * p, battle_switch_proto_t* bpkg, uint32_t len)
{
	CHECK_VAL_GE(len - sizeof(battle_switch_proto_t), 4);
	int room_cnt = 0;
	int idx = 0;
	unpack_h(bpkg->body, room_cnt, idx);
	CHECK_VAL_EQ(len - sizeof(battle_switch_proto_t), 4 + room_cnt * sizeof(pvp_room_rsp_t)); 
	int ifx = sizeof(cli_proto_t);
	pack(pkgbuf, room_cnt, ifx);
	TRACE_LOG("GET %u pvp room info %u", p->id, room_cnt);
	for (int i = 0; i < room_cnt; i++) {
		pvp_room_rsp_t * room = reinterpret_cast<pvp_room_rsp_t*>(bpkg->body + 
				idx + i * sizeof(pvp_room_rsp_t));
		pack(pkgbuf, room->battle_id, ifx);
		pack(pkgbuf, room->room_id, ifx);
		pack(pkgbuf, room->status, ifx);
		pack(pkgbuf, room->first_id, ifx);
		pack(pkgbuf, room->first_tm, ifx);
		pack(pkgbuf, room->first_type, ifx);
		pack(pkgbuf, room->first_nick, 16, ifx);
		pack(pkgbuf, room->second_id, ifx);
		pack(pkgbuf, room->second_tm, ifx);
		pack(pkgbuf, room->second_type, ifx);
		pack(pkgbuf, room->second_nick, 16, ifx);
		TRACE_LOG("ROOM INFO %u %u %s %s", room->battle_id, room->room_id, 
				room->first_nick, room->second_nick);
	}

	init_cli_proto_head(pkgbuf, p, p->waitcmd, ifx);
	return send_to_player(p, pkgbuf, ifx, 1);
}

int btlsw_auto_join_contest_group_callback(player_t * p, battle_switch_proto_t* bpkg, uint32_t len)
{
	uint64_t session_id = 0;
	int idx = 0;
	CHECK_VAL_GE(len - sizeof(battle_switch_proto_t), sizeof(session_id));
	unpack_h(bpkg->body, session_id, idx);
	p->contest_session = session_id;
	ERROR_LOG("Player join GROUP [%u %lu]", p->id, session_id);
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, session_id, idx);
	//主动加入
	if (p->waitcmd == cli_proto_auto_join_contest) {
		DEBUG_LOG("AUTO JOIN CONTEST GROUP %u %lu", p->id, session_id);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	} 
	
	return 0;
//	else {
//		DEBUG_LOG("RE AUTO JOIN CONTEST GROUP WHEN LOGIN %u %lu", p->id, session_id);
//		//掉线重登自动加入旧的对战组
//		init_cli_proto_head(pkgbuf, p, cli_proto_re_join_contest, idx);
//		return send_to_player(p, pkgbuf, idx, 0);
//	}
	return 0;
}

int btlsw_contest_group_auto_msg_callback(player_t * p, 
										     battle_switch_proto_t * pkg,
											 uint32_t len)
{
	struct contest_auto_start_msg_t {
		uint32_t cur_cir;
		uint32_t start_tm;
		uint32_t first_id;
		uint32_t first_tm;
		uint32_t first_role;
		char     first_name[16];
		uint32_t second_id;
		uint32_t second_tm;
		uint32_t second_role;
		char     second_name[16];
	}__attribute__((packed));
	CHECK_VAL_EQ(len, sizeof(contest_auto_start_msg_t) + sizeof(battle_switch_proto_t));

	contest_auto_start_msg_t * start_msg = reinterpret_cast<contest_auto_start_msg_t*> (pkg->body);
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, start_msg->cur_cir, idx);
	pack(pkgbuf, start_msg->start_tm, idx);
	pack(pkgbuf, start_msg->first_id, idx);
	pack(pkgbuf, start_msg->first_tm, idx);
	pack(pkgbuf, start_msg->first_role, idx);
	pack(pkgbuf, start_msg->first_name, max_nick_size, idx);
	pack(pkgbuf, start_msg->second_id, idx);
	pack(pkgbuf, start_msg->second_tm, idx);
	pack(pkgbuf, start_msg->second_role, idx);
	pack(pkgbuf, start_msg->second_name, max_nick_size, idx);

	ERROR_LOG("GROUP START IN FUTURE MSG %u ", p->id);
	init_cli_proto_head(pkgbuf, p, cli_proto_contest_group_auto_msg, idx);
	return send_to_player(p, pkgbuf, idx, 0);
}

int btlsw_list_contest_group_callback(player_t * p,
									  battle_switch_proto_t * pkg,
									  uint32_t len)
{
	struct contest_info_t {
		uint32_t first_id;
		uint32_t first_tm;
		uint32_t first_role;
		char  first_name[16];
		uint32_t second_id;
		uint32_t second_tm;
		uint32_t second_role;
		char second_name[16];
	}__attribute__((packed));
	CHECK_VAL_GE(len, 4);

	int idx = 0;
	uint32_t contest_cnt = 0;
	uint32_t winner_id = 0;
	unpack_h(pkg->body, contest_cnt, idx);
	unpack_h(pkg->body, winner_id, idx);
	CHECK_VAL_EQ(pkg->len,  contest_cnt * sizeof(contest_info_t) + sizeof(battle_switch_proto_t) + 8); 
	
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, contest_cnt, idx);	
	pack(pkgbuf, winner_id, idx);
	for (uint32_t i = 0; i < contest_cnt; ++i) {
		contest_info_t * contest = reinterpret_cast<contest_info_t*> ( pkg->body + 
				i * sizeof(contest_info_t) + 8);
		pack(pkgbuf, contest->first_id, idx);
		pack(pkgbuf, contest->first_tm, idx);
		pack(pkgbuf, contest->first_role, idx);
		pack(pkgbuf, contest->first_name, max_nick_size, idx);
		pack(pkgbuf, contest->second_id, idx);
		pack(pkgbuf, contest->second_tm, idx);
		pack(pkgbuf, contest->second_role, idx);
		pack(pkgbuf, contest->second_name, max_nick_size, idx);
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int btlsw_leave_contest_group_callback(player_t * p,
							           battle_switch_proto_t * pkg,
									   uint32_t len)
{
//	ERROR_LOG("PLAYER LEAVE GROUP [%u]", p->id);
	p->contest_session = 0;
	int idx = sizeof(cli_proto_t);
	init_cli_proto_head(pkgbuf, p, cli_proto_leave_contest_group, idx);
	if (p->waitcmd == cli_proto_leave_contest_group) {
		return send_to_player(p, pkgbuf, idx, 1);
	} else {
		return send_to_player(p, pkgbuf, idx, 0);
	}
}

struct enter_contest_room_t {
	uint32_t cur_cir;
	uint32_t win_exp;
	uint32_t lose_exp;
	uint32_t surprise_exp;
}__attribute__((packed));

int send_enter_contest_room_msg_to_player(player_t * p, enter_contest_room_t * req)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, req->cur_cir, idx);
	pack(pkgbuf, 300, idx); //报名费
	pack(pkgbuf, req->win_exp, idx);
	pack(pkgbuf, req->lose_exp, idx);
	pack(pkgbuf, req->surprise_exp, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_contest_start, idx);
	return send_to_player(p, pkgbuf, idx, 0);
}


int btlsw_contest_group_enter_room_callback(player_t * p,
	   							  	        battle_switch_proto_t * data, 
											uint32_t len)
{
	CHECK_VAL_EQ(len - sizeof(battle_switch_proto_t), sizeof(enter_contest_room_t));
	enter_contest_room_t * req = reinterpret_cast<enter_contest_room_t*>(data->body);

	if (p->exploit >= 300) {
		p->exploit -= 300;
		KDEBUG_LOG(p->id, "PLAYER ENTER USE EXPLOIT %u %u %u", p->id, 300, p->exploit);
	} else {
		p->exploit = 0;
	}
	db_set_role_info(p, "pvp_fight", p->exploit);

	return send_enter_contest_room_msg_to_player(p, req);
}

int btlsw_contest_btl_callback(player_t * p,
	   						   battle_switch_proto_t * data, 
							   uint32_t len)
{
	return 0;
}

int btlsw_contest_all_over_callback(player_t * p,
							        battle_switch_proto_t * data,
									uint32_t len)
{
	struct contest_group_over_t {
		uint32_t champion_id;
		uint32_t champion_tm;
		char     champion_nick[16];
		uint32_t guess_flag;
	}__attribute__((packed));

	CHECK_VAL_EQ(len - sizeof(battle_switch_proto_t), sizeof(contest_group_over_t));

	contest_group_over_t * pkg = reinterpret_cast<contest_group_over_t*>(data->body);

	//发送竞猜邮件
	if (pkg->guess_flag == 1) {
		char title[MAX_MAIL_TITLE_LEN + 1] = {0};
		char content[MAX_MAIL_CONTENT_LEN + 1] = {0};
		std::vector<mail_item_enclosure> item_vec;
		strncpy(title, char_content[34].content, sizeof(title) - 1);
		strcpy(content, char_content[35].content);
		//ERROR_LOG("mail: %s ===== %s", title, content);
		item_vec.push_back(mail_item_enclosure(2500023, 1));
		db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_normal, 0, &item_vec, 0);
		item_vec.clear();
	} else if (pkg->guess_flag == 2) {
		char title[MAX_MAIL_TITLE_LEN + 1] = {0};
		char content[MAX_MAIL_CONTENT_LEN + 1] = {0};
		std::vector<mail_item_enclosure> item_vec;
		strncpy(title, char_content[38].content, sizeof(title) - 1);
		strcpy(content, char_content[39].content);
		//ERROR_LOG("mail: %s ===== %s", title, content);
		item_vec.push_back(mail_item_enclosure(1500901, 1));
		db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_normal, 0, &item_vec, 0);
		item_vec.clear();
	}

	//发送冠军邮件
	if (pkg->champion_id == p->id && pkg->champion_tm == p->role_tm) {
		char title[MAX_MAIL_TITLE_LEN + 1] = {0};
		char content[MAX_MAIL_CONTENT_LEN + 1] = {0};
		std::vector<mail_item_enclosure> item_vec;
		strncpy(title, char_content[36].content, sizeof(title) - 1);
		strcpy(content, char_content[37].content);
		//ERROR_LOG("mail: %s ===== %s", title, content);
		item_vec.push_back(mail_item_enclosure(2500023, 1));
		item_vec.push_back(mail_item_enclosure(1300315, 1));
		item_vec.push_back(mail_item_enclosure(1740029, 100));
		db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_normal, 0, &item_vec, 0);
		item_vec.clear();
	}

//	p->contest_session = 0;
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, pkg->champion_id, idx);
	pack(pkgbuf, pkg->champion_tm, idx);
	pack(pkgbuf, pkg->champion_nick, max_nick_size, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_contest_all_over, idx);
	return send_to_player(p, pkgbuf, idx, 0);
}

int btlsw_player_win_contest_callback(player_t * p,
									battle_switch_proto_t * data,
									uint32_t len)
{
	struct contest_end_info_t {
		uint32_t cur_cir;
		uint32_t win_flag;
		uint32_t enemy_id;
		uint32_t enemy_tm;
		uint32_t enemy_role;
		char     enemy_name[max_nick_size];
		uint32_t win_val;
		uint32_t monster_val;
		uint32_t surprise_val;
	}__attribute__((packed));
	CHECK_VAL_EQ(data->len - sizeof(battle_switch_proto_t), sizeof(contest_end_info_t));
	contest_end_info_t * req = reinterpret_cast<contest_end_info_t*>(data->body);	

	if (req->win_flag) {
		uint32_t win_val = req->win_val + req->surprise_val + req->monster_val;
		if (win_val > 10000) {
			win_val = 10000;
		}
		KDEBUG_LOG(p->id, "PLAYER WIN CONTEST %u %u", p->exploit, win_val);
		p->exploit += win_val;
	} else {
		if (req->monster_val < 1000) {
			p->exploit += req->monster_val;
		}

		KDEBUG_LOG(p->id, "PLAYER LOSE CONTEST %u %u", p->exploit, req->win_val);
		if (p->exploit < req->win_val && req->win_val < 1000) {
			p->exploit = 0;
		}
	}
	db_set_role_info(p, "pvp_fight", p->exploit);

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, req->cur_cir, idx);
	pack(pkgbuf, req->win_flag, idx);
	pack(pkgbuf, req->enemy_id, idx);
	pack(pkgbuf, req->enemy_tm, idx);
	pack(pkgbuf, req->enemy_role, idx);
	pack(pkgbuf, req->enemy_name, max_nick_size, idx);
	pack(pkgbuf, p->exploit, idx);
	init_cli_proto_head(pkgbuf, p, cli_proto_contest_end_msg, idx);

	return send_to_player(p, pkgbuf, idx, 0);
}

int btlsw_contest_guess_champion_callback(player_t *p,
										  battle_switch_proto_t * data,
										  uint32_t len)
{
	struct guess_champion_t {
		uint32_t errer_no;
		uint32_t guess_id;
		uint32_t guess_tm;
//		uint32_t reduce_val;
	}__attribute((packed));
	CHECK_VAL_EQ(len - sizeof(battle_switch_proto_t), sizeof(guess_champion_t));

	guess_champion_t * pkg = reinterpret_cast<guess_champion_t*>(data->body);

	if (pkg->errer_no) {
		return	send_header_to_player(p, p->waitcmd, pkg->errer_no, 1);
	} else {
		if (p->exploit < 200) {
			p->exploit = 0;
		} else {
			p->exploit -= 200;
//			DEBUG_LOG("PLAYER　REDUCE EXPLOIT %u %u", p->id, pkg->reduce_val);
		}
		KDEBUG_LOG(p->id, "GUESS CHAMPION EXPLOIT%u %u", 200, p->exploit);
		db_set_role_info(p, "pvp_fight", p->exploit);

		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, pkg->guess_id, idx);
		pack(pkgbuf, pkg->guess_tm, idx);
		pack(pkgbuf, p->exploit, idx);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	}
}


int btlsw_list_all_contest_player_callback(player_t * p, 
										   battle_switch_proto_t * data,
										   uint32_t len)
{
	struct contest_player_info_t {
		uint32_t userid;
		uint32_t role_tm;
		uint32_t role_type;
		char     nick_name[max_nick_size];
	}__attribute__((packed));

	int idx = 0;
	uint32_t player_cnt = 0;
	uint32_t guess_id = 0;
	CHECK_VAL_GE(len, sizeof(battle_switch_proto_t) + 8);
	unpack_h(data->body, player_cnt, idx);
	unpack_h(data->body, guess_id, idx);
	CHECK_VAL_EQ(len, sizeof(battle_switch_proto_t) + 8 + player_cnt * sizeof(contest_player_info_t));

	idx = sizeof(cli_proto_t);
	pack(pkgbuf, player_cnt, idx);
	pack(pkgbuf, guess_id, idx);
	for (uint32_t i = 0; i < player_cnt; ++i) {
		contest_player_info_t * player = reinterpret_cast<contest_player_info_t*>(data->body +
			   	8 +  i * sizeof(contest_player_info_t));
		pack(pkgbuf, player->userid, idx);
		pack(pkgbuf, player->role_tm, idx);
		pack(pkgbuf, player->role_type, idx);
		pack(pkgbuf, player->nick_name, max_nick_size, idx);
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int btlsw_list_passed_contest_info_callback(player_t * p,
										    battle_switch_proto_t * data, 
											uint32_t len)
{
	struct passed_contest_t {
		uint32_t userid;
		uint32_t role_tm;
		uint32_t role_type;
		char     nick_name[max_nick_size];
		uint32_t win_flag;
		uint32_t win_val;
		uint32_t monster_val;
		uint32_t spurprise_val;
	}__attribute__((packed));

	uint32_t contest_cnt = 0;
	int idx = 0;
	CHECK_VAL_GE(len - sizeof(battle_switch_proto_t), 4);
	unpack_h(data->body, contest_cnt, idx);
	CHECK_VAL_EQ(len - sizeof(battle_switch_proto_t), 4 + contest_cnt * sizeof(passed_contest_t));

	idx = sizeof(cli_proto_t);
	pack(pkgbuf, contest_cnt, idx);
	for (uint32_t i = 0; i < contest_cnt; ++i) {
		passed_contest_t * contest = reinterpret_cast<passed_contest_t*>(data->body + 4 
				+ i * sizeof(passed_contest_t));
		pack(pkgbuf, contest->userid, idx);
		pack(pkgbuf, contest->role_tm, idx);
		pack(pkgbuf, contest->role_type, idx);
		pack(pkgbuf, contest->nick_name, max_nick_size, idx);
		pack(pkgbuf, contest->win_flag, idx);
		pack(pkgbuf, contest->win_val, idx);
		pack(pkgbuf, contest->monster_val, idx);
		pack(pkgbuf, contest->spurprise_val, idx);
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int btlsw_team_contest_auto_join_callback(player_t * p,
	   									  battle_switch_proto_t * data, 
										  uint32_t len)
{
	int idx = 0;
	int server_id = 0;
	unpack_h(data->body, server_id, idx);
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, server_id, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);

	if (server_id) {
		reduce_team_coin(p, 1000000);
		add_swap_action_times(p, 1431);
		for (uint32_t i = 0; i < p->team_info.team_member_cnt; i++) {
			if (p->team_info.team_member[i].uid != p->id) {
				send_simple_notification_with_pre_svr(p->team_info.team_member[i].uid, p,
						cli_proto_captain_team_contest, server_id);
			}
		}
	}


	return 0;
}

int  send_contest_room_info_to_player(player_t * p,
									  int server_id,
									  uint32_t room_id,
									  uint32_t pos_id)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, server_id, idx);
	pack(pkgbuf, room_id, idx);
	pack(pkgbuf, pos_id, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int btlsw_team_contest_get_server_id_callback(player_t * p,
	                                      	  battle_switch_proto_t * data,
											  uint32_t len)
{
	int idx = 0;
	uint32_t server_id;
	unpack_h(data->body, server_id, idx);

	if (server_id && server_id == get_server_id()) {
		//check has a room for team
		TeamContestRoom * room = get_player_team_contest_room(p->team_info.team_id);
		if (room == NULL) { //the first one try into 
			room = get_contest_room(); //get a new room 
		}
		if (room) {
//			ERROR_LOG("PLAYER>>>>> %u TEAM %u GET ROOM %u", p->id, p->team_info.team_id, room->RoomMap());
			room->team_join_room(p->team_info.team_id);

//			ERROR_LOG("PLAYER>>>>>> %u TEAM %u GET ROOM  POS %u", p->id, p->team_info.team_id, room->GetTeamPos(p->team_info.team_id));

			return send_contest_room_info_to_player(p, 0, room->RoomMap(), room->GetTeamPos(p->team_info.team_id));

		} else {
			ERROR_LOG("SOME FATAL ERROR HAPPEN %u %u", p->id, p->team_info.team_id);
		}
	} else if (server_id) { 
		//错误的服务器
		return send_contest_room_info_to_player(p, server_id, 0, 0);
	} 
	//没有报名
	return send_contest_room_info_to_player(p, -1, 0, 0);
}




int btlsvr_player_contest_yazhu(player_t *p)
{

	if (init_battle(p, btl_mode_watch, 0) == -1) {
		return send_header_to_player(p, p->waitcmd, 1, 1);
	}

	int fd_idx = get_btl_fd_idx_by_server_id(p->watch_info->battle_id);
	if (fd_idx != -1) {
		p->battle_grp->fd_idx = fd_idx;
	} else {
		return send_header_to_player(p, p->waitcmd, 1, 1);
	}


	int idx = sizeof(btl_proto_t);

	pack_h(btlpkgbuf, p->watch_info->battle_id, idx);
	pack_h(btlpkgbuf, p->watch_info->room_id, idx);
	pack_h(btlpkgbuf, p->watch_info->watch_id, idx); 

	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_contest_watch_yazhu);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}



void free_battle_grp(battle_grp_t* grp, player_t* p)
{
	if (!grp) {
		return;
	}
	if (p) {
		leave_pvp_footprint(p, true);
	}
	if(p &&  p->home_grp)
	{
		homesvr_player_leave_battle(p);
		//homesvr_syn_player_info(p, p->home_grp->home_owner_id, p->home_grp->role_regtime);
	}

	if (p && p->battle_grp->pvp_btl_type == pvp_team_contest) {
		player_cancel_contest_team_btl(p);
	}

	if (p) {
		add_player_stat_cnt(p, 1, p->battle_grp->monster_killed_cnt);
	}

	TRACE_LOG("free battle group");
	g_slice_free1(sizeof(battle_grp_t), grp);
}

battle_grp_t* alloc_battle_grp()
{
	TRACE_LOG("alloc battle group");
	return reinterpret_cast<battle_grp_t*>(g_slice_alloc0(sizeof(battle_grp_t)));
}

int btlsvr_contest_cancel_yazhu(player_t *p, uint32_t reason)
{
	int idx = sizeof(btl_proto_t);
	pack_h(btlpkgbuf, reason, idx);
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_contest_cancel_yazhu);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int get_challenge_status_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	uint32_t stageId = 0;
	int idx = 0;
	unpack(body, stageId, idx);
	kill_boss_data * t_data = get_player_kill_boss_data(p, stageId, 1);
	int pass_cnt = 0;
	if (t_data) {
		pass_cnt = t_data->pass_cnt_;
	}

	idx = sizeof(cli_proto_t);
	pack(pkgbuf, stageId, idx);
	pack(pkgbuf, pass_cnt, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1); 
}

int summon_ai_change_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	uint32_t ai_type = 0;
	int idx = 0;
	unpack(body, ai_type, idx);
	if (p->battle_grp && p->fight_summon && p->fight_summon->call_flag == 1) {
		idx = sizeof(btl_proto_t);
		pack_h(btlpkgbuf, ai_type, idx);
		init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_summon_ai_change);
		return	send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
	} 

	return send_header_to_player(p, p->waitcmd, 14829, 1);

}
	

int btlsvr_player_talk(player_t* p, uint8_t* msg, uint32_t msg_len, userid_t recvid)
{
	int idx = sizeof(btl_proto_t);
	pack_h(pkgbuf, recvid, idx);
	pack_h(pkgbuf, msg_len, idx);
	pack(pkgbuf, msg, msg_len, idx);
	init_btl_proto_head(p, p->id, pkgbuf, idx, btl_player_talk);
	return send_to_battle_svr(p, pkgbuf, idx, p->battle_grp->fd_idx);
}

int btlsvr_kill_monster_callback(player_t * p, btl_proto_t * pkg)
{
	int idx = 0;
	uint32_t killed_monster = 0;
	uint32_t diffcult = 0;
	unpack_h(pkg->body, killed_monster, idx);
	unpack_h(pkg->body, diffcult, idx);
	KDEBUG_LOG(p->id, "KILL MONSTER %u", killed_monster);
	p->battle_grp->monster_killed_cnt++;
	update_task_step_info_by_kill_monster(p, killed_monster);
	proc_kill_monster_achieve_logic(p, killed_monster, diffcult, p->battle_grp->monster_killed_cnt);
	return 0;
}


