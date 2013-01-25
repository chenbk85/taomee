#include <cerrno>
#include <cstdlib>
#include <cstring>

#include <vector>

using namespace std;

#include <libtaomee++/inet/pdumanip.hpp>

using namespace taomee;

extern "C" {
#include <fcntl.h>
#include <sys/mman.h>

#include <libtaomee/timer.h>
#include <libtaomee/utils.h>
#include <libtaomee/conf_parser/config.h>
#include "libtaomee/project/stat_agent/msglog.h"
}

#include "dbproxy.hpp"
#include "utils.hpp"

#include "online.hpp"

#include "proto/mole2_switch.h"
#include "proto/mole2_switch_enum.h"
#define CHECK_USER_ID(uid_) \
		do { \
			if (!Online::is_valid_uid((uid_))) { \
				ERROR_LOG("invalid uid=%u", (uid_)); \
				return -1; \
			} \
		} while (0)

struct friend_list_t {
	uint32_t cnt;
	userid_t uid[];
} __attribute__((packed));

userid_t		   Online::s_max_uid_;
uint32_t           Online::s_max_online_id_;
online_info_t      Online::s_online_info_[online_num_max];
keepalive_timer_t  Online::s_keepalive_tmr_[online_num_max];
timer_head_t       Online::s_statistic_tmr_;
Online::fd_map_t   Online::s_fd_map_;

Online::users_online_t	Online::s_users;
Online::userid_set_t 	Online::s_users_set[online_num_max];
uint8_t      Online::s_pkg_[pkg_size];
char*        Online::s_statistic_file_ = NULL;
pkwait_info_t Online::s_single_pk_[pklv_phase_max];
pkwait_info_t Online::s_team_pk_[pk_team_cnt_max][pklv_phase_max];
//pkwait_info_t Online::s_gvg_pk_[2][pklv_phase_max];
//std::map<uint32_t,uint32_t> Online::s_gvg_challengee_;
pkwait_info_t Online::s_pet_pk_[pk_pet_cnt_max][pklv_phase_max];
Online::pk_set_t	  Online::s_pkwait_;
Online::pk_start_t	  Online::s_start_;
Online::pk_accept_t	  Online::s_accept_;
CLevelDB      Online::lvdb;
int Online::s_idc_ = 0;
int Online::s_beast_cnt_ = -1;

//timer_head_t Online::timer_head;




//-------------------------------------------------------------
// For Online Server
//
// receive info reported by online server
int Online::report_online_info(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	// define online information 
	struct online_info_req_t {
		uint16_t	domain_id;
		uint32_t	online_id;
		uint8_t		online_name[16];
		uint8_t		online_ip[16];
		in_port_t	online_port;
		uint32_t	user_num;
		uint32_t	seqno;
	} __attribute__((packed));

	CHECK_VAL(bodylen, sizeof(online_info_req_t));

	online_info_req_t* req = reinterpret_cast<online_info_req_t*>(pkg->body);
	if ((req->online_id > online_num_max) || (req->online_id == 0)) {
		ERROR_LOG("invalid onlineid=%u from fd=%d", req->online_id, fdsess->fd);
		return -1;
	}

	int idx = req->online_id - 1;
	// this can happen on a very special case
	if (s_online_info_[idx].online_id == req->online_id) {
		DEBUG_LOG("DUPLICATE ONLINE ID\t[id=%u]", req->online_id);
		// close previous fd
		close_client_conn(s_online_info_[idx].fdsess->fd);
	}
	
	s_online_info_[idx].domain_id   = req->domain_id;
	s_online_info_[idx].online_id   = req->online_id;
	memcpy(s_online_info_[idx].online_ip, req->online_ip, sizeof(s_online_info_[0].online_ip));
	s_online_info_[idx].online_port = req->online_port;
	s_online_info_[idx].user_num    = req->user_num;
	s_online_info_[idx].seqno       = req->seqno; // depreciated
	s_online_info_[idx].fdsess      = fdsess;

	// make an online connection dead if no package is received in 70 secs
	INIT_LIST_HEAD(&(s_keepalive_tmr_[idx].timer_list));
	s_keepalive_tmr_[idx].tmr = ADD_TIMER_EVENT_EX(&s_keepalive_tmr_[idx], n_make_online_dead, &s_online_info_[idx], get_now_tv()->tv_sec + 70);
	if (!s_keepalive_tmr_[idx].tmr) {
		ERROR_RETURN(("add timer fail"), -1);
	}

	s_fd_map_[fdsess->fd] = req->online_id;

	//update s_max_online_id_
	if (s_max_online_id_ < req->online_id) {
		s_max_online_id_ = req->online_id;
	}

	DEBUG_LOG("ONLINE INFO\t[max=%u fd=%d olid=%u olip=%.16s olport=%d usrnum=%u seq=%u]",
				s_max_online_id_, fdsess->fd, req->online_id, req->online_ip, req->online_port, req->user_num, req->seqno);

	return 0;
}

int Online::report_user_onoff(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen, 2);

	uint16_t cur_online_id = s_fd_map_[fdsess->fd];
	int cur_idx = cur_online_id - 1;
	CHECK_VAL_GE(cur_idx, 0);

	if (!is_guest(pkg->id)) { 
		CHECK_USER_ID(pkg->id);

		if (pkg->body[0]) { //user online
			uint16_t online_id = get_user_online_id(pkg->id);
			// this would be a bug of online server
			if (online_id == cur_online_id) {
				ERROR_LOG("impossible error! online id match=%d", cur_online_id);
				return -1;
			}

			if (online_id != 0) {
				// notify the other online server to kick the user offline
				int idx = sizeof(svr_proto_t);
				pack_h(s_pkg_, pkg->id, idx);
				pack_h(s_pkg_, 0x00100001, idx);
				init_proto_head(s_pkg_, pkg->id, idx, 0, proto_kick_usr_offline, 0);
				send_pkg_to_client(s_online_info_[online_id - 1].fdsess, s_pkg_, idx);

				--(s_online_info_[online_id - 1].user_num);

				DEBUG_LOG("KICK PREVIOUS LOGIN OFFLINE\t[uid=%u olid=%d]", pkg->id, online_id);
			}

			++(s_online_info_[cur_idx].user_num);
			set_user_online_id(pkg->id, cur_online_id);
			s_users_set[cur_idx].insert(pkg->id);
			DEBUG_LOG("USER LOGIN\t[uid=%u olid=%d]", pkg->id, cur_online_id);
		} else { //user offline
			--(s_online_info_[cur_idx].user_num);
			if (get_user_online_id(pkg->id) == cur_online_id) {
				set_user_online_id(pkg->id, 0);
				s_users_set[cur_idx].erase(pkg->id);
			}
			clean_user_pkinfo(pkg->id, pkg->body[1], fdsess);
			DEBUG_LOG("USER OFF\t[uid=%u olid=%d]", pkg->id, cur_online_id);
		}
	} else {  //guest user
		if (pkg->body[0]) {
			++(s_online_info_[cur_idx].user_num);
		} else {
			--(s_online_info_[cur_idx].user_num);
		}
	}

	// TODO - to be commented out
	if (((int)(s_online_info_[cur_idx].user_num)) < 0) {
		ERROR_LOG("impossible error! user num=%d", s_online_info_[cur_idx].user_num);
		return -1;
	}

	return 0;
}

void Online::clean_user_pkinfo(userid_t uid, uint8_t inbattle, fdsession_t* fdsess)
{
	pk_start_t::iterator itst;
	pk_accept_t::iterator itacpt;
	if ((itst = s_start_.find(uid)) != s_start_.end()) {
		if (inbattle) {
			ERROR_LOG("IN");
			s_start_.erase(uid);
		} else {
			pkwait_info_t pkinfo = itst->second;
			INIT_LIST_HEAD(&pkinfo.timer_list);
			s_accept_.erase(pkinfo.teamid);
			s_start_.erase(uid);
			s_pkwait_.erase(uid);
			uint32_t match;
			pkwait_info_t* ppi = try_to_match(&pkinfo, match);
			if (match == match_succ) {
				int idx = sizeof(svr_proto_t);
				pack_h(s_pkg_, ppi->teamid, idx);
				pack_h(s_pkg_, (uint32_t)ppi->type, idx);
				init_proto_head(s_pkg_, pkinfo.teamid, idx, 0, proto_onli_auto_challenge, 0);
				send_pkg_to_client(fdsess, s_pkg_, idx);
				s_start_.insert(pair<userid_t, pkwait_info_t>(pkinfo.teamid, *ppi));
				pkaccept_info_t pi = {pkinfo.teamid, 0};
				s_accept_.insert(pair<userid_t, pkaccept_info_t>(ppi->teamid, pi));
				REMOVE_TIMERS(ppi);
				memset(ppi, 0, sizeof(pkwait_info_t));
				INIT_LIST_HEAD(&ppi->timer_list);
			}
		}
	} else if ((itacpt = s_accept_.find(uid)) != s_accept_.end()) {
		s_start_.erase(itacpt->second.uid);
		if (inbattle) {
			s_accept_.erase(uid);
		} else {
			int idx = sizeof(svr_proto_t);
			pack_h(s_pkg_, itacpt->second.btid, idx);
			init_proto_head(s_pkg_, uid, idx, 0, proto_onli_clear_btr_info, 0);
			send_pkg_to_client(fdsess, s_pkg_, idx);
			s_accept_.erase(uid);
		}
	}
	s_pkwait_.erase(uid);
	for (int loop = 0; loop < pklv_phase_max; loop ++) {
		if (s_single_pk_[loop].teamid == uid) {
			REMOVE_TIMERS(&s_single_pk_[loop]);
			memset(&s_single_pk_[loop], 0, sizeof(pkwait_info_t));
			INIT_LIST_HEAD(&s_single_pk_[loop].timer_list);
			return;
		}
		for (int k = 0; k < pk_team_cnt_max; k ++) {
			if (s_team_pk_[k][loop].teamid == uid) {
				REMOVE_TIMERS(&s_team_pk_[k][loop]);
				memset(&s_team_pk_[k][loop], 0, sizeof(pkwait_info_t));
				INIT_LIST_HEAD(&s_team_pk_[k][loop].timer_list);
				return;
			}
		}

		for (int k = 0; k < pk_pet_cnt_max; k ++) {
			if (s_pet_pk_[k][loop].teamid == uid) {
				REMOVE_TIMERS(&s_pet_pk_[k][loop]);
				memset(&s_pet_pk_[k][loop], 0, sizeof(pkwait_info_t));
				INIT_LIST_HEAD(&s_pet_pk_[k][loop].timer_list);
				return;
			}
		}
	}
	return;
}

int Online::users_online_status(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	// pkg->body includes the user's friend list
	CHECK_VAL_GE(bodylen, sizeof(friend_list_t) + sizeof(userid_t));

	friend_list_t* friendlist = reinterpret_cast<friend_list_t*>(pkg->body);

	CHECK_VAL(bodylen, sizeof(friend_list_t) + friendlist->cnt * sizeof(userid_t));

	typedef vector<userid_t> usrid_vec_t;
	typedef map<uint32_t, usrid_vec_t> usr_online_map_t;// map of online id and the friends' id

	usr_online_map_t usr_online;
	for (uint32_t i = 0; i != friendlist->cnt; ++i) {
		uint32_t online_id = get_user_online_id(friendlist->uid[i]);
		if (online_id) {
			usr_online[online_id].push_back(friendlist->uid[i]);
		}
	}

	int idx = sizeof(svr_proto_t);
	pack_h(s_pkg_, static_cast<uint16_t>(1), idx); // domain count
	pack_h(s_pkg_, static_cast<uint16_t>(0), idx); // domain id
	pack_h(s_pkg_, static_cast<uint32_t>(usr_online.size()), idx); // online count
	for (usr_online_map_t::iterator it = usr_online.begin();
			it != usr_online.end(); ++it) {
		pack_h(s_pkg_, it->first, idx); // online id
		pack_h(s_pkg_, static_cast<uint32_t>(it->second.size()), idx); // friend count
		for (usrid_vec_t::size_type i = 0; i != it->second.size(); ++i) {
			pack_h(s_pkg_, it->second[i], idx); // friend id
		}
	}
	init_proto_head(s_pkg_, pkg->id, idx, pkg->seq, pkg->cmd, 0);

	return send_pkg_to_client(fdsess, s_pkg_, idx);
}

int Online::chat_across_svr(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL_GE(bodylen, 4);// body = recvid(4 bytes) + package(>0 bytes) 

	userid_t* p_uid     = reinterpret_cast<userid_t*>(pkg->body);
	userid_t  recv_id = *p_uid; // sender id, the first 4 bytes
	uint16_t  online_id = get_user_online_id(recv_id);
	DEBUG_LOG("CHAT ACROSS SVR\t[%u %u %u]", pkg->id, recv_id, online_id);
	if (online_id) {
		// user's online, send msg to the corresponding online server
		*p_uid = recv_id;  // The first 4 bytes set to receiver id
		init_proto_head(pkg, pkg->id, pkg->len, 0, pkg->cmd, 0); //initialize header
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
	} else if(recv_id == 2){//发送给交易地图
		*p_uid = recv_id;  // The first 4 bytes set to receiver id
		init_proto_head(pkg, pkg->id, pkg->len, 0, pkg->cmd, 0); //initialize header
		for (uint32_t i = 0; i != s_max_online_id_; ++i) {
			if (s_online_info_[i].online_id) {
				send_pkg_to_client(s_online_info_[i].fdsess, pkg, pkg->len);
			}
		}
	} else {
		// user's offline, send offline msg to db proxy
		*p_uid = bodylen - 4;
		send_request_to_db(recv_id, dbcmd_offline_msg, pkg->body, bodylen);
	}

	return 0;
}



int Online::do_proto_across_svr(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL_GE(bodylen, 4);// body = recvid(4 bytes) + package(>0 bytes) 

	userid_t* p_uid     = reinterpret_cast<userid_t*>(pkg->body);
	userid_t  recv_id = *p_uid; // sender id, the first 4 bytes
	uint16_t  online_id = get_user_online_id(recv_id);
	DEBUG_LOG("CHAT ACROSS SVR\t[%u %u %u]", pkg->id, recv_id, online_id);
	if (online_id) {
		// user's online, send msg to the corresponding online server
		*p_uid = recv_id;  // The first 4 bytes set to receiver id
		init_proto_head(pkg, pkg->id, pkg->len, 0, pkg->cmd, 0); //initialize header
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
	}

	return 0;
}

int Online::onli_sync_beast(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL_GE(bodylen, 4);

	int beast_cnt_before = s_beast_cnt_;

	if (Online::s_beast_cnt_ == -1) {
		return send_request_to_db(begin_user_id, dbcmd_get_val, NULL, 0);
	} else {
		uint32_t count = *(uint32_t *)(pkg->body);
		DEBUG_LOG("ADD BEAST\t[%u %u]", count, s_beast_cnt_);
		if (count == 0 || s_beast_cnt_ >= beast_count_max) {
			*(uint32_t *)(pkg->body) = Online::s_beast_cnt_;
			init_proto_head(pkg, pkg->id, pkg->len, 0, pkg->cmd, 0);
			send_pkg_to_client(fdsess, pkg, pkg->len);
		} else {
			if (s_beast_cnt_ + count > beast_count_max)
				count = beast_count_max - s_beast_cnt_;
			s_beast_cnt_ += count;
			uint32_t buf[2];
			buf[0] = beast_val_id;
			buf[1] = beast_count_max;
			send_request_to_db(0, dbcmd_add_val, buf, sizeof(buf));
		}
	}

	if ((beast_cnt_before != s_beast_cnt_)) {
		broadcast_beast_count();
	}
	
	return 0;
}

void Online::broadcast_beast_count()
{
	uint8_t buf[32];
	svr_proto_t* pkg = (svr_proto_t *)buf;
	int len = sizeof(svr_proto_t) + 4;

	*(uint32_t *)(pkg->body) = s_beast_cnt_;
	//DEBUG_LOG("BROAD BEAST CNT\t[%u]", s_beast_cnt_);
	init_proto_head(pkg, 0, len, 0, proto_onli_sync_beast, 0);
	for (uint32_t loop = 0; loop < s_max_online_id_; loop ++) {
		if (s_online_info_[loop].fdsess) {
			send_pkg_to_client(Online::s_online_info_[loop].fdsess, pkg, pkg->len);
		}
	}
}

int Online::noti_across_svr(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL_GE(bodylen, 4);// body = recvid(4 bytes) + package(>0 bytes) 

	userid_t* p_uid     = reinterpret_cast<userid_t*>(pkg->body);
	userid_t  recv_id = *p_uid; // sender id, the first 4 bytes
	uint16_t  online_id = get_user_online_id(recv_id);
	if (online_id) {
		// user's online, send msg to the corresponding online server
		*p_uid = recv_id;  // The first 4 bytes set to receiver id
		init_proto_head(pkg, pkg->id, pkg->len, 0, pkg->cmd, 0); //initialize header
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
	}

	return 0;
}

int Online::chk_user_location(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct location_req_t {
		userid_t sender_id;
		userid_t checkee_id;
	} __attribute__((packed));

	CHECK_VAL(bodylen, sizeof(location_req_t));

	location_req_t* req = reinterpret_cast<location_req_t*>(pkg->body);

	int      idx        = sizeof(svr_proto_t);
	uint32_t online_id  = get_user_online_id(req->checkee_id);
	/* The follwing two statements for  building the response protocol package to online server */
	pack_h(s_pkg_, online_id, idx); // build package body
									//+ After this function, idx changes to the size of the package to send
	init_proto_head(s_pkg_, pkg->id, idx, pkg->seq, pkg->cmd, 0); // initialize package header

	return send_pkg_to_client(fdsess, s_pkg_, idx);
}

int Online::online_keepalive(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	int idx = s_fd_map_[fdsess->fd] - 1;
	CHECK_VAL_GE(idx, 0);

	mod_expire_time(s_keepalive_tmr_[idx].tmr, get_now_tv()->tv_sec + 70);

	uint8_t buf[32];
	svr_proto_t* ppkg = (svr_proto_t *)buf;
	int len = sizeof(svr_proto_t) + 4;

	*(uint32_t *)(ppkg->body) = s_beast_cnt_;
	//DEBUG_LOG("BROAD BEAST CNT\t[%u]", s_beast_cnt_);
	init_proto_head(ppkg, 0, len, 0, proto_onli_sync_beast, 0);
	send_pkg_to_client(fdsess, ppkg, ppkg->len);

//	DEBUG_LOG("KEEP ALIVE\t[fd=%d id=%u ip=%s port=%d]",
//				fdsess->fd, s_online_info_[idx].online_id,
//				s_online_info_[idx].online_ip, s_online_info_[idx].online_port);
	return 0;
}

int Online::challenge_battle(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct battle_req_t {
		battle_id_t	btid;
		userid_t	challengee;
	} __attribute__((packed));

	CHECK_VAL(bodylen, sizeof(battle_req_t));

	battle_req_t* req = reinterpret_cast<battle_req_t*>(pkg->body);

	int      idx        = sizeof(svr_proto_t);
	uint32_t online_id  = get_user_online_id(req->challengee);
	
	/* The follwing two statements for  building the response protocol package to online server */
	if (online_id == 0) {
		ERROR_LOG("cannot find challengee\t[uid=%u]", req->challengee);
		init_proto_head(s_pkg_, pkg->id, idx, pkg->seq, pkg->cmd, 200003);
		send_pkg_to_client(fdsess, s_pkg_, idx);
	} else {
		DEBUG_LOG("000000 send pkg_len:%u", idx );
		init_proto_head(s_pkg_, pkg->id, idx, pkg->seq, pkg->cmd, 0);
		send_pkg_to_client(fdsess, s_pkg_, idx);

		//DEBUG_LOG("000000 send pkg_len:%u", idx );
		pack_h(s_pkg_, req->btid , idx);
		//DEBUG_LOG("000000 send pkg_len:%u", idx );
		//pack_h(s_pkg_, uint32_t (req->btid&0xFFFFFFFF) , idx);
		//pack(s_pkg_, &req->challenger, sizeof(req->challenger), idx);
		DEBUG_LOG("11111111111111 send pkg_len:%u", idx );
		init_proto_head(s_pkg_, req->challengee, idx, 0, proto_challenge_battle_onli, 0);
		DEBUG_LOG("CHALLENGE UID\t[uid=%u olid=%u]", req->challengee, online_id);
		send_pkg_to_online(online_id, s_pkg_, idx);
	}
	return 0;
}

pkwait_info_t* Online::try_to_match(pkwait_info_t* ppi, uint32_t& match)
{
	DEBUG_LOG("try to match [%u]",ppi->teamid);
	int lvidx = (ppi->level - 1) / pklv_per_phase;
	pkwait_info_t* ppret = NULL;
	pkwait_info_t* ppi_next = NULL;
	pkwait_info_t* ppi_pre = NULL;
	switch (ppi->type) {
	case 0:
		ppret = &s_single_pk_[lvidx];
		ppi_next = (lvidx < (pklv_per_phase - 1)) ? &s_single_pk_[lvidx + 1] : NULL;
		ppi_pre = lvidx > 0 ? &s_single_pk_[lvidx-1] : NULL;
		break;
	case 1:
		ppret = &s_team_pk_[ppi->count - 1][lvidx];
		ppi_next = (lvidx < (pklv_per_phase - 1)) ? &s_team_pk_[ppi->count - 1][lvidx + 1] : NULL;
		ppi_pre = lvidx > 0 ? &s_team_pk_[ppi->count-1][lvidx-1] : NULL;
		break;
	case 2:
		ppret = &s_pet_pk_[ppi->count - 1][lvidx];
		ppi_next = (lvidx < (pklv_per_phase - 1)) ? &s_pet_pk_[ppi->count - 1][lvidx + 1] : NULL;
		ppi_pre = lvidx > 0 ? &s_pet_pk_[ppi->count-1][lvidx-1] : NULL;
		break;
	/*case 3:
		{
			std::map<uint32_t,uint32_t>::iterator it=s_gvg_challengee_.find(ppi->teamid);
			uint32_t uid= it!=s_gvg_challengee_.end() ? it->second : 0;
			DEBUG_LOG("last challengee [%u]",uid);
			ppret = s_gvg_pk_[0][lvidx].teamid && s_gvg_pk_[0][lvidx].teamid != uid ? &s_gvg_pk_[0][lvidx]:NULL;
			ppi_next = (lvidx < (pklv_per_phase - 1)) && s_gvg_pk_[0][lvidx+1].teamid  && s_gvg_pk_[0][lvidx+1].teamid!=uid ? 
					&s_gvg_pk_[0][lvidx + 1] : NULL;
			ppi_pre = lvidx > 0 &&  s_gvg_pk_[0][lvidx-1].teamid && s_gvg_pk_[0][lvidx-1].teamid!=uid ? &s_gvg_pk_[0][lvidx-1] : NULL;
			s_gvg_challengee_.erase(it->first);
			s_gvg_challengee_.erase(it->second);
			break;

		}*/
	default:
		return NULL;
	}
	if (ppret && ppret->teamid) {
		match = match_succ;
	} else if (ppi_next && ppi_next->teamid && (ppi_next->level - ppi->level) <= 5) {
		match = match_succ;
		ppret = ppi_next;
	} else if (ppi_pre && ppi_pre->teamid && (ppi->level - ppi_pre->level) <= 5) {
		match = match_succ;
		ppret = ppi_pre;
	} else {
		match = match_fail;
		//if(ppi->type==3 && s_gvg_pk_[0][lvidx].teamid != 0)
		//	ppret = &s_pet_pk_[1][lvidx];
		ppret->teamid = ppi->teamid;
		ppret->level = ppi->level;
		ppret->count = ppi->count;
		ppret->type = ppi->type;
		DEBUG_LOG("ADD TIMER FOM MATCH AGAIN\t[%u %u %u %u]", ppi->teamid, ppi->level, ppi->count, ppi->type);
		ADD_TIMER_EVENT_EX(ppret, n_try_match_again, NULL, get_now_tv()->tv_sec + 30);
		s_pkwait_.insert(ppret->teamid);
	}
	return ppret;
}

int Online::onli_apply_pk(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct pk_req_t {
		uint32_t	type;
		uint32_t	level;
		uint32_t	count;
	} __attribute__((packed));
	CHECK_VAL(bodylen, sizeof(pk_req_t));

	pk_req_t* req = reinterpret_cast<pk_req_t*>(pkg->body);
	int idx = sizeof(svr_proto_t);
	if (s_pkwait_.find(pkg->id) != s_pkwait_.end() \
		|| !req->level || req->level > pklv_per_phase * pklv_phase_max \
		|| !req->count || (req->type == 0 && req->count > pk_team_cnt_max) \
		|| (req->type == 2 && req->count > pk_pet_cnt_max)) {
		ERROR_LOG("apply pk fail\t[%u %u %u %u]", pkg->id, req->type, req->level, req->count);	
		pack_h(s_pkg_, (uint32_t)apply_fail, idx);
		init_proto_head(s_pkg_, pkg->id, idx, 0, pkg->cmd, 0);
		send_pkg_to_client(fdsess, s_pkg_, idx);
	} else {
		DEBUG_LOG("APPLY PK SUCC\t[%u %u %u %u]", pkg->id, req->type, req->level, req->count);
		uint32_t match;
		pkwait_info_t pkreq;
		pkreq.teamid = pkg->id;
		pkreq.type = req->type;
		pkreq.level = req->level;
		pkreq.count = req->count;
		INIT_LIST_HEAD(&pkreq.timer_list);
		pkwait_info_t* ppi = try_to_match(&pkreq, match);
		pack_h(s_pkg_, match, idx);
		init_proto_head(s_pkg_, pkg->id, idx, 0, pkg->cmd, 0);
		send_pkg_to_client(fdsess, s_pkg_, idx);
		if (match == match_succ) {
			/*if(req->type == 3)//gvg pk匹配成功添加对手id
			{
				DEBUG_LOG("gvg match succ [%u %u]",pkg->id,ppi->teamid);
				s_gvg_challengee_.insert(pair<uint32_t,uint32_t>(pkg->id,ppi->teamid));
				s_gvg_challengee_.insert(pair<uint32_t,uint32_t>(ppi->teamid,pkg->id));
			}*/
			idx = sizeof(svr_proto_t);
			pack_h(s_pkg_, ppi->teamid, idx);
			pack_h(s_pkg_, (uint32_t)ppi->type, idx);
			init_proto_head(s_pkg_, pkg->id, idx, 0, proto_onli_auto_challenge, 0);
			send_pkg_to_client(fdsess, s_pkg_, idx);
			s_start_.insert(pair<userid_t, pkwait_info_t>(pkg->id, *ppi));
			pkaccept_info_t pi = {pkg->id, 0};
			s_accept_.insert(pair<userid_t, pkaccept_info_t>(ppi->teamid, pi));
			ppi->teamid = 0;
			ppi->level = 0;
			REMOVE_TIMERS(ppi);
			INIT_LIST_HEAD(&ppi->timer_list);
		}
	}
	return 0;
}

int Online::onli_auto_accept(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct battle_req_t {
		battle_id_t	btid;
		userid_t	challengee;
	} __attribute__((packed));

	CHECK_VAL(bodylen, sizeof(battle_req_t));

	battle_req_t* req = reinterpret_cast<battle_req_t*>(pkg->body);

	int      idx        = sizeof(svr_proto_t);
	uint32_t online_id  = get_user_online_id(req->challengee);
	
	/* The follwing two statements for  building the response protocol package to online server */
	if (online_id) {
		pk_accept_t::iterator it = s_accept_.find(req->challengee);
		if (it != s_accept_.end()) {
			it->second.btid = req->btid;
		} else {
			ERROR_LOG("can't find accept info\t[%u %u]", pkg->id, req->challengee);
			pkaccept_info_t pi = {pkg->id, req->btid};
			s_accept_.insert(pair<userid_t, pkaccept_info_t>(req->challengee, pi));
		}
		pack_h(s_pkg_, req->btid, idx);
		init_proto_head(s_pkg_, req->challengee, idx, 0, proto_onli_auto_fight, 0);
		DEBUG_LOG("CHALLENGE UID\t[uid=%u olid=%u]", req->challengee, online_id);
		send_pkg_to_online(online_id, s_pkg_, idx);
		s_start_.erase(pkg->id);
	}
	return 0;
}

int Online::onli_auto_fight(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen, 4);
	userid_t uid = *(userid_t*)pkg->body;
	s_accept_.erase(pkg->id);
	s_pkwait_.erase(pkg->id);
	s_pkwait_.erase(uid);
	return 0;
}


int Online::onli_cancel_pk_apply(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen, 0);

	int ret = 0;
	DEBUG_LOG("CANCEL APPLY\t[%u ]", pkg->id);

	if (s_pkwait_.find(pkg->id) == s_pkwait_.end()) {
		ret = 200072;
	} else if (s_start_.find(pkg->id) != s_start_.end() \
		|| s_accept_.find(pkg->id) != s_accept_.end()) {
		ret = 200073;
	}

	if (!ret) {
		pkwait_info_t* ppi = get_pkwait_info(pkg->id);
		if (ppi) {
			REMOVE_TIMERS(ppi);
			memset(ppi, 0, sizeof(pkwait_info_t));
			INIT_LIST_HEAD(&ppi->timer_list);			
			s_pkwait_.erase(pkg->id);
		} else {
			ret = 200000;
		}
	}

	int idx = sizeof(svr_proto_t);
	init_proto_head(s_pkg_, pkg->id, idx, 0, proto_onli_cancel_pk_apply, ret);
	send_pkg_to_client(fdsess, s_pkg_, idx);

	return 0;
}


void Online::clear_online_info(int fd)
{
	uint16_t online_id = s_fd_map_[fd];

	if (online_id) {
		int idx = online_id - 1;

		DEBUG_LOG("ONLINE CLOSE CONN\t[id=%u %u ip=%.16s]",
			online_id, s_online_info_[idx].online_id, s_online_info_[idx].online_ip);

		s_fd_map_.erase(fd);

		for (userid_set_t::iterator it = s_users_set[idx].begin(); it != s_users_set[idx].end(); ++it)  {
			s_users.erase(*it);
		}
		s_users_set[idx].clear();
		s_keepalive_tmr_[idx].tmr = 0;
		REMOVE_TIMERS(&(s_keepalive_tmr_[idx]));
		memset(&(s_online_info_[idx]), 0, sizeof(s_online_info_[0]));
		// reset max online id
		if (online_id == s_max_online_id_) {
			uint32_t i = s_max_online_id_ - 1;
			while (i && (s_online_info_[i - 1].online_id == 0)) {
				--i;
			}
			s_max_online_id_ = i;
		}
	}
}

//----------------------------------------------------------
// For Login Server
//
// get recommended server list
typedef std::set<uint32_t> set_uint32_t;
typedef struct{uint32_t count; set_uint32_t server;} s_group_t;
typedef std::map<in_addr_t,s_group_t> s_group_map_t;

int Online::get_recommeded_svrlist_new(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL_GE(pkg->len, sizeof(svr_proto_t) + 11); // DomainID(2)+OM_Server(1)+online(4)+FriendCnt(4)

	uint32_t prev_online_id = *(reinterpret_cast<uint32_t*>(pkg->body + 3));
	friend_list_t* friendlist = reinterpret_cast<friend_list_t*>(pkg->body + 7);// FriendCnt + Friend List
	CHECK_VAL_GE(pkg->len, sizeof(svr_proto_t) + 11 + friendlist->cnt * sizeof(userid_t));

	// get hot servers

	uint32_t mix;
	uint32_t total = 0;
	s_group_map_t online_grpmap;
	const online_info_t *s = NULL;
	//group online by addr
	for (uint32_t i = 0; i < s_max_online_id_; i++) {
		s = get_online_info(i + 1);
		if (!s) continue;
		if (s->online_id != prev_online_id ) {
			total += s->user_num + 1;
			mix = (s->user_num << 16) | s->online_id;
			in_addr_t addr = inet_addr(s->online_ip);
			s_group_map_t::iterator it = online_grpmap.find(addr);
			if(it != online_grpmap.end()) {
				it->second.count += s->user_num + 1;
				it->second.server.insert(mix);
			} else {
				s_group_t item;
				item.count += s->user_num + 1;
				item.server.insert(mix);
				online_grpmap[addr] = item;
			}
		}
	}

	uint32_t total_count = 0;
	s_group_map_t::iterator git;
	//calc weight backwards of online group 
	for(git = online_grpmap.begin();git != online_grpmap.end();git++) {
		git->second.count = total / git->second.count + 1;
		total_count += git->second.count;
	}

	int count = 0;
	set_uint32_t hot_online;
	set_uint32_t::reverse_iterator sit;
	for(git = online_grpmap.begin();git != online_grpmap.end();git++) {
		uint32_t i = 0;
		//calc count of online group
		if (!total_count) total_count = 1;
		git->second.count = git->second.count * 11 / total_count;
		for(sit = git->second.server.rbegin(); i < git->second.count && count < 9 && sit != git->second.server.rend();sit++) {
			mix = *sit;
			if((mix >> 16) < 250) {
				i++;
				count++;
				hot_online.insert(mix);
			}
		}
		total_count = total_count - git->second.count + i;
	}

	int idx = sizeof(svr_proto_t);
	pack_h(s_pkg_, s_max_online_id_, idx);
	pack_h(s_pkg_, static_cast<uint16_t>(0), idx);
	pack_h(s_pkg_, count + 1, idx);

	s = get_online_info(prev_online_id);
	if (s) {
		pack_h(s_pkg_,	s->online_id,	idx);		
		pack_h(s_pkg_,	s->user_num,	idx);
		pack(s_pkg_,	s->online_ip, 	sizeof(s->online_ip), idx);
		pack_h(s_pkg_,	s->online_port, idx);
		pack_h(s_pkg_,	0, idx); // no friends
	} else {
		pack_h(s_pkg_,	0, idx);
		idx += 26;
	}

	set_uint32_t::reverse_iterator iter;
	for (iter = hot_online.rbegin(); iter != hot_online.rend(); iter++) {
		mix = *iter;
		s = get_online_info((mix & 0xFFFF));
		if(s) {
			pack_h(s_pkg_,	s->online_id,	idx);
			pack_h(s_pkg_, 	s->user_num,	idx);
			pack(s_pkg_, 	s->online_ip, 	sizeof(s->online_ip), idx);
			pack_h(s_pkg_,	s->online_port, idx);
			pack_h(s_pkg_,	0, idx); // no friends
		} else {
			pack_h(s_pkg_,	0, idx);
			idx += 26;
		}
	}

	init_proto_head(s_pkg_, pkg->id, idx, pkg->seq, pkg->cmd, 0);
	send_pkg_to_client(fdsess, s_pkg_, idx);

	s = get_online_info( get_user_online_id(pkg->id) );
	if (s) {
		int idx = sizeof(svr_proto_t);
		pack_h(s_pkg_, pkg->id, idx);
		pack_h(s_pkg_, 0x00100001, idx);
		init_proto_head(s_pkg_, pkg->id, idx, 0, proto_kick_usr_offline, 0);
		send_pkg_to_client(s->fdsess, s_pkg_, idx);
	}

	return 0;
}

int Online::get_ranged_svrlist_new(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct svrlist_req_t {
		uint16_t domain_id;
		uint32_t start_id;   // start online id
		uint32_t end_id;     // end online id
		uint32_t friend_cnt;
		userid_t friend_id[];
	} __attribute__((packed));

	CHECK_VAL_GE(bodylen, sizeof(svrlist_req_t));

	svrlist_req_t* svrlist = reinterpret_cast<svrlist_req_t*>(pkg->body);

	CHECK_VAL(bodylen, sizeof(svrlist_req_t) + svrlist->friend_cnt * sizeof(userid_t));

	if ((svrlist->start_id == 0) || (svrlist->end_id > online_num_max)
			|| (svrlist->start_id > svrlist->end_id) || ((svrlist->end_id - svrlist->start_id) > 200)) {
		ERROR_LOG("invalid range: start=%u end=%u", svrlist->start_id, svrlist->end_id);
		return -1;
	}

	// pack server list
	int idx = sizeof(svr_proto_t), cnt_idx;
	pack_h(s_pkg_, s_max_online_id_, idx);
	pack_h(s_pkg_, static_cast<uint16_t>(0), idx);	
	cnt_idx = idx; // remember the index to pack online_cnt
	idx += 4; // reserve space for online_cnt
	uint32_t online_cnt = 0;	
	for (uint32_t i = svrlist->start_id - 1; i != svrlist->end_id; ++i) {
		if (s_online_info_[i].online_id) {
			pack_h(s_pkg_, s_online_info_[i].online_id, idx);
			pack_h(s_pkg_, s_online_info_[i].user_num, idx);
			pack(s_pkg_, s_online_info_[i].online_ip, sizeof(s_online_info_[i].online_ip), idx);
			pack_h(s_pkg_, s_online_info_[i].online_port, idx);
			pack_h(s_pkg_, static_cast<uint32_t>(0), idx);
			++online_cnt;
		}
	}
	pack_h(s_pkg_, online_cnt, cnt_idx);
	init_proto_head(s_pkg_, pkg->id, idx, pkg->seq, pkg->cmd, 0);

	return send_pkg_to_client(fdsess, s_pkg_, idx);
}


//----------------------------------------------------------
// For Adminer Server
//
int Online::syn_vip_flag(svr_proto_t* pkg, uint32_t bodylen, int fd)
{
    CHECK_VAL(bodylen, 25);
	uint16_t online_id = get_user_online_id(pkg->id);
    DEBUG_LOG("SYN VIP OP [%d %d %x]", pkg->id, online_id, *(uint32_t*)pkg->body);
	if (online_id) {
		init_proto_head(pkg, pkg->id, pkg->len, 0, proto_syn_vip_flag, 0);
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
	}

    return 0;
}


int Online::syn_info_to_user (svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{

	struct syn_info_t {
		uint32_t src_userid;
		uint32_t obj_userid;
		uint32_t opt_type;   
		uint32_t v1;    
		uint32_t v2;   
		uint32_t v3;  
		uint32_t v4; 
	} __attribute__((packed));

	CHECK_VAL(bodylen, sizeof(syn_info_t));
	syn_info_t * p_in = reinterpret_cast<syn_info_t*>(pkg->body);



	uint16_t online_id = get_user_online_id(p_in->obj_userid );
    DEBUG_LOG("SYN  INFO OP [ srcid=%u  objid=%u onlineid=%d ]", 
			p_in->src_userid , p_in->obj_userid , online_id );
	if (online_id) {
		init_proto_head(pkg, p_in->obj_userid , pkg->len, 0, proto_syn_info_to_user, 0);
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
	}

    return 0;
}
int Online::syn_vip_exp(svr_proto_t* pkg, uint32_t bodylen, int fd)
{
    CHECK_VAL(bodylen, 6);
	uint16_t online_id = get_user_online_id(pkg->id);
    DEBUG_LOG("SYN VIP EXP OP [%d %d %x]", pkg->id, online_id, *(uint32_t*)pkg->body);
	if (online_id) {
		init_proto_head(pkg, pkg->id, pkg->len, 0, proto_syn_vip_exp, 0);
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
	}

    return 0;
}

int Online::kick_user_offline(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen, 8);

	userid_t uid;
	int idx = 0;
	unpack_h(pkg->body, uid, idx); // unpack user id

	DEBUG_LOG("KICK USER OFFLINE\t[%u]", uid);

	uint16_t online_id = get_user_online_id(uid);
	if (online_id) {
		init_proto_head(pkg, 0, pkg->len, 0, proto_kick_usr_offline, 0);
		// No response package to adminer server, so ignore "fdsess"(adminer server information),
		//+ just send package to destination online server
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
	}

	return 0;
}

int Online::broadcast_msg(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct msg_t {
		uint32_t online_id; // 0 for all online servers
		userid_t uid;    // receiver id, 0 for all users
		uint32_t type;   // message type
		uint16_t msglen;
		uint8_t  msg[];
	} __attribute__((packed));

	CHECK_VAL_GE(bodylen, sizeof(msg_t));

	msg_t* msg = reinterpret_cast<msg_t*>(pkg->body);

	CHECK_VAL_GE(bodylen, sizeof(msg_t) + msg->msglen + 4);

	uint32_t dbmsglen; // size of "dbmsglen" itself(4 bytes) included
	int idx = sizeof(msg_t) + msg->msglen;
	unpack_h(pkg->body, dbmsglen, idx);

	CHECK_VAL(bodylen, sizeof(msg_t) + msg->msglen + dbmsglen);

	// pack msg to send to the user(s)
	idx = sizeof(svr_proto_t);
	// online_id is not necessary to be included in the dispathed packages,
	//+ here "pkg->body+4" means discarding online_id
	pack(s_pkg_, pkg->body + 4, bodylen - 4 - dbmsglen, idx);
	init_proto_head(s_pkg_, 0, idx, 0, proto_broadcast_msg, 0);
	DEBUG_LOG("broadcast_msg ");
	if (msg->uid) { // send msg to a given user
		uint16_t online_id = get_user_online_id(msg->uid);
		if (online_id) {
			send_pkg_to_client(s_online_info_[online_id - 1].fdsess, s_pkg_, idx);
		} else {
			idx = 0; //no header
			pack(s_pkg_, pkg->body + sizeof(msg_t) + msg->msglen, dbmsglen, idx);
			send_request_to_db(msg->uid, dbcmd_offline_msg, s_pkg_, idx);
		}
	} else if (msg->online_id) { // send msg to all users of a given online server
		send_pkg_to_online(msg->online_id, s_pkg_, idx);
	} else { // send msg to all users of all the online servers
		for (uint32_t i = 0; i != s_max_online_id_; ++i) {
			if (s_online_info_[i].online_id) {
				send_pkg_to_client(s_online_info_[i].fdsess, s_pkg_, idx);
			}
		}
	}

	return 0;
}

int Online::create_npc(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct npc_t {
		uint32_t online_id;
		uint32_t npclen;
		uint8_t  npc[];
	} __attribute__((packed));

	CHECK_VAL_GE(bodylen, sizeof(npc_t));

	npc_t* npc = reinterpret_cast<npc_t*>(pkg->body);

	CHECK_VAL(bodylen, sizeof(npc_t) + npc->npclen - 4);

	// pack npc and send to online
	int idx = sizeof(svr_proto_t);
	pack(s_pkg_, pkg->body + sizeof(npc_t), npc->npclen - 4, idx);
	init_proto_head(s_pkg_, 0, idx, 0, proto_create_npc, 0);

	if (npc->online_id == 0) {
		for (uint32_t i = 0; i != s_max_online_id_; ++i) {
			if (s_online_info_[i].online_id) {
				DEBUG_LOG("CREATE NPC\t[olid=%u]", i + 1);
				send_pkg_to_client(s_online_info_[i].fdsess, s_pkg_, idx);
			}
		}
	} else {
		DEBUG_LOG("CREATE NPC\t[olid=%u]", npc->online_id);
		send_pkg_to_online(npc->online_id, s_pkg_, idx);
	}

	return 0;
}

int Online::make_online_dead(void* owner, void* data)
{
	online_info_t* on_info = reinterpret_cast<online_info_t*>(data);

	DEBUG_LOG("ONLINE DEAD\t[fd=%d id=%u ip=%s port=%d]",
				on_info->fdsess->fd, on_info->online_id,
				on_info->online_ip, on_info->online_port);

	close_client_conn(on_info->fdsess->fd);
	return 0;
}

int Online::report_usr_num(void* owner, void* data)
{
    int cnt    = 0;
	int dx_cnt = 0;
	int wt_cnt = 0;
    for(uint32_t i = 0; i < s_max_online_id_; i++) {
        cnt += s_online_info_[i].user_num;
		if (s_online_info_[i].domain_id == 0)
			dx_cnt += s_online_info_[i].user_num;
		else
			wt_cnt += s_online_info_[i].user_num;
    }

    msglog(s_statistic_file_, s_idc_ ? 0x0B00000D : 0x0B000002, get_now_tv()->tv_sec, &cnt, 4);
	//msglog(s_statistic_file_, 0x020A0001, get_now_tv()->tv_sec, &dx_cnt, 4);
	//msglog(s_statistic_file_, 0x020A0002, get_now_tv()->tv_sec, &wt_cnt, 4);
    if (!ADD_TIMER_EVENT_EX(&s_statistic_tmr_, n_report_usr_num, NULL, get_now_tv()->tv_sec + 60)){
		ERROR_RETURN(("add timer fail"), -1);
	};

	return 0;
}

int Online::get_val_from_db(void* owner, void* data)
{
	send_request_to_db(begin_user_id, dbcmd_get_val, NULL, 0);
	return 0;
}

int Online::start_refresh(void* owner, void* data)
{
	uint32_t nowsec = (get_now_tv()->tv_sec + 3600 * 8) % (3600 * 24);
	uint32_t refreshtime = 86400 - nowsec + 2;
	ADD_TIMER_EVENT_EX(&s_statistic_tmr_, n_start_refresh, NULL, get_now_tv()->tv_sec + 86400);
	ADD_TIMER_EVENT_EX(&s_statistic_tmr_, n_get_val_from_db, NULL, get_now_tv()->tv_sec + refreshtime);
	return 0;
}

pkwait_info_t* Online::match_again_single(pkwait_info_t* ppi)
{
	pkwait_info_t* ppi_ret = NULL;
	int lvidx = (ppi->level - 1) / pklv_per_phase;
	int lvdiff = 100;
	if (lvidx < (pklv_phase_max - 1) && s_single_pk_[lvidx + 1].teamid) {
		lvdiff = s_single_pk_[lvidx + 1].level - ppi->level;
		ppi_ret = &s_single_pk_[lvidx + 1];
	}

	if (lvidx > 0 && s_single_pk_[lvidx - 1].teamid) {
		int lvd = ppi->level - s_single_pk_[lvidx - 1].level;
		if (lvd < lvdiff) {
			ppi_ret = &s_single_pk_[lvidx - 1];
			lvdiff = lvd;
		}
	}

	if (!ppi_ret) {
		if (lvidx < (pklv_phase_max - 2) && s_single_pk_[lvidx + 2].teamid) {
			lvdiff = s_single_pk_[lvidx + 2].level - ppi->level;
			ppi_ret = lvdiff <= 10 ? &s_single_pk_[lvidx + 2] : ppi_ret;
		}

		if (lvidx > 1 && s_single_pk_[lvidx - 2].teamid) {
			int lvd = ppi->level - s_single_pk_[lvidx - 2].level;
			if (lvd < lvdiff && lvd <= 10) {
				ppi_ret = &s_single_pk_[lvidx - 2];
				lvdiff = lvd;
			}
		}
	}
	return ppi_ret;
}

pkwait_info_t* Online::match_again_team(pkwait_info_t* ppi)
{
	pkwait_info_t* ppi_ret = NULL;
	int lvidx = (ppi->level - 1) / pklv_per_phase;
	DEBUG_LOG("MATCH AGAIN TEAM\t[%u %u %u]", ppi->teamid, lvidx, ppi->count);

	for (uint32_t cnt = 0; cnt < 3; cnt ++) {
		if (!cnt) {
			DEBUG_LOG("MATCH SAME CNT");
			pkwait_info_t* ppit = (lvidx < pklv_phase_max - 1) ? &s_team_pk_[ppi->count - 1][lvidx + 1] : NULL;
			ppi_ret = ppit && ppit->teamid ? ppit : NULL;
			uint32_t lvdiff = ppi_ret ? (ppi_ret->level - ppi->level) : 100;
			ppit = (lvidx > 0) ? &s_team_pk_[ppi->count - 1][lvidx - 1] : NULL;
			if (ppit && ppit->teamid) {
				ppi_ret = (!ppi_ret || (ppi_ret && lvdiff > ppi->level - ppit->level)) ? ppit : ppi_ret;
				lvdiff = (ppi_ret == ppit) ? (ppi->level - ppit->level) : lvdiff;
			}
			ppit = (lvidx < pklv_phase_max - 2) ? &s_team_pk_[ppi->count - 1][lvidx + 2] : NULL;
			if (ppit && ppit->teamid && (ppit->level - ppi->level) <= 10) {
				ppi_ret = (!ppi_ret || (ppi_ret && lvdiff > ppit->level - ppi->level)) ? ppit : ppi_ret;
				lvdiff = (ppi_ret == ppit) ? (ppit->level - ppi->level) : lvdiff;
			}
			ppit = (lvidx > 1) ? &s_team_pk_[ppi->count - 1][lvidx - 2] : NULL;
			if (ppit && ppit->teamid && (ppi->level - ppit->level) <= 10) {
				ppi_ret = (!ppi_ret || (ppi_ret && lvdiff > ppi->level - ppit->level)) ? ppit : ppi_ret;
				lvdiff = (ppi_ret == ppit) ? (ppi->level - ppit->level) : lvdiff;
			}
			if (ppi_ret) return ppi_ret;
		}

		if (cnt && ppi->count + cnt <= pk_team_cnt_max) {
			DEBUG_LOG("MATCH CNT INCR\t[%u]", cnt);
			ppi_ret = lvidx > 0 ? &s_team_pk_[ppi->count + cnt - 1][lvidx - 1] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
			ppi_ret = &s_team_pk_[ppi->count + cnt - 1][lvidx];
			if (ppi_ret->teamid) return ppi_ret;
			ppi_ret = lvidx < (pklv_phase_max - 1) ? &s_team_pk_[ppi->count + cnt - 1][lvidx + 1] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
			ppi_ret = lvidx > 1 ? &s_team_pk_[ppi->count + cnt - 1][lvidx - 2] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
			ppi_ret = lvidx < (pklv_phase_max - 2) ? &s_team_pk_[ppi->count + cnt - 1][lvidx + 2] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
		}

		if (cnt && ppi->count > cnt) {
			DEBUG_LOG("MATCH CNT DECR\t[%u]", cnt);
			ppi_ret = (lvidx < pklv_phase_max - 1) ? &s_team_pk_[ppi->count - cnt - 1][lvidx + 1] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
			ppi_ret = &s_team_pk_[ppi->count - cnt - 1][lvidx];
			if (ppi_ret->teamid) return ppi_ret;
			ppi_ret = lvidx > 0 ? &s_team_pk_[ppi->count - cnt - 1][lvidx - 1] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
			ppi_ret = lvidx < (pklv_phase_max - 2) ? &s_team_pk_[ppi->count - cnt - 1][lvidx + 2] : NULL;
			if (ppi_ret && ppi_ret->teamid && (ppi_ret->level - ppi->level) <= 10) return ppi_ret;
			ppi_ret = lvidx > 1 ? &s_team_pk_[ppi->count - cnt - 1][lvidx - 2] : NULL;
			if (ppi_ret && ppi_ret->teamid && (ppi->level - ppi_ret->level) <= 10) return ppi_ret;
		}
	}
	return NULL;
}

pkwait_info_t* Online::match_again_pet(pkwait_info_t* ppi)
{
	pkwait_info_t* ppi_ret = NULL;
	int lvidx = (ppi->level - 1) / pklv_per_phase;
		DEBUG_LOG("MATCH AGAIN PET\t[%u %u %u]", ppi->teamid, lvidx, ppi->count);

	for (uint32_t cnt = 0; cnt < 3; cnt ++) {
		if (!cnt) {
			DEBUG_LOG("MATCH SAME CNT");
			pkwait_info_t* ppit = (lvidx < pklv_phase_max - 1) ? &s_pet_pk_[ppi->count - 1][lvidx + 1] : NULL;
			ppi_ret = ppit && ppit->teamid ? ppit : NULL;
			uint32_t lvdiff = ppi_ret ? (ppi_ret->level - ppi->level) : 100;
			ppit = (lvidx > 0) ? &s_pet_pk_[ppi->count - 1][lvidx - 1] : NULL;
			if (ppit && ppit->teamid) {
				ppi_ret = (!ppi_ret || (ppi_ret && lvdiff > ppi->level - ppit->level)) ? ppit : ppi_ret;
				lvdiff = (ppi_ret == ppit) ? (ppi->level - ppit->level) : lvdiff;
			}
			ppit = (lvidx < pklv_phase_max - 2) ? &s_pet_pk_[ppi->count - 1][lvidx + 2] : NULL;
			if (ppit && ppit->teamid && (ppit->level - ppi->level) <= 10) {
				ppi_ret = (!ppi_ret || (ppi_ret && lvdiff > ppit->level - ppi->level)) ? ppit : ppi_ret;
				lvdiff = (ppi_ret == ppit) ? (ppit->level - ppi->level) : lvdiff;
			}
			ppit = (lvidx > 1) ? &s_pet_pk_[ppi->count - 1][lvidx - 2] : NULL;
			if (ppit && ppit->teamid && (ppi->level - ppit->level) <= 10) {
				ppi_ret = (!ppi_ret || (ppi_ret && lvdiff > ppi->level - ppit->level)) ? ppit : ppi_ret;
				lvdiff = (ppi_ret == ppit) ? (ppi->level - ppit->level) : lvdiff;
			}
			if (ppi_ret) return ppi_ret;
		}

		if (cnt && ppi->count + cnt <= pk_pet_cnt_max) {
			DEBUG_LOG("MATCH CNT INCR\t[%u]", cnt);
			ppi_ret = lvidx > 0 ? &s_pet_pk_[ppi->count + cnt - 1][lvidx - 1] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
			ppi_ret = &s_pet_pk_[ppi->count + cnt - 1][lvidx];
			if (ppi_ret->teamid) return ppi_ret;
			ppi_ret = lvidx < (pklv_phase_max - 1) ? &s_pet_pk_[ppi->count + cnt - 1][lvidx + 1] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
			ppi_ret = lvidx > 1 ? &s_pet_pk_[ppi->count + cnt - 1][lvidx - 2] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
			ppi_ret = lvidx < (pklv_phase_max - 2) ? &s_pet_pk_[ppi->count + cnt - 1][lvidx + 2] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
		}

		if (cnt && ppi->count > cnt) {
			DEBUG_LOG("MATCH CNT DECR\t[%u]", cnt);
			ppi_ret = (lvidx < pklv_phase_max - 1) ? &s_pet_pk_[ppi->count - cnt - 1][lvidx + 1] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
			ppi_ret = &s_pet_pk_[ppi->count - cnt - 1][lvidx];
			if (ppi_ret->teamid) return ppi_ret;
			ppi_ret = lvidx > 0 ? &s_pet_pk_[ppi->count - cnt - 1][lvidx - 1] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
			ppi_ret = lvidx < (pklv_phase_max - 2) ? &s_pet_pk_[ppi->count - cnt - 1][lvidx + 2] : NULL;
			if (ppi_ret && ppi_ret->teamid && (ppi_ret->level - ppi->level) <= 10) return ppi_ret;
			ppi_ret = lvidx > 1 ? &s_pet_pk_[ppi->count - cnt - 1][lvidx - 2] : NULL;
			if (ppi_ret && ppi_ret->teamid && (ppi->level - ppi_ret->level) <= 10) return ppi_ret;
		}
	}
	return NULL;
}
/*
pkwait_info_t* Online::match_again_gvg(pkwait_info_t* ppi)
{
	pkwait_info_t* ppi_ret = NULL,*ppi_next=NULL,*ppi_pre=NULL;
	int lvidx = (ppi->level - 1) / pklv_per_phase;
	DEBUG_LOG("MATCH AGAIN GVG\t[%u %u %u]", ppi->teamid, lvidx, ppi->count);
	ppi_ret = s_gvg_pk_[0][lvidx].teamid && s_gvg_pk_[0][lvidx].teamid!=ppi->teamid ? &s_gvg_pk_[0][lvidx]:NULL;
	ppi_next = (lvidx < (pklv_per_phase - 1)) && s_gvg_pk_[0][lvidx+1].teamid  ? 
			&s_gvg_pk_[0][lvidx + 1] : NULL;
	ppi_pre = lvidx > 0 &&  s_gvg_pk_[0][lvidx-1].teamid ? &s_gvg_pk_[0][lvidx-1] : NULL;
	if (ppi_ret) 
		return ppi_ret;	
	else if (ppi_next && (ppi_next->level - ppi->level) <= 5) 
		return ppi_next;
	else if (ppi_pre && ppi_pre->teamid && (ppi->level - ppi_pre->level) <= 5) 
		return ppi_pre;
	s_gvg_pk_[0][lvidx].teamid= ppi->teamid;	
	s_gvg_pk_[0][lvidx].level= ppi->level;	
	s_gvg_pk_[0][lvidx].type= ppi->type;	
	s_gvg_pk_[0][lvidx].count= ppi->count;	
	return NULL;
}
*/
int Online::try_match_again(void* owner, void* data)
{
    pkwait_info_t* ppi = (pkwait_info_t *)owner;
	DEBUG_LOG("TIME OUT TRY MATCH AGAIN\t[%u %u %u %u]", \
		ppi->teamid, ppi->type, ppi->level, ppi->count);

	uint32_t online_id = get_user_online_id(ppi->teamid);
	if (!online_id) {
		ERROR_LOG("user offline\t[%u]", ppi->teamid);
		s_pkwait_.erase(ppi->teamid);
		ppi->teamid = 0;
		ppi->count = 0;
		ppi->level = 0;
		return 0;
	}

	pkwait_info_t* ppi_ret = NULL;
	switch (ppi->type) {
	case 0:
		ppi_ret = match_again_single(ppi);
		break;
	case 1:
		ppi_ret = match_again_team(ppi);
		break;
	case 2:
		ppi_ret = match_again_pet(ppi);
		break;
	default:
		return 0;
	}

	if (ppi_ret) {
		/*if(ppi->type == 3)
		{
			s_gvg_challengee_.insert(pair<uint32_t,uint32_t>(ppi_ret->teamid,ppi->teamid));
			s_gvg_challengee_.insert(pair<uint32_t,uint32_t>(ppi->teamid,ppi_ret->teamid));
		}*/
		DEBUG_LOG("MATCH AGAIN SUCC\t[ppi_ret=%u ppi=%u]", ppi_ret->teamid,ppi->teamid);
		REMOVE_TIMERS(ppi_ret);
		int idx = sizeof(svr_proto_t);
		pack_h(s_pkg_, ppi_ret->teamid, idx);
		pack_h(s_pkg_, (uint32_t)ppi_ret->type, idx);
		init_proto_head(s_pkg_, ppi->teamid, idx, 0, proto_onli_auto_challenge, 0);
		send_pkg_to_online(online_id, s_pkg_, idx);
		s_start_.insert(pair<userid_t, pkwait_info_t>(ppi->teamid, *ppi_ret));
		pkaccept_info_t pi = {ppi->teamid, 0};
		s_accept_.insert(pair<userid_t, pkaccept_info_t>(ppi_ret->teamid, pi));
		memset(ppi_ret, 0, sizeof(pkwait_info_t));
		INIT_LIST_HEAD(&ppi_ret->timer_list);
		memset(ppi, 0, sizeof(pkwait_info_t));
		INIT_LIST_HEAD(&ppi->timer_list);
	}

	return 0;
}

int Online::switch_test(fdsession_t* fdsess,Cmessage *c_in)
{	
	return 0;
}

int Online::switch_add_luckystar_exchange_cnt(fdsession_t* fdsess,Cmessage *c_in)
{	
	switch_add_luckystar_exchange_cnt_in *p_in=P_IN;
	switch_add_luckystar_exchange_cnt_out out; 
	out.ret=-1;
	uint32_t count=lvdb.get_value_uint32("global_data",g_luckstar_exchange_cnt);
	DEBUG_LOG("set_global_data_cnt %u %u",p_in->max,count);
	if(count < p_in->max){
		out.ret=0;
		count++;
		lvdb.set_value_uint32("global_data",g_luckstar_exchange_cnt,count,0);
	}
	send_msg_to_user(fdsess,p_in->uid, switch_add_luckystar_exchange_cnt_cmd, &out);
	return 0;
}

void Online::init()
{
	int ok = -1;

	srand(time(0));

	s_max_uid_ = config_get_intval("max_uid", 0);
	s_idc_ = config_get_intval("msg_log_idc", 0);
	
	s_statistic_file_ = config_get_strval("statistic_file");

	if (s_max_uid_ <= 0) {
		s_max_uid_ = 1000000000;
	}

    for (int i = 0; i != online_num_max; ++i) {
        s_users_set[i].clear();
    }

    if(s_statistic_file_) {
        INIT_LIST_HEAD(&s_statistic_tmr_.timer_list);
        if (!ADD_TIMER_EVENT_EX(&s_statistic_tmr_, n_report_usr_num, NULL, get_now_tv()->tv_sec + 60)) {
			ERROR_LOG("add timer event fail");
			goto fail;
		}
    }

	ADD_TIMER_EVENT_EX(&s_statistic_tmr_, n_get_val_from_db, NULL, get_now_tv()->tv_sec + 3);

	s_pkwait_.clear();
	s_start_.clear();
	s_accept_.clear();
	for (int loop = 0; loop < pklv_phase_max; loop ++) {
		memset(&s_single_pk_[loop], 0, sizeof(pkwait_info_t));
		INIT_LIST_HEAD(&s_single_pk_[loop].timer_list);

		for (int l = 0; l < pk_team_cnt_max; l ++) {
			memset(&s_team_pk_[l][loop], 0, sizeof(pkwait_info_t));
			INIT_LIST_HEAD(&s_team_pk_[l][loop].timer_list);
		}

		for (int l = 0; l < pk_pet_cnt_max; l ++) {
			memset(&s_pet_pk_[l][loop], 0, sizeof(pkwait_info_t));
			INIT_LIST_HEAD(&s_pet_pk_[l][loop].timer_list);
		}
	}

	start_refresh(0, 0);

	init_db_proto_handles();
	
	ok = 0;
fail:
	boot_log(ok, 0, "Online inited");
}

void Online::fini()
{

}

