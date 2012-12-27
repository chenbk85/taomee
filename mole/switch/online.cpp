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
#include  <statistic_agent/msglog.h>
}

#include "dbproxy.hpp"
#include "utils.hpp"

#include "online.hpp"

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

uint32_t           Online::s_max_online_id_;
online_info_t      Online::s_online_info_[online_num_max];
keepalive_timer_t  Online::s_keepalive_tmr_[online_num_max];
timer_head_t       Online::s_statistic_tmr_;
Online::fd_map_t   Online::s_fd_map_;
Online::olid_set_t Online::s_nochat_olid_set_;
userid_t     Online::s_max_uid_;
usr_info_t*  Online::s_usr_onoff_;
uint16_t*    Online::s_prev_logined_online_id_;
uint8_t      Online::s_pkg_[pkg_size];
char*        Online::s_statistic_file_ = NULL;
//timer_head_t Online::timer_head;

// chat-forbidden servers
uint16_t   Online::s_nochat_svrs_[] = {
	2, 4, 6, 112, 115, 120, 129, 132, 143, 146, 148, 151, 154,
	171, 187, 193, 225, 230, 248, 249, 256, 258, 272, 278, 295, 296,
	297, 298, 299, 311, 316, 317, 333, 335, 339, 349, 358, 367, 375,
	383, 385, 395, 396, 420, 427, 430, 442, 445, 447, 450, 454, 459,
	462, 469, 470, 483, 490, 498, 522, 524, 534, 535, 542, 543, 566,
	576, 580, 582, 586, 587, 595
};


#define ONE_ONLINE_MAX_NUM   300


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
		DEBUG_LOG("DUPLICATE ONLINE ID\t[id=%u ip=%s port = %d, ip=%s, port=%d]", req->online_id, req->online_ip, req->online_port,  s_online_info_[idx].online_ip,s_online_info_[idx].online_port);
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
    //add new online server info, init the list head 
    INIT_LIST_HEAD(&s_online_info_[idx].online_head);
	s_keepalive_tmr_[idx].tmr = ADD_TIMER_EVENT(&s_keepalive_tmr_[idx], Online::make_online_dead, &s_online_info_[idx], get_now_tv()->tv_sec + 70);

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
	CHECK_VAL(bodylen, 1);

	uint16_t cur_online_id = s_fd_map_[fdsess->fd];
	int idx = cur_online_id - 1;
	CHECK_VAL_GE(idx, 0);

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
                //del this list node
                list_del(&s_usr_onoff_[pkg->id].online_node);

				DEBUG_LOG("KICK PREVIOUS LOGIN OFFLINE\t[uid=%u olid=%d]", pkg->id, online_id);
			}

			++(s_online_info_[idx].user_num);
			set_user_online_id(pkg->id, cur_online_id);
			set_user_prev_online_id(pkg->id, cur_online_id);
		} else { //user offline
			--(s_online_info_[idx].user_num);
			if (get_user_online_id(pkg->id) == cur_online_id) {
				set_user_online_id(pkg->id, 0);
			}
		}
	} else {  //guest user
		if (pkg->body[0]) {
			++(s_online_info_[idx].user_num);
		} else {
			--(s_online_info_[idx].user_num);
		}
	}

	// TODO - to be commented out
	if (((int)(s_online_info_[idx].user_num)) < 0) {
		ERROR_LOG("impossible error! user num=%d", s_online_info_[idx].user_num);
		return -1;
	}

	return 0;
}


int Online::kick_user(uint32_t uid)
{
	uint32_t buff[1024];
	uint16_t online_id = get_user_online_id(uid);
	if (online_id != 0) {
		// notify the other online server to kick the user offline
		int idx = sizeof(svr_proto_t);
		pack_h(buff, uid, idx);
		pack_h(buff, 0x00100001, idx);
		init_proto_head(buff, uid, idx, 0, proto_kick_usr_offline, 0);
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, buff, idx);
	}
	return 0;
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
	CHECK_VAL_GE(bodylen, 4);// body = senderid(4 bytes) + package(>0 bytes) 

	userid_t* p_uid     = reinterpret_cast<userid_t*>(pkg->body);
	userid_t  sender_id = *p_uid; // sender id, the first 4 bytes
	uint16_t  online_id = get_user_online_id(pkg->id);
	if (online_id) {
		// user's online, send msg to the corresponding online server
		*p_uid = pkg->id;  // The first 4 bytes set to receiver id
		init_proto_head(pkg, sender_id, pkg->len, 0, pkg->cmd, 0); //initialize header
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
	} else {
		// user's offline, send offline msg to db proxy
		*p_uid = bodylen;
		send_request_to_db(pkg->id, dbcmd_offline_msg, pkg->body, bodylen);
	}

	return 0;
}

int Online::user_attr_upd_noti(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct attr_upd_noti_t {
		userid_t sender_id;
		userid_t recver_id;
		uint32_t type;
	} __attribute__((packed));

	CHECK_VAL(bodylen, sizeof(attr_upd_noti_t));

	attr_upd_noti_t* noti = reinterpret_cast<attr_upd_noti_t*>(pkg->body);
	uint16_t online_id = get_user_online_id(noti->recver_id);

	if (online_id) { //receiver online
		int idx = sizeof(svr_proto_t);
		pack_h(s_pkg_, noti->recver_id, idx);
		pack_h(s_pkg_, noti->type, idx);
		init_proto_head(s_pkg_, pkg->id, idx, 0, pkg->cmd, 0);

		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, s_pkg_, idx);
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

//	DEBUG_LOG("KEEP ALIVE\t[fd=%d id=%u ip=%s port=%d]",
//				fdsess->fd, s_online_info_[idx].online_id,
//				s_online_info_[idx].online_ip, s_online_info_[idx].online_port);
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

		s_keepalive_tmr_[idx].tmr = 0;
		REMOVE_TIMERS(&(s_keepalive_tmr_[idx]));

        usr_info_t* p;
        //set all usr at this online server offline
        list_for_each_entry (p, &s_online_info_[idx].online_head, online_node) {
            p->online_id = 0;
        }

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

#define is_busy_online(V) (V.user_num >= ONE_ONLINE_MAX_NUM)

//----------------------------------------------------------
// For Login Server
//
// get recommended server list
int Online::get_recommeded_svrlist(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	DEBUG_LOG("get recommeded");
	errno = 0;
	CHECK_VAL_GE(pkg->len, sizeof(svr_proto_t) + 7); // DomainID(2)+OM_Server(1)+FriendCnt(4)
	kick_user(pkg->id);
	uint16_t* dx_or_wt = reinterpret_cast<uint16_t*>(pkg->body);
	friend_list_t* friendlist = reinterpret_cast<friend_list_t*>(pkg->body + 3);// FriendCnt + Friend List
	CHECK_VAL_GE(pkg->len, sizeof(svr_proto_t) + 7 + friendlist->cnt * sizeof(userid_t));

	/*=============================================
	  * return 11 server at most:
	  *   1. One previous logined server;
	  *   2. Servers in which the user's friends are located;
	  *   3. Hot servers;
	  *   4. Two chat-forbidden servers.
	  *=============================================*/

	// previous logined online server id
	uint32_t prev_online_id = get_user_prev_online_id(pkg->id);

	// get online server IDs in which the user's friend are located
	olid_set_t olid_frd_set; // online server id set in which the user's friends are located
	for (uint32_t i = 0; (i != friendlist->cnt) && (olid_frd_set.size() != 8); ++i) {
		uint16_t online_id = get_user_online_id(friendlist->uid[i]);
		if (online_id && (online_id != prev_online_id)
				&& (s_online_info_[online_id - 1].user_num < ONE_ONLINE_MAX_NUM)
				&& (s_online_info_[online_id - 1].domain_id == *dx_or_wt)
				&& (s_nochat_olid_set_.count(online_id) == 0)) {
			olid_frd_set.insert(online_id);
		}
	}

	// get hot servers
	uint32_t remain_cnt = 8 - olid_frd_set.size();
	olid_set_t olid_remain_set; // remaining online server id set
	for (uint32_t i = 0; (olid_remain_set.size() != remain_cnt) && (i != s_max_online_id_); ++i) {
		if ((s_online_info_[i].online_id != prev_online_id)
				&& (s_online_info_[i].user_num >= 300)
				&& (s_online_info_[i].user_num < 350)
				&& (s_online_info_[i].domain_id == *dx_or_wt)
				&& !s_nochat_olid_set_.count(s_online_info_[i].online_id)
				&& !olid_frd_set.count(s_online_info_[i].online_id)) {
			olid_remain_set.insert(s_online_info_[i].online_id);
		}
	}

	// get remaining online server IDs to form the server list package
	for (uint32_t i = 0; (olid_remain_set.size() != remain_cnt) && (i != s_max_online_id_); ++i) {
		if (s_online_info_[i].online_id
				&& (s_online_info_[i].online_id != prev_online_id)
				&& (s_online_info_[i].user_num < 300)
				&& (s_online_info_[i].domain_id == *dx_or_wt)
				&& !s_nochat_olid_set_.count(s_online_info_[i].online_id)
				&& !olid_frd_set.count(s_online_info_[i].online_id)) {
			olid_remain_set.insert(s_online_info_[i].online_id);
		}
	}
	
	// get remaining online server IDs to form the server list package
		for (uint32_t i = 0; (olid_remain_set.size() != remain_cnt) && (i != s_max_online_id_); ++i) {
			if (s_online_info_[i].online_id
					&& (s_online_info_[i].online_id != prev_online_id)
					&& (s_online_info_[i].user_num < 300)
					&& !s_nochat_olid_set_.count(s_online_info_[i].online_id)
					&& !olid_frd_set.count(s_online_info_[i].online_id)) {
				olid_remain_set.insert(s_online_info_[i].online_id);
			}
		}

	// pack server list and send it to login server
	int idx = sizeof(svr_proto_t), cnt_idx;
	pack_h(s_pkg_, static_cast<uint16_t>(0), idx); // domain id
	cnt_idx = idx; // remember the index to pack online_cnt
	idx += 4; // reserve space for online_cnt

	uint32_t online_cnt      = 1; // at least the previous logined online server id is returned
	// pack previous logined online server id
	uint32_t prev_online_idx = prev_online_id - 1;
	if (prev_online_id && (s_online_info_[prev_online_idx].online_id == prev_online_id)) {
		pack_h(s_pkg_, prev_online_id, idx);
		if(is_busy_online(s_online_info_[prev_online_idx])) {
			pack_h(s_pkg_, 5, idx);
		} else {
			pack_h(s_pkg_, (s_online_info_[prev_online_idx].user_num * 5)/ONE_ONLINE_MAX_NUM, idx);
		}
		pack(s_pkg_, s_online_info_[prev_online_idx].online_ip, sizeof(s_online_info_[0].online_ip), idx);
		pack_h(s_pkg_, s_online_info_[prev_online_idx].online_port, idx);
		pack_h(s_pkg_, 0, idx); // no friends
	} else { // previous logined online server not exist
		pack_h(s_pkg_, 0, idx);
		idx += 26;
	}

	// pack online servers in which the user's friends are located
	for (olid_set_t::iterator it = olid_frd_set.begin(); it != olid_frd_set.end(); ++it) {
		int index = *it - 1;
		pack_h(s_pkg_, s_online_info_[index].online_id, idx);
		if(is_busy_online(s_online_info_[index])) {
			pack_h(s_pkg_, 5, idx);
		} else {
			pack_h(s_pkg_, (s_online_info_[index].user_num * 5)/ONE_ONLINE_MAX_NUM, idx);
		}
		pack(s_pkg_, s_online_info_[index].online_ip, sizeof(s_online_info_[0].online_ip), idx);
		pack_h(s_pkg_, s_online_info_[index].online_port, idx);
		pack_h(s_pkg_, 1, idx); // has friends
		++online_cnt;
	}

	// pack the remaining online servers
	for (olid_set_t::iterator it = olid_remain_set.begin(); it != olid_remain_set.end(); ++it) {
		int index = *it - 1;
		pack_h(s_pkg_, s_online_info_[index].online_id, idx);
		if(is_busy_online(s_online_info_[index])) {
			pack_h(s_pkg_, 5, idx);
		} else {
			pack_h(s_pkg_, (s_online_info_[index].user_num * 5)/ONE_ONLINE_MAX_NUM, idx);
		}
		pack(s_pkg_, s_online_info_[index].online_ip, sizeof(s_online_info_[0].online_ip), idx);
		pack_h(s_pkg_, s_online_info_[index].online_port, idx);
		pack_h(s_pkg_, 0, idx); // no friends
		++online_cnt;
	}

	// pack two chat-forbidden online servers
	uint16_t saved_olid = 0; // first chat-forbidden online server id
	for (int i = 0, cnt = 0; (cnt != 2) && (i != 10); ++i) {
		int olid_idx = s_nochat_svrs_[rand() % array_elem_num(s_nochat_svrs_)] - 1;
		if ((s_online_info_[olid_idx].online_id != 0)
				&& (s_online_info_[olid_idx].online_id != prev_online_id)
				&& (s_online_info_[olid_idx].online_id != saved_olid)
				&& (s_online_info_[olid_idx].user_num < ONE_ONLINE_MAX_NUM)) {
			pack_h(s_pkg_, s_online_info_[olid_idx].online_id, idx);
			if(is_busy_online(s_online_info_[olid_idx])) {
				pack_h(s_pkg_, 5, idx);
			} else {
				pack_h(s_pkg_, (s_online_info_[olid_idx].user_num * 5)/ONE_ONLINE_MAX_NUM, idx);
			}
			pack(s_pkg_, s_online_info_[olid_idx].online_ip, sizeof(s_online_info_[0].online_ip), idx);
			pack_h(s_pkg_, s_online_info_[olid_idx].online_port, idx);
			pack_h(s_pkg_, 0, idx); // no friends
			++online_cnt;
			++cnt;
			saved_olid = s_online_info_[olid_idx].online_id;
		}
	}

	pack_h(s_pkg_, online_cnt, cnt_idx);
	pack_h(s_pkg_, s_max_online_id_, idx);
	init_proto_head(s_pkg_, pkg->id, idx, pkg->seq, pkg->cmd, 0);
	DEBUG_LOG("pkg to client %u %u %u, %u", pkg->id, pkg->cmd, fdsess->fd, idx);
	int ret = send_pkg_to_client(fdsess, s_pkg_, idx);
	DEBUG_LOG("ret %u %s", ret, strerror(errno));
	return ret;
}

int Online::get_ranged_svrlist(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct svrlist_req_t {
		uint16_t domain_id;
		uint32_t start_id;   // start online id
		uint32_t end_id;     // end online id
		uint32_t friend_cnt;
		userid_t friend_id[];
	} __attribute__((packed));
	

	CHECK_VAL_GE(bodylen, sizeof(svrlist_req_t));
	kick_user(pkg->id);
	svrlist_req_t* svrlist = reinterpret_cast<svrlist_req_t*>(pkg->body);

	CHECK_VAL(bodylen, sizeof(svrlist_req_t) + svrlist->friend_cnt * sizeof(userid_t));
	
	//kick_user_offline(pkg, );
	if ((svrlist->start_id == 0) || (svrlist->end_id > online_num_max)
			|| (svrlist->start_id > svrlist->end_id) || ((svrlist->end_id - svrlist->start_id) > 200)) {
		ERROR_LOG("invalid range: start=%u end=%u", svrlist->start_id, svrlist->end_id);
		return -1;
	}

	// pack server list
	int idx = sizeof(svr_proto_t), cnt_idx;
	pack_h(s_pkg_, static_cast<uint16_t>(0), idx);	
	cnt_idx = idx; // remember the index to pack online_cnt
	idx += 4; // reserve space for online_cnt
	uint32_t online_cnt = 0;	
	for (uint32_t i = svrlist->start_id - 1; i != svrlist->end_id; ++i) {
		if (s_online_info_[i].online_id) {
			pack_h(s_pkg_, s_online_info_[i].online_id, idx);
			if(is_busy_online(s_online_info_[i])) {
				pack_h(s_pkg_, 5, idx);
			} else {
				pack_h(s_pkg_, (s_online_info_[i].user_num * 5)/ONE_ONLINE_MAX_NUM, idx);
			}
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
int Online::syn_vip_flag(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
    CHECK_VAL(bodylen, 4);
	uint16_t online_id = get_user_online_id(pkg->id);
    DEBUG_LOG("SYN VIP OP [%d %d %x]", pkg->id, online_id, *(uint32_t*)pkg->body);
	if (online_id) {
		init_proto_head(pkg, pkg->id, pkg->len, 0, proto_syn_vip_flag, 0);
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
	}

    return 0;
}

int Online::syn_to_user_cmd(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
 	uint16_t online_id = get_user_online_id(pkg->id);
	DEBUG_LOG("SYN USER CMD [%d %d %d]", pkg->id, online_id, bodylen);
	if (online_id) {
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
		DEBUG_LOG("SYN USER [%d %d]%d", pkg->id, online_id, *(uint32_t*)pkg->body);
	}

	return 0;
}


/* @brief 如果一个用户把某个用户加入黑名单，则通知这个用户从好友列表中删除这个用户
 */
int Online::del_from_friend_list(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen, 8);

	userid_t uid;
	int idx = 0;
	unpack_h(pkg->body, uid, idx); // unpack user id
	uint16_t online_id = get_user_online_id(uid);
	if (online_id) {
		init_proto_head(pkg, 0, pkg->len, 0, proto_del_uid_friend_list, 0);
		// No response package to adminer server, so ignore "fdsess"(adminer server information),
		//+ just send package to destination online server
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
	DEBUG_LOG("receiver %u", msg->uid);
	CHECK_VAL_GE(bodylen, sizeof(msg_t) + msg->msglen + 4);

	uint32_t dbmsglen; // size of "dbmsglen" itself(4 bytes) included
	int idx = sizeof(msg_t) + msg->msglen;
	unpack_h(pkg->body, dbmsglen, idx);

	CHECK_VAL(bodylen, sizeof(msg_t) + msg->msglen + dbmsglen);

	DEBUG_LOG("receiver0 %u", msg->uid);
	// pack msg to send to the user(s)
	idx = sizeof(svr_proto_t);
	// online_id is not necessary to be included in the dispathed packages,
	//+ here "pkg->body+4" means discarding online_id
	pack(s_pkg_, pkg->body + 4, bodylen - 4 - dbmsglen, idx);
	init_proto_head(s_pkg_, 0, idx, 0, proto_broadcast_msg, 0);

	if (msg->uid) { // send msg to a given user
		uint16_t online_id = get_user_online_id(msg->uid);
		if (online_id) {
			send_pkg_to_client(s_online_info_[online_id - 1].fdsess, s_pkg_, idx);
			DEBUG_LOG("receiver1 %u", msg->uid);
		} else {
			idx = 0; //no header
			pack(s_pkg_, pkg->body + sizeof(msg_t) + msg->msglen, dbmsglen, idx);
			DEBUG_LOG("receiver2 %u", msg->uid);
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

int Online::broadcast_bubble_msg(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	typedef struct {
		uint32_t online_id; // 0 for all online servers
		uint32_t map_id;
		uint32_t pic_id;
		uint32_t jump_map_id;
		uint32_t uid_count;
		uint32_t task_id;
		uint32_t task_state;
		uint32_t msg_type;
		uint32_t msg_len;
		uint8_t  data[];
	} __attribute__((packed)) msg_bubble_t;

	CHECK_VAL_GE(bodylen, sizeof(msg_bubble_t));

	msg_bubble_t* msg = reinterpret_cast<msg_bubble_t*>(pkg->body);

	CHECK_VAL(bodylen, sizeof(msg_bubble_t) + sizeof(uint32_t) * msg->uid_count + msg->msg_len);

	int idx = sizeof(svr_proto_t);
	/*填充UID,如果需要针对UID发，后面会重新填充这个字段*/
	pack_h(s_pkg_, 0, idx);
	pack_h(s_pkg_, msg->map_id, idx);
	pack_h(s_pkg_, msg->pic_id, idx);
	pack_h(s_pkg_, msg->task_id, idx);
	pack_h(s_pkg_, msg->task_state, idx);
	pack_h(s_pkg_, msg->jump_map_id, idx);
	pack_h(s_pkg_, msg->msg_type, idx);
	pack_h(s_pkg_, msg->msg_len,idx);
	pack(s_pkg_, (pkg->body + sizeof(msg_bubble_t) + msg->uid_count * sizeof(uint32_t)), msg->msg_len, idx);
	init_proto_head(s_pkg_, 0, idx, 0, proto_broadcast_bubble_msg, 0);

	DEBUG_LOG("bubble %u %u %u", msg->online_id, msg->map_id, msg->pic_id);
	if (msg->uid_count) { // send msg to a given user
		uint32_t *p_uid = (uint32_t *)(pkg->body + sizeof(msg_bubble_t));
		for (uint32_t i = 0; i < msg->uid_count; i++) {
			uint32_t uid = *(p_uid + i);
			int index = sizeof(svr_proto_t);
			pack_h(s_pkg_, uid, index);
			uint16_t online_id = get_user_online_id(uid);
			if (online_id) {
				send_pkg_to_client(s_online_info_[online_id - 1].fdsess, s_pkg_, idx);
			} 	
		}
	DEBUG_LOG("bubble1 %u %u %u", msg->online_id, msg->map_id, msg->pic_id);
	} else if (msg->online_id) { // send msg to all users of a given online server
		int ret = send_pkg_to_online(msg->online_id, s_pkg_, idx);
		DEBUG_LOG("bubble2 %u %u %u ret=%d", msg->online_id, msg->map_id, msg->pic_id,ret);
	} else if (msg->online_id) { // send msg to all users of a given online server
	} else { // send msg to all users of all the online servers
		for (uint32_t i = 0; i != s_max_online_id_; ++i) {
			if (s_online_info_[i].online_id) {
				send_pkg_to_client(s_online_info_[i].fdsess, s_pkg_, idx);
			}
		}
	}

	DEBUG_LOG("bubble3 %u %u %u", msg->online_id, msg->map_id, msg->pic_id);
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

int Online::broadcast_guess_pkt(svr_proto_t* pkg, uint32_t bodylen, int fd)
{
	if(pkg->cmd == dbcmd_guess_start)
		pkg->cmd = proto_guess_start;
	else
		pkg->cmd = proto_guess_end;

	for (uint32_t i = 0; i != s_max_online_id_; ++i) {
		if (s_online_info_[i].online_id) {
			send_pkg_to_client(s_online_info_[i].fdsess, pkg, pkg->len);
		}
	}

	return 0;
}

int Online::broadcast_use_skill_prize_pkt(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{	
	DEBUG_LOG("broadcast_use_skill_prize_pkt ");
	for (uint32_t i = 0; i != s_max_online_id_; ++i) {
		if (s_online_info_[i].online_id) {
			DEBUG_LOG("broadcast_use_skill_prize_pkt online_id %d", s_online_info_[i].online_id);
			init_proto_head(pkg, 0, pkg->len, 0, proto_notify_use_skill_prize, 0);
			send_pkg_to_client(s_online_info_[i].fdsess, pkg, pkg->len);
		}
	}

	return 0;
}

int Online::broadcast_get_cp_beauty_prize(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	DEBUG_LOG("broadcast_get_cp_beauty_prize");
	for (uint32_t i = 0; i != s_max_online_id_; ++i) {
		if (s_online_info_[i].online_id) {
			//DEBUG_LOG("broadcast_get_cp_beauty_prize online_id %d", s_online_info_[i].online_id);
			init_proto_head(pkg, 0, pkg->len, 0, proto_tell_cp_beauty_prize, 0);
			send_pkg_to_client(s_online_info_[i].fdsess, pkg, pkg->len);
		}
	}

	return 0;
}

int Online::broadcast_tell_flash_some_msg(svr_proto_t* pkg, uint32_t body_len, fdsession_t* fdsess)
{
	DEBUG_LOG("tell flash some msg");
	uint32_t msg_type = *(uint32_t*)pkg->body;
	uint32_t msg_len = *(uint32_t*)(pkg->body + 4);	
	CHECK_VAL(body_len, msg_len + 8);
	switch( msg_type )
	{
	case CBMT_UPDATE_MONEY:
		{
			uint32_t user_id = *(uint32_t*)(pkg->body + 8);
			uint16_t online_id = get_user_online_id(user_id);
			if (online_id) {
				init_proto_head(pkg, 0, pkg->len, 0, proto_tell_flash_some_msg, 0);
				send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
			}		
		}
		break;
	default:
		for (uint32_t i = 0; i != s_max_online_id_; ++i) {
			if (s_online_info_[i].online_id) {
				init_proto_head(pkg, 0, pkg->len, 0, proto_tell_flash_some_msg, 0);
				send_pkg_to_client(s_online_info_[i].fdsess, pkg, pkg->len);
			}

		}
		break;
	}

	return 0;
}

int Online::user_add_item_pkt(svr_proto_t* pkg, uint32_t bodylen, int fd)
{
	return 0;
}

//-------------------------------------------------------
//  utils
//
/*int Online::report_user_num(void* owner, void* data)
{
	TONLINE_USERCOUNT usernum;

	usernum.onlinecount = 0;
	for (uint32_t i = 0; i != s_max_online_id_; ++i) {
		if (s_online_info_[i].user_num > 0) {
			usernum.onlinecount += s_online_info_[i].user_num;
		}
	}
	usernum.logtime  = get_now_tv()->tv_sec;
	usernum.serverid = 1;
	SAVE_PROTOLOG_ONLINE_USERCOUNT(usernum);

	ADD_TIMER_EVENT(&timer_head, report_user_num, 0, get_now_tv()->tv_sec + 60);

	return 0;
}*/

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

    msglog(s_statistic_file_, 0x020A0000, get_now_tv()->tv_sec, &cnt, 4);
	msglog(s_statistic_file_, 0x020A0001, get_now_tv()->tv_sec, &dx_cnt, 4);
	msglog(s_statistic_file_, 0x020A0002, get_now_tv()->tv_sec, &wt_cnt, 4);
    ADD_TIMER_EVENT(&s_statistic_tmr_, Online::report_usr_num, NULL, get_now_tv()->tv_sec + 60);

	return 0;
}

static timer_head_t  s_guess_tmr_;
static uint32_t 	 s_guess_state_ = 0; // guess state

int Online::report_guess_state(void* owner, void*data)
{
	int32_t seconds = get_now_tv()->tv_sec ;
	seconds = seconds - ((seconds + 3600 * 8) % (3600 * 24)) + 3600 * 20;//20:00 pm of today
	if(get_now_tv()->tv_sec >= seconds) {
		if(s_guess_state_) {
			send_request_to_db(0, dbcmd_guess_end, NULL, 0);
		}
		s_guess_state_ = 0;
		seconds = seconds + 3600 * 22;// tomorrow 18:00 pm
		ADD_TIMER_EVENT(&s_guess_tmr_, Online::report_guess_state, NULL, seconds);
	} else {
		s_guess_state_ = !s_guess_state_;
		if(s_guess_state_) {
			seconds = get_now_tv()->tv_sec + 120;//120 seconds
			ADD_TIMER_EVENT(&s_guess_tmr_, Online::report_guess_state, NULL, seconds);
			send_request_to_db(0, dbcmd_guess_start, NULL, 0);
		} else {
			seconds = get_now_tv()->tv_sec + 2;//wait 2 seconds for next round
			ADD_TIMER_EVENT(&s_guess_tmr_, Online::report_guess_state, NULL, seconds);
			send_request_to_db(0, dbcmd_guess_end, NULL, 0);
		}
	}

	DEBUG_LOG("guess state %d",s_guess_state_);
	return 0;
}

int Online::xhx_user_set_value(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen, 4);
	userid_t uid = pkg->id;
	CHECK_USER_ID(uid);

	uint32_t type;
	int idx = 0;
	unpack_h(pkg->body, type, idx);

	DEBUG_LOG("get msg from xhx online: uid[%u] type[%u]", uid, type);
	uint32_t attireid = 0;
	uint32_t attire_cnt = 1;
	uint32_t attire_type = 99;
	switch (type) {
	case 1:
		attireid = 1351126;
		break;
	case 2:
		attireid = 1351127;
		break;
	case 3:
		attireid = 1351128;
		break;
	case 4:
		attireid = 1351129;
		break;
	default:
		return 0;
	}
	if (attireid != 0) {
		uint32_t db_buf[]={0, 1, 0, 0, attire_type, attireid, attire_cnt, 99999};
		send_request_to_db(uid, 0x114D, db_buf, sizeof(db_buf));
	}

	uint32_t buff[1024] = {0};
	uint16_t online_id = get_user_online_id(uid);
	if (online_id) {
		DEBUG_LOG("send msg to online: uid[%u] type[%u] onlineid[%u]", uid, type, online_id);
		idx = sizeof(svr_proto_t);
		pack_h(buff, type, idx);
		init_proto_head(buff, uid, idx, 0, proto_xhx_user_set_value, 0);
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, buff, idx);
	}
	return 0;
}

int Online::chat_monitor_warning_user(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen, 4);

	userid_t uid = pkg->id;
	uint32_t type;
	int idx = 0;
	unpack_h(pkg->body, type, idx);

	DEBUG_LOG("get msg from chat monitor online: uid[%u] type[%u]", uid, type);
	uint32_t buff[1024] = {0};
	uint16_t online_id = get_user_online_id(uid);
	if (online_id) {
		idx = sizeof(svr_proto_t);
		DEBUG_LOG("get chat monitor to online: uid[%u] type[%u]", uid, type);
		pack_h(buff, type, idx);
		init_proto_head(buff, uid, idx, 0, proto_chat_monitor_warning_user, 0);
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, buff, idx);
	}
	return 0;
}


void Online::init()
{
	int ok = -1, fd;
	off_t offset;

	srand(time(0));
//	uint32_t seconds = get_now_tv()->tv_sec;

//	INIT_LIST_HEAD(&timer_head.timer_list);
//	ADD_TIMER_EVENT(&timer_head, report_user_num, 0, get_now_tv()->tv_sec + 60);

	s_max_uid_ = config_get_intval("max_uid", 0);

	s_statistic_file_ = config_get_strval("statistic_file");

	// init memory to hold the current logined online id of each user
	s_usr_onoff_ = reinterpret_cast<usr_info_t*>(calloc(s_max_uid_, sizeof(*s_usr_onoff_)));
	if ((s_max_uid_ <= 0) || (s_usr_onoff_ == 0)) {
		ERROR_LOG("failed to allocate %d * %lu bytes of space", s_max_uid_, sizeof(*s_usr_onoff_));
		goto fail;
	}

	// init memory to hold the previous logined online id of each user
	fd = open(config_get_strval("online_id_file"),
				O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd == -1) {
		ERROR_LOG("failed to open file=%s err=%d %s",
					config_get_strval("online_id_file"), errno, strerror(errno));
		goto fail;
	}
	
	// resize the file if necessary (in case that the file does not 
	//+ exist, which leads to zero byte memory mapping)
	offset = lseek(fd, s_max_uid_ * sizeof(*s_prev_logined_online_id_), SEEK_SET);
	if ((offset == -1) || (write(fd, "", 1) == -1)) {
		ERROR_LOG("failed to resize file=%s err=%d %s",
					config_get_strval("online_id_file"), errno, strerror(errno));
		goto fail;
	}
	// mmap the file to memory
	s_prev_logined_online_id_= reinterpret_cast<uint16_t*>(mmap(0, s_max_uid_ * sizeof(*s_prev_logined_online_id_), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0));
	if (s_prev_logined_online_id_ == MAP_FAILED) {
		ERROR_LOG("failed to mmap file=%s err=%d %s",
					config_get_strval("online_id_file"), errno, strerror(errno));
		goto fail;
	}
	close(fd);

	for (size_t i = 0; i != array_elem_num(s_nochat_svrs_); ++i) {
		s_nochat_olid_set_.insert(s_nochat_svrs_[i]);
	}

    for (int i = 0; i != online_num_max; ++i) {
        INIT_LIST_HEAD(&s_online_info_[i].online_head);
    }
	
    if(s_statistic_file_) {
        INIT_LIST_HEAD(&s_statistic_tmr_.timer_list);
        ADD_TIMER_EVENT(&s_statistic_tmr_, Online::report_usr_num, NULL, get_now_tv()->tv_sec + 60);
    }
/*
	INIT_LIST_HEAD(&s_guess_tmr_.timer_list);
	seconds = seconds - ((seconds + 3600 * 8) % (3600 * 24));//00:00 pm of today
	
	DEBUG_LOG("now %u, seconds=%u",get_now_tv()->tv_sec,seconds);
	if(get_now_tv()->tv_sec >= seconds + 3600 * 18) {// > 18:00 pm of today
		if(get_now_tv()->tv_sec < seconds + 20 * 3600) { // < 20:00 of today
			seconds = get_now_tv()->tv_sec + 2;
		} else {
			seconds = seconds + (3600 * (24 + 18));// tomorrow 18:00 pm
		}
	} else {
		seconds = seconds + 3600 * 18;// 18:00 pm of today
	}

	DEBUG_LOG("now %u, timer=%u",get_now_tv()->tv_sec,seconds);
	ADD_TIMER_EVENT(&s_guess_tmr_, Online::report_guess_state, NULL, seconds);
*/
	ok = 0;
fail:
	boot_log(ok, 0, "Online inited");
}

void Online::fini()
{
	free(s_usr_onoff_);
	munmap(s_prev_logined_online_id_, s_max_uid_ * sizeof(*s_prev_logined_online_id_));
}

