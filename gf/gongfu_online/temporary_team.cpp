#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee/project/utilities.h>
#include <libtaomee++/utils/md5.h>
#include <kf/utils.hpp>
#include <kf/fight_team.hpp>
#include <libtaomee++/time/time.hpp>
extern "C" {
#include <async_serv/mcast.h>
}

using namespace taomee;

#include "utils.hpp"
#include "message.hpp"
#include "player.hpp"
#include "switch.hpp"
#include "global_data.hpp"
#include "mcast_proto.hpp"
#include "rank_top.hpp"
#include "common_op.hpp"
#include "stat_log.hpp"
#include "chat.hpp"
#include "temporary_team.hpp"
#include "title_system.hpp" 
//-----------------------------------------------------------
// static function declarations
//-----------------------------------------------------------
//--------------------------------------------------------------------

static uint16_t s_temporay_team_id = 0;

uint32_t alloc_new_temporay_team_id()
{
	if (!s_temporay_team_id) {
		s_temporay_team_id++;
	}
	uint32_t ret_id = (uint32_t((get_server_id() % 10000) << 16) & 0xffff0000)  + s_temporay_team_id;
	s_temporay_team_id++;
	return ret_id;
}

bool is_player_have_temporary_team(player_t* p)
{
	return p->temporary_team_id;
}

bool is_player_temporary_team_leader(player_t* p)
{
	std::map<uint32_t, temporary_team_member_t>::iterator it = p->m_temporary_team_map->find(p->id);
	if (it != p->m_temporary_team_map->end()) {
		temporary_team_member_t* p_member = &(it->second);
		if (p_member->member_type == temporary_team_member_type_team_leader) {
			return true;
		}
	}
	return false;
}

void bak_temporary_team_member(const player_t* p)
{
	std::map<uint32_t, temporary_team_member_t>::iterator it_begin = p->m_temporary_team_map->begin();
	std::map<uint32_t, temporary_team_member_t>::iterator it_end = p->m_temporary_team_map->end();

	p->m_temporary_team_map_bak->clear();
	for (;it_begin != it_end; ++it_begin) {
		p->m_temporary_team_map_bak->insert(std::map<uint32_t, 
			temporary_team_member_t>::value_type(it_begin->second.userid, it_begin->second));
	}
}

uint32_t get_temporary_team_leader(player_t* p)
{
	std::map<uint32_t, temporary_team_member_t>::iterator it = p->m_temporary_team_map->begin();
	for (;it != p->m_temporary_team_map->end(); ++it) {
		temporary_team_member_t* p_member = &(it->second);
		if (p_member->member_type == temporary_team_member_type_team_leader) {
			return p_member->userid;
		}
	}
	return 0;
}

bool is_temporary_team_have_member(player_t* p, uint32_t member_id, uint32_t member_tm)
{
	std::map<uint32_t, temporary_team_member_t>::iterator it = p->m_temporary_team_map->find(member_id);
	if (it != p->m_temporary_team_map->end()) {
		return true;
	}
	return false;
}

temporary_team_member_t* get_tmp_team_leader(const player_t* p)
{
	
	std::map<uint32_t, temporary_team_member_t>::iterator it_begin = p->m_temporary_team_map->begin();
	std::map<uint32_t, temporary_team_member_t>::iterator it_end = p->m_temporary_team_map->end();

	for (;it_begin != it_end; ++it_begin) {
		if (it_begin->second.member_type == temporary_team_member_type_team_leader) {
			return &(it_begin->second);
		}
	}
	return 0;
}

uint32_t get_tmp_team_pvp_match_value(const player_t* p)
{
	
	std::map<uint32_t, temporary_team_member_t>::iterator it_begin = p->m_temporary_team_map->begin();
	std::map<uint32_t, temporary_team_member_t>::iterator it_end = p->m_temporary_team_map->end();
	
	uint32_t max_lv = 0;
	for (;it_begin != it_end; ++it_begin) {
		temporary_team_member_t* p_member = &(it_begin->second);	
		if (p_member->level > max_lv) {
			max_lv = p_member->level;
		}
	}
	return max_lv;
}
bool do_2v2_title(player_t* p, uint32_t swap_id)
{
   if (swap_id == 1451) {
       do_special_title_logic(p, 111);
   } else if (swap_id == 1452) {
    //2-3
       do_special_title_logic(p, 112);
   } else if (swap_id == 1453) {
    //4-10
       do_special_title_logic(p, 113);
   } else if (swap_id == 1454) {
    //11-20
       do_special_title_logic(p, 114);
   } else if (swap_id == 1455) {
    //21-50
       do_special_title_logic(p, 115);
   }
   return false;
}



bool judge_player_catch_2v2_rewards(player_t* p, uint32_t swap_id)
{
    uint32_t rank_range = get_player_ranker_range(p, 12);
   if (swap_id == 1451) {
       if (rank_range == 1) {
           return true;
       }
   } else if (swap_id == 1452) {
    //2-3
        if (rank_range >= 2 && rank_range <=3) {
            return true;
        }
   } else if (swap_id == 1453) {
    //4-10
        if (rank_range >= 4 && rank_range <= 10) {
            return true;
        }
   } else if (swap_id == 1454) {
    //11-20
        if (rank_range >= 11 && rank_range <= 20) {
            return true;
        }
   } else if (swap_id == 1455) {
    //21-50
        if (rank_range >= 21 && rank_range <= 50) {
            return true;
        }
   }
   return false;
}



void player_leave_temporary_team(player_t* p)
{
	if (is_player_temporary_team_leader(p)) {
	//destroy the team	
		//mcast_tmp_team_member_changed_msg(p, p->temporary_team_id, true);
		mcast_tmp_team_destroy_msg(p);
		send_player_leave_tmp_team_rsp(p, p->id, p->role_tm, 2);
	} else {
	//team member leave team
		uint32_t leader = get_temporary_team_leader(p);
		send_tmp_team_member_leave_msg_2_leader(leader, p, p->temporary_team_id, 
			p->id, p->role_tm, p->nick);
		send_player_leave_tmp_team_rsp(p, p->id, p->role_tm, 0);
	}
	destroy_temporary_team(p);
}

int send_invite_player_2_tmp_team_rsp(player_t* p)
{
	int idx = sizeof(cli_proto_t);	
	pack(pkgbuf, p->temporary_team_id, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

void send_player_leave_tmp_team_rsp(player_t* from, uint32_t leave_id, uint32_t leave_tm, uint32_t leave_type)
{
	uint8_t tmp_pkgbuf[1024];
	int off_len = sizeof(userid_t);
	uint8_t* buf = tmp_pkgbuf + off_len;

	int idx = sizeof(cli_proto_t);
	pack(buf, from->temporary_team_id, idx);
	pack(buf, leave_id, idx);
	pack(buf, leave_tm, idx);
	pack(buf, leave_type, idx); // 0: leave self , 1: kicked by leader, 2: team destroy

	TRACE_LOG("[%u, %u, %u %s %d]", from->id, leave_id, leave_type, from->nick, get_server_id());
	init_cli_proto_head(buf, from, cli_proto_tmp_team_member_leave, idx);

	TRACE_LOG("%u -> %u", from->id, leave_id);
	player_t* p = get_player(leave_id);
	*(reinterpret_cast<userid_t*>(tmp_pkgbuf)) = leave_id; // fill with receiver id 
	if (p) {
		on_tmp_team_player_recv_self_leave_msg(tmp_pkgbuf, idx + off_len);
	} else {
		chat_across_svr_rltm(from, from->id, tmp_pkgbuf,	idx + off_len);
	}

}

bool temporary_team_add_member(player_t* p, uint32_t member_id, uint32_t member_tm, uint32_t level, uint32_t member_type = 0)
{
	temporary_team_member_t tmp_member = {member_id, member_tm, level, member_type};

	TRACE_LOG("try add member %u %lu", p->id, p->m_temporary_team_map->size());
	if (p->m_temporary_team_map->find(member_id) == p->m_temporary_team_map->end()) {
		p->m_temporary_team_map->insert(std::map<uint32_t, temporary_team_member_t>::value_type(member_id, tmp_member));
		TRACE_LOG("added member %u %lu", p->id, p->m_temporary_team_map->size());	
		KDEBUG_LOG(p->id, "add tmp team member %u %u %u %u", p->temporary_team_id, (uint32_t)(p->m_temporary_team_map->size()), member_id, member_type);
	}
	return true;
}

bool create_temporary_team(player_t* p, uint32_t teamid, uint32_t leaderid, uint32_t leadertm, uint32_t level)
{
	if (p->temporary_team_id) {
		return true;
	}
	
	KDEBUG_LOG(p->id, "create tmp team member %u %u %u %u", p->temporary_team_id, (uint32_t)(p->m_temporary_team_map->size()), leaderid, leadertm);
	p->temporary_team_id = teamid;
	p->m_temporary_team_map->clear();
	temporary_team_add_member(p, leaderid, leadertm, level, 1);
	report_user_base_info_to_chat_svr(p, talk_type_tmp_team, p->temporary_team_id);
	return true;
}



bool temporary_team_del_member(player_t* p, uint32_t member_id, uint32_t member_tm)
{
	if (p->id == member_id) {
		destroy_temporary_team(p);
		return true;
	}
	KDEBUG_LOG(p->id, "del tmp team member %u %u %u %u", p->temporary_team_id, (uint32_t)(p->m_temporary_team_map->size()), member_id, member_tm);
	if (p->m_temporary_team_map->find(member_id) != p->m_temporary_team_map->end()) {
		p->m_temporary_team_map->erase(member_id);
	}
	return true;
}

void destroy_temporary_team(player_t* p, bool report_flg)
{
	KDEBUG_LOG(p->id, "destroy tmp team %u %u", p->temporary_team_id, (uint32_t)(p->m_temporary_team_map->size()));
	p->temporary_team_id = 0;
	p->m_temporary_team_map->clear();
	if (report_flg)
		report_user_base_info_to_chat_svr(p, talk_type_tmp_team, p->temporary_team_id);
}


/**
  * @brief send a real time notification to 'to'
  * @param to user to send a notification to
  * @param from user from whom the notification is sent
  * @param flag 0:accept, 1:refuse, 2:in battle, 3:in trade, 4:in home, 
  * @param m map
  */
int send_invitee_reply_msg(userid_t to, const player_t* from, uint32_t teamid, uint32_t flag)
{
	uint8_t tmp_pkgbuf[1 << 21];
	int off_len = sizeof(userid_t);
	uint8_t* buf = tmp_pkgbuf + off_len;
	
	int idx = sizeof(cli_proto_t);
	pack(buf, teamid, idx);
	pack(buf, from->id, idx);
	pack(buf, from->role_tm, idx);
	pack(buf, (uint32_t)(from->lv), idx);
	pack(buf, from->nick, sizeof(from->nick), idx);
	pack(buf, flag, idx);

	TRACE_LOG("[%u, %u, %u %s %d]", from->id, from->role_tm, flag, from->nick, get_server_id());
	init_cli_proto_head(buf, from, cli_proto_tmp_team_noti_reply_result, idx);

	TRACE_LOG("%u -> %u", from->id, to);
	player_t* p = get_player(to);
	*(reinterpret_cast<userid_t*>(tmp_pkgbuf)) = to; // fill with receiver id 
	if (p) {
		on_tmp_team_inviter_recv_reply_msg(tmp_pkgbuf,  idx + off_len);
	} else {
 		chat_across_svr_rltm(from, from->id, tmp_pkgbuf,  idx + off_len);
	}
	return 0;
}

void send_tmp_team_member_leave_msg_2_leader(userid_t to, const player_t* from, 
	uint32_t teamid, uint32_t memberid, uint32_t membertm, char* nick)
{
	uint8_t tmp_pkgbuf[1 << 21];
	int off_len = sizeof(userid_t);
	uint8_t* buf = tmp_pkgbuf + off_len;
	
	int idx = sizeof(cli_proto_t);
	pack(buf, teamid, idx);
	pack(buf, memberid, idx);
	pack(buf, membertm, idx);
	pack(buf, nick, max_nick_size, idx);

	TRACE_LOG("[%u, %u, %s %d]", from->id, from->role_tm, nick, get_server_id());
	init_cli_proto_head(buf, from, cli_proto_tmp_team_member_leave_msg_2_leader, idx);

	TRACE_LOG("%u -> %u", from->id, to);
	player_t* p = get_player(to);
	*(reinterpret_cast<userid_t*>(tmp_pkgbuf)) = to; // fill with receiver id 
	if (p) {
		//send_to_player(p, tmp_pkgbuf + off_len, idx, 0);
		on_team_leader_recv_member_leave_msg(tmp_pkgbuf,  idx + off_len);
	} else {
 		chat_across_svr_rltm(from, from->id, tmp_pkgbuf,  idx + off_len);
	}
}


/**
  * @brief send a real time notification to 'to'
  * @param to user to send a notification to
  * @param from user from whom the notification is sent
  * @param flag 0:accept, 1:refuse, 2:in battle, 3:in trade, 4:in home, 
  * @param m map
  */
void mcast_tmp_team_member_changed_msg(const player_t* from, 
	uint32_t teamid, bool destroy_flag)
{
	uint8_t tmp_pkgbuf[1 << 21];
	int off_len = sizeof(userid_t);
	uint8_t* buf = tmp_pkgbuf + off_len;

	uint32_t cnt = destroy_flag ? 0 : from->m_temporary_team_map->size();
	int idx = sizeof(cli_proto_t);
	pack(buf, teamid, idx);
	pack(buf, cnt, idx);

	std::map<uint32_t, temporary_team_member_t>::iterator it_begin = from->m_temporary_team_map->begin();
	std::map<uint32_t, temporary_team_member_t>::iterator it_end = from->m_temporary_team_map->end();

	for (; cnt && it_begin != it_end; it_begin++) { // if destroy team then pack empty 
		temporary_team_member_t* p_member = &(it_begin->second);
		
		pack(buf, p_member->userid, idx);
		pack(buf, p_member->role_tm, idx);
		pack(buf, p_member->level, idx);
		pack(buf, p_member->member_type, idx);
	}
	TRACE_LOG("[%u, %u %d]", from->id, from->role_tm, get_server_id());
	init_cli_proto_head(buf, from, cli_proto_noti_tmp_team_info_changed, idx);

	uint32_t to = 0;


	bak_temporary_team_member(from);
	it_begin = from->m_temporary_team_map_bak->begin();
	it_end = from->m_temporary_team_map_bak->end();

	for (;it_begin != it_end; ++it_begin) {
		to = it_begin->second.userid;
		TRACE_LOG("%u -> %u", from->id, to);
		player_t* p = get_player(to);
		*(reinterpret_cast<userid_t*>(tmp_pkgbuf)) = to; // fill with receiver id 
		if (p) {
			on_tmp_team_member_changed(tmp_pkgbuf, idx + off_len);
		} else {
	 		chat_across_svr_rltm(from, from->id, tmp_pkgbuf,  idx + off_len);
		}
	}
}

void mcast_tmp_team_destroy_msg(player_t* from)
{
	std::map<uint32_t, temporary_team_member_t>::iterator it_begin = from->m_temporary_team_map->begin();
	std::map<uint32_t, temporary_team_member_t>::iterator it_end = from->m_temporary_team_map->end();

	for (; it_begin != it_end; it_begin++) { // if destroy team then pack empty 
		temporary_team_member_t* p_member = &(it_begin->second);
		if (p_member->userid != from->id) {
			send_player_leave_tmp_team_rsp(from, p_member->userid, p_member->role_tm, 2);
		}
	}
}

int on_tmp_team_recv_pvp_rsp(void* buf, uint32_t len)
{
	uint32_t  rcvid = *(reinterpret_cast<uint32_t*>(buf));
	player_t* rcver = get_player(rcvid);
	TRACE_LOG("invitee_recv_invite_msg ,rcvid=%u", rcvid);
	if (rcver) {
		cli_proto_t* proto = reinterpret_cast<cli_proto_t*>((reinterpret_cast<uint8_t*>(buf)) + 4);
		uint32_t team_id =	0;
		uint32_t local_room_id =  0;
		uint32_t fd_idx = 0;
		int idx = 0;

		unpack(proto->body, team_id, idx);
		unpack(proto->body, local_room_id, idx);
		unpack(proto->body, fd_idx, idx);

		if (enter_room(rcver, fd_idx, btl_mode_temp_team_pvp) != 0) {
			KDEBUG_LOG(rcver->id, "team member enter pvp err %u", fd_idx);
			return 0;
		}
		rcver->battle_grp->id = local_room_id;
		rcver->battle_grp->pvp_btl_type = pvp_temporary_team;
		send_tmp_team_hot_join_pvp_room_2_btl(rcver, local_room_id, pvp_temporary_team, btl_mode_temp_team_pvp);
		
		KDEBUG_LOG(rcvid, "recv tmp team pvp rsp %u %u %u", team_id, local_room_id, fd_idx);
		return send_to_player(rcver, (reinterpret_cast<uint8_t*>(buf)) + 4, len - 4, 0);
	}
	return 0;
}


void send_tmp_team_pvp_rsp(player_t* from, uint32_t member_id, uint32_t member_tm, uint32_t btl_id)
{
	uint8_t tmp_pkgbuf[1024];
	int off_len = sizeof(userid_t);
	uint8_t* buf = tmp_pkgbuf + off_len;

	int idx = sizeof(cli_proto_t);
	pack(buf, from->temporary_team_id, idx);
	pack(buf, btl_id, idx); 
	pack(buf, from->battle_grp->fd_idx, idx); 

	TRACE_LOG("[%u, %u %s %d]", from->id, member_id, from->nick, get_server_id());
	init_cli_proto_head(buf, from, cli_proto_tmp_team_pvp_start, idx);

	TRACE_LOG("%u -> %u", from->id, member_id);
	player_t* p = get_player(member_id);
	*(reinterpret_cast<userid_t*>(tmp_pkgbuf)) = member_id; // fill with receiver id 
	if (p) {
		on_tmp_team_recv_pvp_rsp(tmp_pkgbuf, idx + off_len);
	} else {
		chat_across_svr_rltm(from, from->id, tmp_pkgbuf,	idx + off_len);
	}
}

void mcast_tmp_team_pvp_rsp(player_t* from)
{
	std::map<uint32_t, temporary_team_member_t>::iterator it_begin = from->m_temporary_team_map->begin();
	std::map<uint32_t, temporary_team_member_t>::iterator it_end = from->m_temporary_team_map->end();

	for (; it_begin != it_end; it_begin++) { // if destroy team then pack empty 
		temporary_team_member_t* p_member = &(it_begin->second);
		if (from->id != p_member->userid) {
			send_tmp_team_pvp_rsp(from, p_member->userid, p_member->role_tm, from->battle_grp->id);
		}
	}
}

int on_tmp_team_member_changed(void* buf, uint32_t len)
{
	uint32_t  rcvid = *(reinterpret_cast<uint32_t*>(buf));
	player_t* rcver = get_player(rcvid);
	TRACE_LOG("invitee_recv_invite_msg ,rcvid=%u", rcvid);
	if (rcver) {
		cli_proto_t* proto = reinterpret_cast<cli_proto_t*>((reinterpret_cast<uint8_t*>(buf)) + 4);
		
		team_team_member_changed_t* p_rsp = 
			reinterpret_cast<team_team_member_changed_t*>(proto->body);
		uint32_t teamid =  bswap(p_rsp->teamid);
		uint32_t cnt =  bswap(p_rsp->cnt);

		KDEBUG_LOG(rcvid, "tmp team member changed team %u cnt %u ", teamid, cnt);

		if (rcver->temporary_team_id) {
			destroy_temporary_team(rcver, false);
		} else if (cnt) {
			report_user_base_info_to_chat_svr(rcver, talk_type_tmp_team, teamid);
		}

		if (cnt) {
			rcver->temporary_team_id = teamid;
		}
		int idx = 8;
		for (uint32_t i = 0; i < cnt; i++) {
			uint32_t member_id = 0;
			uint32_t member_tm = 0;
			uint32_t member_type = 0;
			uint32_t level = 0;
			unpack(proto->body, member_id, idx);
			unpack(proto->body, member_tm, idx);
			unpack(proto->body, level, idx);
			unpack(proto->body, member_type, idx);

			KDEBUG_LOG(rcvid, "tmp team member changed team %u %u %u %u", member_id, member_tm, level, member_type);
		
			temporary_team_add_member(rcver, member_id, member_tm, level, member_type);
		}
		return send_to_player(rcver, (reinterpret_cast<uint8_t*>(buf)) + 4, len - 4, 0);
	}
	return 0;

}

void send_team_status_rsp(player_t* from, uint32_t from_id, uint32_t teamid, uint32_t member_id, uint32_t member_tm, uint32_t status)
{
	uint8_t tmp_pkgbuf[1024];
	int off_len = sizeof(userid_t);
	uint8_t* buf = tmp_pkgbuf + off_len;

	int idx = sizeof(cli_proto_t);
	pack(buf, teamid, idx);
	pack(buf, status, idx); //0 nor , 1 team full, 2 off line

	TRACE_LOG("[%u, %u %s %d]", from_id, member_id, from->nick, get_server_id());
	init_cli_proto_head(buf, from, cli_proto_tmp_team_status, idx);

	TRACE_LOG("%u -> %u", from_id, member_id);
	player_t* p = get_player(member_id);
	*(reinterpret_cast<userid_t*>(tmp_pkgbuf)) = member_id; // fill with receiver id 
	if (p) {
		on_tmp_team_recv_team_status_rsp(tmp_pkgbuf, idx + off_len);
	} else {
		chat_across_svr_rltm(from, from->id, tmp_pkgbuf,	idx + off_len);
	}
}

int on_tmp_team_recv_team_status_rsp(void* buf, uint32_t len)
{
	//only team leader process
	uint32_t  rcvid = *(reinterpret_cast<uint32_t*>(buf));
	player_t* rcver = get_player(rcvid);
	TRACE_LOG("tmp_team_recv_team_status ,rcvid=%u", rcvid);
	if (rcver) {
		return send_to_player(rcver, (reinterpret_cast<uint8_t*>(buf)) + 4, len - 4, 0);
	}
	return 0;

}

int on_tmp_team_inviter_recv_reply_msg(void* buf, uint32_t len)
{
	//only team leader process
	uint32_t  rcvid = *(reinterpret_cast<uint32_t*>(buf));
	player_t* rcver = get_player(rcvid);
	TRACE_LOG("invitee_recv_invite_msg ,rcvid=%u", rcvid);

	cli_proto_t* proto = reinterpret_cast<cli_proto_t*>((reinterpret_cast<uint8_t*>(buf)) + 4);
	tmp_team_invite_recv_reply_t* p_rsp = 
		reinterpret_cast<tmp_team_invite_recv_reply_t*>(proto->body);
	uint32_t invitee_id =  bswap(p_rsp->invitee_id);
	uint32_t invitee_tm =  bswap(p_rsp->invitee_tm);
	uint32_t teamid =	bswap(p_rsp->teamid);
	uint32_t flag =	bswap(p_rsp->flag);
	uint32_t level =	bswap(p_rsp->level);
	
	if (rcver) {
		if (flag == tmp_team_reply_accept) {
			if (rcver->temporary_team_id && 
				rcver->m_temporary_team_map->size() < max_temporary_team_member_number) {
				if (temporary_team_add_member(rcver, invitee_id, invitee_tm, level)) {
					mcast_tmp_team_member_changed_msg(rcver, teamid);
				}
			} else {
				send_team_status_rsp(rcver, rcvid, teamid, invitee_id, invitee_tm, 1);
			}
			KDEBUG_LOG(rcvid, "inviter recv reply %u %u %u %u", teamid, invitee_id, invitee_tm, flag);
		} 
		
		return send_to_player(rcver, (reinterpret_cast<uint8_t*>(buf)) + 4, len - 4, 0);
	} else {
	//rcver off line ....
		send_team_status_rsp(rcver, rcvid, teamid, invitee_id, invitee_tm, 2);
	}
	return 0;
}


int on_invitee_recv_invite_msg(void* buf, uint32_t len)
{
	uint32_t  rcvid = *(reinterpret_cast<uint32_t*>(buf));
	player_t* rcver = get_player(rcvid);
	TRACE_LOG("invitee_recv_invite_msg ,rcvid=%u", rcvid);
	if (rcver) {
		cli_proto_t* proto = reinterpret_cast<cli_proto_t*>((reinterpret_cast<uint8_t*>(buf)) + 4);
		tmp_team_invite_notification_t* p_rsp = 
			reinterpret_cast<tmp_team_invite_notification_t*>(proto->body);
		uint32_t inviter_id =  bswap(p_rsp->inviter_id);
		uint32_t temporary_team_id =  bswap(p_rsp->temporary_team_id);

		KDEBUG_LOG(rcvid, "invitee_recv_invite_msg ,teamid %u inviter %u", temporary_team_id, inviter_id);
		if (rcver->battle_grp) {
			return send_invitee_reply_msg(inviter_id, rcver, temporary_team_id, tmp_team_reply_btl);
		} else if (rcver->trade_grp || rcver->safe_trade_room_id) {
			return send_invitee_reply_msg(inviter_id, rcver, temporary_team_id, tmp_team_reply_trd);
		} else if (rcver->home_grp) {
			return send_invitee_reply_msg(inviter_id, rcver, temporary_team_id, tmp_team_reply_home);
		} else if (rcver->temporary_team_id) {
			return send_invitee_reply_msg(inviter_id, rcver, temporary_team_id, tmp_team_reply_repeat);
		}

		if (rcver->client_buf[6]) {
			return send_invitee_reply_msg(inviter_id, rcver, temporary_team_id, tmp_team_reply_client_config_refuse);
		}
		return send_to_player(rcver, (reinterpret_cast<uint8_t*>(buf)) + 4, len - 4, 0);
	}
	return 0;
}

int on_tmp_team_player_recv_self_leave_msg(void* buf, uint32_t len)
{
	uint32_t  rcvid = *(reinterpret_cast<uint32_t*>(buf));
	player_t* rcver = get_player(rcvid);
	TRACE_LOG("invitee_recv_invite_msg ,rcvid=%u", rcvid);
	if (rcver) {
		cli_proto_t* proto = reinterpret_cast<cli_proto_t*>((reinterpret_cast<uint8_t*>(buf)) + 4);
		tmp_team_player_recv_self_leave_msg_t* p_rsp = 
			reinterpret_cast<tmp_team_player_recv_self_leave_msg_t*>(proto->body);
		uint32_t team_id =  bswap(p_rsp->team_id);
		uint32_t leave_id =  bswap(p_rsp->leave_id);
		uint32_t leave_tm =  bswap(p_rsp->leave_tm);
		uint32_t leave_type =  bswap(p_rsp->leave_type);

		KDEBUG_LOG(rcvid, "recv self leave msg %u %u %u %u", team_id, leave_id, leave_tm, leave_type);
		if (leave_id == rcver->id && leave_tm == rcver->role_tm) {
			destroy_temporary_team(rcver);
			return send_to_player(rcver, (reinterpret_cast<uint8_t*>(buf)) + 4, len - 4, 0);
		}
	}
	return 0;
}



int on_team_leader_recv_member_leave_msg(void* buf, uint32_t len)
{
	uint32_t  rcvid = *(reinterpret_cast<uint32_t*>(buf));
	player_t* rcver = get_player(rcvid);
	TRACE_LOG("invitee_recv_invite_msg ,rcvid=%u", rcvid);
	if (rcver) {
		if (is_player_temporary_team_leader(rcver)) {
			cli_proto_t* proto = reinterpret_cast<cli_proto_t*>((reinterpret_cast<uint8_t*>(buf)) + 4);
			team_leader_recv_member_leave_msg_t* p_rsp = 
				reinterpret_cast<team_leader_recv_member_leave_msg_t*>(proto->body);
			uint32_t teamid =  bswap(p_rsp->teamid);
			uint32_t member_id =  bswap(p_rsp->member_id);
			uint32_t member_tm =  bswap(p_rsp->member_tm);
			KDEBUG_LOG(rcvid, "leader recv member leave msg %u %u %u", teamid, member_id, member_tm);
			temporary_team_del_member(rcver, member_id, member_tm);
			mcast_tmp_team_member_changed_msg(rcver, teamid);

			if (rcver->m_temporary_team_map->size() == 1) {
			//auto destroy
				send_player_leave_tmp_team_rsp(rcver, rcver->id, rcver->role_tm, 2);
			}
		}
	}
	return 0;
}


/**
  * @brief send a real time notification to 'to'
  * @param to user to send a notification to
  * @param from user from whom the notification is sent
  * @param type type of the notification
  * @param accept
  * @param m map
  */
void send_tmp_team_invite_notification(uint16_t cmd, userid_t to, const player_t* from, tmp_team_invite_notification_t* msg_in)
{
	int off_len = sizeof(userid_t);
	uint8_t* buf = pkgbuf + off_len;
	
	int idx = sizeof(cli_proto_t);
	pack(buf, msg_in, sizeof(*msg_in), idx);

	TRACE_LOG("[%u, %u, %u %s %d]", from->id, from->role_tm, from->lv, from->nick, get_server_id());
	init_cli_proto_head(buf, from, cmd, idx);

	TRACE_LOG("%u -> %u", from->id, to);
	player_t* p = get_player(to);

	*(reinterpret_cast<userid_t*>(pkgbuf)) = to; // fill with receiver id 
	if (p) {
		on_invitee_recv_invite_msg(pkgbuf,  idx + off_len);
	} else {
 		chat_across_svr_rltm(from, from->id, pkgbuf,  idx + off_len);
	}
}

/**
  * @brief 
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int invite_player_2_tmp_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	invite_player_2_tmp_team_in_t cmd_in = { 0 };
	
	unpack(body, cmd_in.invitee_id, idx);	
	unpack(body, cmd_in.invitee_tm, idx);	

	if (p->m_temporary_team_map->size() >= max_temporary_team_member_number) {
		WARN_LOG("PLAY already have a temp team %u", p->id);
		p->waitcmd = 0;
		return 0;
	}

	if (create_temporary_team(p, alloc_new_temporay_team_id(), p->id, p->role_tm, p->lv)) {
	//创建临时队伍成功:
		send_invite_player_2_tmp_team_rsp(p);

		if (cmd_in.invitee_id && cmd_in.invitee_tm) {
		//发送邀请给被邀请者

			tmp_team_invite_notification_t msg_in = { 0 };
			msg_in.inviter_id = bswap(p->id);
			msg_in.inviter_tm = bswap(p->role_tm);
			memcpy(msg_in.inviter_nick, p->nick, sizeof(msg_in.inviter_nick));
			msg_in.temporary_team_id = bswap(p->temporary_team_id);
			send_tmp_team_invite_notification(cli_proto_tmp_team_invite_msg_2_invitee, 
				cmd_in.invitee_id, p, &msg_in);
		}
	}
	return 0;
}

/**
  * @brief 
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int reply_tmp_team_invite_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	reply_tmp_team_invite_in_t cmd_in = { 0 };
	
	unpack(body, cmd_in.teamid, idx);	
	unpack(body, cmd_in.inviter_id, idx);	
	unpack(body, cmd_in.inviter_tm, idx);	
	unpack(body, cmd_in.flag, idx);	

	if (is_player_have_temporary_team(p)) {
		WARN_LOG("reply: PLAY already have a temp team %u", p->id);
		p->waitcmd = 0;
		return 0;
	}

	send_invitee_reply_msg(cmd_in.inviter_id, p, 
		cmd_in.teamid, cmd_in.flag ? tmp_team_reply_refuse : tmp_team_reply_accept);
	
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

/**
  * @brief 
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int tmp_team_member_leave_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	uint32_t member_id = 0;
	uint32_t member_tm = 0;
	int idx = 0;
	unpack(body, member_id, idx);	
	unpack(body, member_tm, idx);	
//	char tmp_nick[max_nick_size] = "team member";

	if (is_player_have_temporary_team(p) && is_temporary_team_have_member(p, member_id, member_tm)) {
		if (p->id == member_id) {
			player_leave_temporary_team(p);
		} else {
			if (is_player_temporary_team_leader(p)) {
			//kick member
				send_player_leave_tmp_team_rsp(p,member_id, member_tm, 1);
				if (temporary_team_del_member(p, member_id, member_tm)) {
					mcast_tmp_team_member_changed_msg(p, p->temporary_team_id);
				}
				if (p->m_temporary_team_map->size() == 1) {
				//auto destroy
					send_player_leave_tmp_team_rsp(p, p->id, p->role_tm, 2);
				}
			}
		}
		p->waitcmd = 0;
		return 0;
	}
	p->waitcmd = 0;
	return 0;//return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
}

int send_tmp_team_pvp_invite_2_member(player_t* from, uint32_t member_id, uint32_t member_tm)
{
	uint8_t tmp_pkgbuf[1024];
	int off_len = sizeof(userid_t);
	uint8_t* buf = tmp_pkgbuf + off_len;

	int idx = sizeof(cli_proto_t);
	pack(buf, from->temporary_team_id, idx);
	init_cli_proto_head(buf, from, cli_proto_send_tmp_team_pvp_invite_2_member, idx);

	uint32_t to = 0;

	std::map<uint32_t, temporary_team_member_t>::iterator it_begin = from->m_temporary_team_map->begin();
	std::map<uint32_t, temporary_team_member_t>::iterator it_end = from->m_temporary_team_map->end();
	for (;it_begin != it_end; ++it_begin) {
		to = it_begin->second.userid;
		
		if (member_id && member_tm) {
			if (member_id == to) {
				TRACE_LOG("%u -> %u", from->id, to);
				player_t* p = get_player(to);
				*(reinterpret_cast<userid_t*>(tmp_pkgbuf)) = to; // fill with receiver id 
				if (p) {
					on_tmp_team_member_recv_pvp_invite(tmp_pkgbuf, idx + off_len);
				} else {
					chat_across_svr_rltm(from, from->id, tmp_pkgbuf,	idx + off_len);
				}
			}
		} else {
			TRACE_LOG("%u -> %u", from->id, to);
			player_t* p = get_player(to);
			*(reinterpret_cast<userid_t*>(tmp_pkgbuf)) = to; // fill with receiver id 
			if (p) {
				on_tmp_team_member_recv_pvp_invite(tmp_pkgbuf, idx + off_len);
			} else {
				chat_across_svr_rltm(from, from->id, tmp_pkgbuf,	idx + off_len);
			}
		}
	}
	return 0;
}

/**
  * @brief 
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int tmp_team_invite_pvp_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	uint32_t member_id = 0;
	uint32_t member_tm = 0;
	int idx = 0;
	unpack(body, member_id, idx);	
	unpack(body, member_tm, idx);

	if (is_player_temporary_team_leader(p)) {
		send_tmp_team_pvp_invite_2_member(p, member_id, member_tm);
		return send_header_to_player(p, p->waitcmd, 0, 1);
	}
	return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
}


int send_tmp_team_reply_pvp_invite(player_t* from, uint32_t teamid, uint32_t flag)
{
	uint8_t tmp_pkgbuf[1024];
	int off_len = sizeof(userid_t);
	uint8_t* buf = tmp_pkgbuf + off_len;

	int idx = sizeof(cli_proto_t);
	pack(buf, teamid, idx);
	pack(buf, from->id, idx);
	pack(buf, from->role_tm, idx);
	pack(buf, flag, idx);
	init_cli_proto_head(buf, from, cli_proto_tmp_send_team_reply_pvp_invite, idx);

	uint32_t to = 0;

	std::map<uint32_t, temporary_team_member_t>::iterator it_begin = from->m_temporary_team_map->begin();
	std::map<uint32_t, temporary_team_member_t>::iterator it_end = from->m_temporary_team_map->end();
	for (;it_begin != it_end; ++it_begin) {
		to = it_begin->second.userid;
		TRACE_LOG("%u -> %u", from->id, to);
		player_t* p = get_player(to);
		*(reinterpret_cast<userid_t*>(tmp_pkgbuf)) = to; // fill with receiver id 
		if (p) {
			on_tmp_team_member_recv_reply_pvp_invite(tmp_pkgbuf, idx + off_len);
		} else {
			chat_across_svr_rltm(from, from->id, tmp_pkgbuf,	idx + off_len);
		}
	}
	return 0;
}

/**
  * @brief 
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int tmp_team_reply_pvp_invite_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	uint32_t teamid = 0;
	uint32_t flag = 0;
	int idx = 0;
	unpack(body, teamid, idx);	
	unpack(body, flag, idx); //0 not ready, 1 set ready

 	if (p->trade_grp || p->battle_grp || p->home_grp) {
		return send_header_to_player(p, p->waitcmd, cli_err_action_refused, 1);
	}
	if (is_player_have_temporary_team(p)) {
		send_tmp_team_reply_pvp_invite(p, teamid, flag);
		return send_header_to_player(p, p->waitcmd, 0, 1);
	}
	return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
}


int on_tmp_team_member_recv_pvp_invite(void* buf, uint32_t len)
{
	uint32_t  rcvid = *(reinterpret_cast<uint32_t*>(buf));
	player_t* rcver = get_player(rcvid);
	TRACE_LOG("invitee_recv_invite_msg ,rcvid=%u", rcvid);
	if (rcver) {
		cli_proto_t* proto = reinterpret_cast<cli_proto_t*>((reinterpret_cast<uint8_t*>(buf)) + 4);
				
		tmp_team_member_recv_pvp_invite_t* p_rsp = 
			reinterpret_cast<tmp_team_member_recv_pvp_invite_t*>(proto->body);
		uint32_t teamid =  bswap(p_rsp->teamid);

		if (rcver->battle_grp) {
			return send_tmp_team_reply_pvp_invite(rcver, teamid, 2);
		} else if (rcver->trade_grp || rcver->safe_trade_room_id) {
			return send_tmp_team_reply_pvp_invite(rcver, teamid, 3);
		} else if (rcver->home_grp) {
			return send_tmp_team_reply_pvp_invite(rcver, teamid, 4);
		}
		
		KDEBUG_LOG(rcver->id, "tmp team member recv pvp invite %u", teamid);
		return send_to_player(rcver, (reinterpret_cast<uint8_t*>(buf)) + 4, len - 4, 0);
	}
	return 0;

}

int on_tmp_team_member_recv_reply_pvp_invite(void* buf, uint32_t len)
{
	uint32_t  rcvid = *(reinterpret_cast<uint32_t*>(buf));
	player_t* rcver = get_player(rcvid);
	TRACE_LOG("invitee_recv_invite_msg ,rcvid=%u", rcvid);
	if (rcver) {
		cli_proto_t* proto = reinterpret_cast<cli_proto_t*>((reinterpret_cast<uint8_t*>(buf)) + 4);
				
		mp_team_member_recv_reply_pvp_invite_t* p_rsp = 
			reinterpret_cast<mp_team_member_recv_reply_pvp_invite_t*>(proto->body);
		uint32_t teamid =  bswap(p_rsp->teamid);
		uint32_t memberid =  bswap(p_rsp->memberid);
		uint32_t membertm =  bswap(p_rsp->membertm);
		uint32_t flag =  bswap(p_rsp->flag);
		
		KDEBUG_LOG(rcver->id, "tmp team member recv reply pvp invite %u %u %u %u", teamid, memberid, membertm, flag);
		return send_to_player(rcver, (reinterpret_cast<uint8_t*>(buf)) + 4, len - 4, 0);
	}
	return 0;
}




