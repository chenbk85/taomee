#ifndef SWITCH_ONLINE_HPP_
#define SWITCH_ONLINE_HPP_

#include <map>
#include <set>

extern "C" {
#include <arpa/inet.h>

#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
}

#include "proto.hpp"

struct online_info_t {
	uint16_t	domain_id;
	uint32_t	online_id;
	char		online_ip[16];
	in_port_t	online_port;
	uint32_t	user_num;
	uint32_t	seqno; // depreciated

	fdsession_t*	fdsess;
	list_head_t online_head; //list start point for every online server
};

struct usr_info_t {
    uint16_t online_id;
    list_head_t online_node; //user's list node
} __attribute__((packed));

struct keepalive_timer_t {
	timer_struct_t*	tmr;
	list_head_t		timer_list;
};

struct timer_head_t {
	list_head_t timer_list;
};

enum CentralBroadcastMsgType
{
	CBMT_TAOTAOLE_DRAW			= 1,
	CBMT_LONGZU_EXPLORE			= 2,
	CBMT_CHP_DONATE_ITEM		= 3,
	CBMT_UPDATE_MONEY			= 4,
};

class Online {
public:
	static void init();
	static void fini();
	// For Online Server
	// receive info reported by online server
	static int  report_online_info(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	// receive user online/offline pkg
	static int  report_user_onoff(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	// check the given users' online status
	static int  users_online_status(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	// check the given users' online status
	static int  chat_across_svr(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	// send user attribute update notification
	static int  user_attr_upd_noti(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	// check logined online id of the given user
	static int  chk_user_location(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	// online keepalive package
	static int  online_keepalive(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

	static int del_from_friend_list(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	// For Login Server
	static int get_recommeded_svrlist(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int get_ranged_svrlist(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

	// For Adminer Server
	static int syn_vip_flag(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int kick_user_offline(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int broadcast_msg(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int create_npc(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

	static int broadcast_bubble_msg(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	// clear online info on connection closed
	static void clear_online_info(int fd);
	static int kick_user(uint32_t uid);

	static int broadcast_guess_pkt(svr_proto_t* pkg, uint32_t bodylen, int fd);
	static int broadcast_use_skill_prize_pkt(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

	static int syn_to_user_cmd(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int xhx_user_set_value(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int chat_monitor_warning_user(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int user_add_item_pkt(svr_proto_t* pkg, uint32_t bodylen, int fd);
	static int broadcast_get_cp_beauty_prize(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int broadcast_tell_flash_some_msg(svr_proto_t* pkg, uint32_t body_len, fdsession_t* fdsess);
private:
	// types
	enum {
		/*! maximum online id */
		online_num_max	= 2048
	};
	typedef std::map<int, uint16_t> fd_map_t; // map<fd, online_id>
	typedef std::set<uint16_t> olid_set_t;    // online id set type

	//
	static uint16_t get_user_online_id(userid_t uid)
		{ return (uid < s_max_uid_) ? s_usr_onoff_[uid].online_id : 0; }
	//
	static void set_user_online_id(userid_t uid, uint16_t online_id)
		{ 
            s_usr_onoff_[uid].online_id =  online_id; 
            if (online_id) {
                //set user online, add this node 
                list_add_tail(&s_usr_onoff_[uid].online_node, &s_online_info_[online_id - 1].online_head);
            } else {
                //set user offline, del this node 
                list_del(&s_usr_onoff_[uid].online_node);
            }
        }
	//
	static uint16_t get_user_prev_online_id(userid_t uid)
		{ return ((uid < s_max_uid_) ? s_prev_logined_online_id_[uid] : 0); }
	//
	static void set_user_prev_online_id(userid_t uid, uint16_t online_id)
		{ s_prev_logined_online_id_[uid] = online_id; }
	//
	static const online_info_t* get_online_info(uint16_t online_id);
	static int send_pkg_to_online(uint16_t online_id, const void* pkg, int pkglen);
	// report current online user number
	//static int report_user_num(void* owner, void* data);
	// close an online server's connection if keepalive package is not received within 70 secs
	static int make_online_dead(void* owner, void* data);
    static int report_usr_num(void* owner, void*data);
    static int report_guess_state(void* owner, void*data);
	// private static methods
	static bool is_guest(userid_t uid)
		{ return uid > 2000000000; }
	static bool is_valid_uid(userid_t uid)
		{ return uid < s_max_uid_; }

	// private static fields
	static online_info_t		s_online_info_[online_num_max];
	static keepalive_timer_t	s_keepalive_tmr_[online_num_max];
	static timer_head_t  s_statistic_tmr_;
	
	static uint32_t      s_max_online_id_; // max online id
	static fd_map_t      s_fd_map_;
	static userid_t      s_max_uid_;
	static usr_info_t*     s_usr_onoff_;     // hold current logined online id of each user
	static uint16_t*     s_prev_logined_online_id_;  // hold previous logined online id of each user
	static char*         s_statistic_file_; 
	static olid_set_t    s_nochat_olid_set_;  // chat-forbidden online id set
	static uint16_t      s_nochat_svrs_[];    // chat-forbidden online ids
	static uint8_t       s_pkg_[pkg_size];
//	static timer_head_t  timer_head;
};

inline const online_info_t*
Online::get_online_info(uint16_t online_id)
{
	int idx = online_id - 1;
	if ((online_id > 0) && (online_id <= online_num_max)
			&& s_online_info_[idx].online_id == online_id) {
		return &(s_online_info_[idx]);
	}
	return 0;
}

inline int
Online::send_pkg_to_online(uint16_t online_id, const void* pkg, int pkglen)
{
	const online_info_t* olinfo = get_online_info(online_id);
	if (olinfo) {
		return send_pkg_to_client(olinfo->fdsess, s_pkg_, pkglen);// with details in net_if.h
	} else {
		ERROR_LOG("Online Svr Info Not Found: olid=%d", online_id);
		return -1;
	}
}

#endif // SWITCH_ONLINE_HPP_

