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
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>
#include "cleveldb.h"
enum{
	g_luckstar_exchange_cnt=1001,
};
struct online_info_t {
	uint16_t	domain_id;
	uint32_t	online_id;
	char		online_ip[16];
	in_port_t	online_port;
	uint32_t	user_num;
	uint32_t	seqno; // depreciated

	fdsession_t*	fdsess;
};

struct user_info_t {
    uint16_t	online;
    uint16_t	states;
	time_t		login_time;
} __attribute__((packed));

struct keepalive_timer_t {
	timer_struct_t*	tmr;
	list_head_t		timer_list;
};

struct timer_head_t {
	list_head_t timer_list;
};

struct pkwait_info_t {
	userid_t		teamid;
	uint8_t			type;
	uint32_t		level;
	uint32_t		count;
	list_head_t		timer_list;
};
struct pkaccept_info_t {
	userid_t		uid;
	battle_id_t		btid;
};

class Online {
public:
	static void init();
	static void fini();
	// For Online Server
	// 新协议
#define PROTO_FUNC_DEF(cmd_name)\
	    static int cmd_name(fdsession_t* fdsess,Cmessage* c_in);
	    #include "./proto/mole2_switch_func_def.h"


	// receive info reported by online server
	static int  report_online_info(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	// receive user online/offline pkg
	static int  report_user_onoff(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	// check the given users' online status
	static int  users_online_status(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	// check the given users' online status
	static int  chat_across_svr(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	// noti across svr
	static int  noti_across_svr(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	
	static int  do_proto_across_svr(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int	onli_sync_beast(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

	// check logined online id of the given user
	static int  chk_user_location(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	// online keepalive package
	static int  online_keepalive(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int	challenge_battle(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	// online apply pk package
	static int onli_apply_pk(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int onli_auto_accept(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int onli_auto_fight(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int onli_cancel_pk_apply(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

	// For Login Server
	static int get_recommeded_svrlist(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int get_recommeded_svrlist_new(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int get_ranged_svrlist(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
		static int get_ranged_svrlist_new(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

	static int syn_info_to_user (svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

	// For Adminer Server
	static int syn_vip_flag(svr_proto_t* pkg, uint32_t bodylen, int fd);
	static int syn_vip_exp(svr_proto_t* pkg, uint32_t bodylen, int fd);
	static int kick_user_offline(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int broadcast_msg(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	static int create_npc(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

	// clear online info on connection closed
	static void clear_online_info(int fd);

	static void clean_user_pkinfo(userid_t uid, uint8_t inbattle, fdsession_t* fdsess);
	static void broadcast_beast_count();
	
	//static inline int send_msg_to_client(fdsession_t* fdsess, uint32_t uid, int cmd, Cmessage *c_in);
	// report current online user number
	//static int report_user_num(void* owner, void* data);
	// close an online server's connection if keepalive package is not received within 70 secs
	static int make_online_dead(void* owner, void* data);
    static int report_usr_num(void* owner, void* data);
	static int get_val_from_db(void* owner, void* data);
	static int start_refresh(void* owner, void* data);
	static int try_match_again(void* owner, void* data);


	static int			 s_beast_cnt_;
private:
	// types
	enum {
		/*! maximum online id */
		online_num_max	= 2048
	};

	enum {
		pklv_phase_max	= 20,
		pklv_per_phase	= 5,
		pk_team_cnt_max	= 5,
		pk_pet_cnt_max	= 3,
	};

	enum {
		apply_fail,
		match_fail,
		match_succ,
	};

	
	typedef std::set<userid_t> pk_set_t;
	typedef std::map<userid_t, pkwait_info_t> pk_start_t;
	typedef std::map<userid_t, pkaccept_info_t> pk_accept_t;

	typedef std::map<int,uint16_t> fd_map_t;
	typedef std::set<userid_t> userid_set_t;
	typedef std::map<userid_t,user_info_t> users_online_t;

	//
	static uint16_t get_user_online_id(userid_t uid){
		users_online_t::iterator iter;
		iter = s_users.find(uid);
		if(iter == s_users.end()) return 0;
		return iter->second.online;
	}
	//
	static void set_user_online_id(userid_t uid, uint16_t online_id) { 
		user_info_t info;
		users_online_t::iterator iter;

		if (online_id) {
			iter = s_users.find(uid);
			if(iter == s_users.end()) {
				info.online = online_id;
				info.states = 1;
				info.login_time = time(NULL);
				s_users[uid] = info;
				if(s_users.size() >= 1000000) {
					iter = s_users.begin();
					while(iter != s_users.end())  {
						if(iter->second.login_time < time(NULL) - 86400) {
							s_users_set[iter->second.online].erase(iter->first);
							s_users.erase(iter++);
						} else {
							iter++;
						}
					}
				}
			} else {
				iter->second.online = online_id;
				iter->second.login_time = time(NULL);
			}
		} else {
			s_users.erase(uid);
		}
	}
	//

	static const online_info_t* get_online_info(uint16_t online_id);
	static pkwait_info_t* get_pkwait_info(uint32_t uid);
	static pkwait_info_t* try_to_match(pkwait_info_t* ppi, uint32_t& match);
	static pkwait_info_t* match_again_single(pkwait_info_t* ppi);
	static pkwait_info_t* match_again_team(pkwait_info_t* ppi);
	static pkwait_info_t* match_again_pet(pkwait_info_t* ppi);
	static int send_pkg_to_online(uint16_t online_id, const void* pkg, int pkglen);
	static int send_msg_to_online(uint32_t onlineid, int cmd, Cmessage *c_in);
	static int send_msg_to_allonline(int cmd, Cmessage *c_in);
	static int send_msg_to_user(fdsession_t* fdsess,uint32_t uid, int cmd, Cmessage *c_in);
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
	static char*         s_statistic_file_; 
	static uint8_t       s_pkg_[pkg_size];
	static pkwait_info_t s_single_pk_[pklv_phase_max];
	static pkwait_info_t s_team_pk_[pk_team_cnt_max][pklv_phase_max];
	static pkwait_info_t s_pet_pk_[pk_pet_cnt_max][pklv_phase_max];
	//static pkwait_info_t s_gvg_pk_[2][pklv_phase_max];
	//static std::map<uint32_t,uint32_t> s_gvg_challengee_;
	static pk_set_t		 s_pkwait_;
	static pk_start_t	 s_start_;
	static pk_accept_t	 s_accept_;

	static int			 s_idc_;

	static users_online_t s_users;
	static userid_set_t	  s_users_set[online_num_max];

	static CLevelDB   lvdb; 
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

inline pkwait_info_t* Online::get_pkwait_info(uint32_t uid)
{
	for (int loop = 0; loop < pklv_phase_max; loop ++) {
		if (s_single_pk_[loop].teamid == uid) {
			return &s_single_pk_[loop];
		}

		for (int l = 0; l < pk_team_cnt_max; l ++) {
			if (s_team_pk_[l][loop].teamid == uid) {
				return &s_team_pk_[l][loop];
			}
		}

		for (int l = 0; l < pk_pet_cnt_max; l ++) {
			if (s_pet_pk_[l][loop].teamid == uid) {
				return &s_pet_pk_[l][loop];
			}
		}
	}
	return NULL;
}

inline int
Online::send_msg_to_user(fdsession_t* fdsess,uint32_t uid, int cmd, Cmessage *c_in)
{   
    char pkg_head[256];
    svr_proto_t* ppr = reinterpret_cast<svr_proto_t*>(pkg_head );
    ppr->id = uid;
	ppr->seq=0;
    ppr->len = sizeof(svr_proto_t);
    ppr->cmd = cmd;
    ppr->ret = 0;
    if (send_msg_to_client(fdsess , pkg_head ,c_in) == -1) {
        ERROR_RETURN(("failed to send msg to user:  cmd=%u", ppr->cmd), -1);
    }
	DEBUG_LOG("send_msg_to_user:cmd=%u",cmd);
    return 0;   
} 
inline int
Online::send_msg_to_online(uint32_t onlineid, int cmd, Cmessage *c_in)
{   
    char pkg_head[256];
    svr_proto_t* ppr = reinterpret_cast<svr_proto_t*>(pkg_head );
    ppr->id = 0;
	ppr->seq=0;
    ppr->len = sizeof(svr_proto_t);
    ppr->cmd = cmd;
    ppr->ret = 0;
	const online_info_t* olinfo = get_online_info(onlineid);
    if (olinfo && send_msg_to_client(olinfo->fdsess , pkg_head ,c_in) == -1) {
        ERROR_RETURN(("failed to send pkg to online: olid=%u cmd=%u",onlineid, ppr->cmd), -1);
    }
	DEBUG_LOG("send_msg_to_online:olid=%u cmd=%u",onlineid,cmd);
    return 0;   
}

inline int
Online::send_msg_to_allonline( int cmd, Cmessage *c_in)
{   
 	for (uint32_t loop = 0; loop < s_max_online_id_; loop ++) {
		if (s_online_info_[loop].fdsess) {
			send_msg_to_online(s_online_info_[loop].online_id,cmd,c_in);
		}
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

