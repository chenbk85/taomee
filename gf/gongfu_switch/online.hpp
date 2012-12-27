// =====================================================================================
//
//       Filename:  online.hpp
// 
//    	 Description: specify the communication between online and switch 
// 
//       Version:  1.0
//       Created:  03/11/2009 08:35:48 PM CST
//       Compiler:  g++
// 
//       Company:  TAOMEE
// 
// =====================================================================================


#ifndef SWITCH_ONLINE_HPP_
#define SWITCH_ONLINE_HPP_

#include <map>
#include <set>
#include <vector>
#include <fstream>

using namespace std;

extern "C" 
{
#include <arpa/inet.h>
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
}

#include "switch.hpp"

enum {
	sys_mail_template	= 10007,
	npc_shawn_id		= 7,
	nick_size			= 16,
};

typedef std::vector<uint32_t> oluser_vec_t;

#pragma pack(1)
/** 
  * @brief online server related information 
  */
struct online_info_t 
{
	/* telecom, netcom....*/
	uint16_t	domain;
	uint32_t	online_id;
	char		online_ip[16];
	in_port_t	online_port;

	uint32_t	user_num;
	/* list to link all the players within one online together  */
	list_head_t player_list;

	/* fd related information of online */
	fdsession_t*	fdsess;
};

struct home_online_info_t 
{
	uint32_t	online_id;

	/* fd related information of home online */
	fdsession_t*	fdsess;
};

struct player_ol_t {
    userid_t    usrid;
    uint8_t     lv;
    uint8_t     vip_lv;
    uint32_t    index[4];
};

/** 
  * @brief player type in online server 
  */
struct player_info_t 
{
	userid_t	usrid;
	uint32_t	role_tm;
	uint32_t	role_type;
	uint8_t		vip_lv;
	uint8_t		lv;
	uint32_t	online_id;
	uint8_t     home_lv;
	uint8_t     home_pet_count;
	/* list node to be linked to the online's players list */
	list_head_t online_hook;
};

/** 
  * @brief online keepalive timer type
  */
struct keepalive_timer_t {
    timer_struct_t* tmr;
    list_head_t     timer_list;
};

/** 
  * @brief statistic timer
  */
struct statistic_timer_t {
    list_head_t     timer_list;
};

/** 
  * @brief timer for writing last login online id to file pointed by fd
  */
struct wr_fd_timer_t {
    list_head_t     timer_list;
};

/**
  * @brief Define the friend list struct for specified user 
  */
struct friend_list_t 
{
	uint32_t cnt;
	userid_t uid[];
};

/**
 * @brief Define the information of vip
 */
struct vip_all_t
{
    uint32_t vip_is;
    uint32_t vip_month_cnt;
    uint32_t auto_incr;
    uint32_t start_tm;
    uint32_t end_tm;
    uint8_t  vip_type;
};
/**
  * @brief vip value struct
  */
struct vip_base_value_t
{
	uint32_t vip_base_val;
    /*! 1 for vip recharge; 0 for mibi cosumption */
	uint16_t syn_val_channel;
};

/**
 * @brief Define the information of official notice
 */
struct official_msg_t
{
    uint32_t index;
    uint32_t type; // 1:realtime 2:loop
    uint32_t frontid;
    uint8_t  msg[500];
};


struct db_send_mail_req_t {
	userid_t    from_id;
	uint8_t     from_nick[nick_size];
	int32_t     time;
	uint32_t    template_id;
	uint32_t    mail_len;
	uint8_t     mail[];
};




#define MAX_MAIL_TITLE_LEN 40
struct new_mail_notify_t
{
	uint32_t mail_id;
	uint32_t mail_time;
	uint32_t mail_state;
	uint32_t mail_templet;
	uint32_t mail_type;
	uint32_t sender_id;
	uint32_t sender_role_tm;
	char     mail_title[ MAX_MAIL_TITLE_LEN+1 ];
};




#pragma pack()
// =====================================================================================
class Online 
{
public:
static void init();
static void fini();
static void rewrite_olid_file();

/***************** Communication with Admin Server  *************************/
// process on admin's system message
static int adm_post_msg(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
static int adm_kick_user_offline(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
static int adm_syn_vip(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
static int adm_syn_vip_base_value(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
static int adm_syn_user_gcoins(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
/******************************************************************************************/

static int mcast_official_interface(char *msg);
static int mcast_official_world_notice(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

/***************** Communication with Online Server  *************************/
// receive info reported by online server
static int  report_online_info(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

// receive info reported by home online server
static int  report_home_online_info(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);


// process user online/offline pkg
static int  report_user_onoff(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

// check the given users' online status
static int  friends_online_status(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

// chat across servers via switch
static int  chat_across_svr(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

// chat across servers via switch real time
static int chat_across_svr_rltm(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

// send user attribute update notification
static int  user_attr_upd_noti(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

// check logined online id of the given user
static int  chk_user_location(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

// online keepalive package
static int  online_keepalive(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

// for statistic platform , report user number every 60 seconds
static int	report_user_num(void* owner, void* data);

static int	write_pre_olid_file(void* owner, void* data);

// Notice when map update
static int	user_map_upd_noti(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

// chat across servers via switch: no db operation
static int  chat_across_svr_2(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

// chat across server version 3( back package needs to be processed in online server ) 
static int	chat_across_svr_mentor_sys(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

// chat across server version 4( back package needs to be processed in online server ) 
static int	chat_across_svr_team_sys(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);


// update new invitee count(who complete freshman task)
static int	upd_new_invitee_cnt(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

//transfer battle invitation to invitee, and give response to invitor
static int send_btl_inv(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

// clear online info on connection closed
static void clear_online_info(int fd);
/******************************************************************************************/
// notify new mail to player
static int notify_player_new_mail(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);


/*************************** Communication with Login Server  *******************************************/
// get the remmended servers for users
static int get_recommeded_svrlist(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

// get servers list for the given range for users
static int get_ranged_svrlist(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
/*****************************************************************************************/

static int send_reward_msg_to_player(uint32_t recvid, uint32_t reward_id);

private:
// types
enum 
{
	/*! maximum online id */
	online_num_max	= 2048,

	online_player_num_max = 200,

	online_player_num_hot_lower = 100,
	online_player_num_hot_upper = 150,
	/*! maximal home online id */
	home_online_num_max	= 20,

    /*! */
};

typedef std::map<int, uint16_t> fd_map_t; 
/*! online id set */
typedef std::set<uint16_t> olid_set_t;   

typedef std::multimap<uint32_t, uint32_t> olid_map_t;   



/***************************inline functions************************************/
//
static player_info_t* get_user_info(userid_t uid)
{
	if (uid < s_max_uid_) {
		return &(s_usr_onoff_[uid]);
	}
	return NULL;
}
//
static uint16_t get_user_online_id(userid_t uid)
{ 
	//return ((uid < s_max_uid_) ? s_usr_onoff_[uid] : 0); 
	return ((uid < s_max_uid_) ? s_usr_onoff_[uid].online_id : 0); 
}
//
static void set_user_role_info(userid_t uid, uint32_t role_tm, uint32_t role_type, uint32_t vip_lv, uint8_t lv, uint8_t home_lv, uint32_t home_pet_count)
{ 
	if (uid < s_max_uid_) {
		s_usr_onoff_[uid].usrid = uid;
		s_usr_onoff_[uid].role_tm = role_tm; 
		s_usr_onoff_[uid].role_type = role_type; 
		s_usr_onoff_[uid].vip_lv = vip_lv; 
		s_usr_onoff_[uid].lv = lv;
	    s_usr_onoff_[uid].home_lv = home_lv;
		s_usr_onoff_[uid].home_pet_count = home_pet_count;	
	}
}
//
static void set_user_online_id(userid_t uid, uint16_t online_id)
{ 
	if (uid < s_max_uid_) {
		s_usr_onoff_[uid].online_id = online_id; 
	}
}
//
static uint16_t get_user_prev_online_id(userid_t uid)
{ 
	return ((uid < s_max_uid_) ? s_prev_logined_online_id_[uid] : 0); 
}
//
static void set_user_prev_online_id(userid_t uid, uint16_t online_id)
{ 
	s_prev_logined_online_id_[uid] = online_id; 
}

//
static bool is_guest(userid_t uid)
{ 	
	return uid > 2000000000; 
}

//
static bool is_valid_uid(userid_t uid)
{ 
	return uid < s_max_uid_; 
}
/******************************************************************************/



/******************************************************************************/
//
static const online_info_t* get_online_info(uint16_t online_id);
static int send_pkg_to_online(uint16_t online_id, const void* pkg, int pkglen);

/* close the active online fd in case that the keepalive timer expireds */
static int make_online_dead(void* owner, void* data);

/* common interface to kick user from the corresponding home online */
static void kick_usr_from_home(userid_t uid, uint32_t reason);

static void do_get_recommended_online_list(userid_t uid, friend_list_t* friendlist, uint16_t domain, uint32_t prev_online_id,
					olid_set_t& olid_frd_set, olid_set_t& olid_remain_set, olid_set_t& olid_nochat_set);

static void do_get_lv_recommended_online_list(userid_t uid, uint32_t lv, friend_list_t* friendlist, uint16_t domain, uint32_t prev_online_id,
										olid_set_t& olid_frd_set, olid_set_t& olid_lv_set, olid_set_t& olid_remain_set, olid_set_t& olid_nochat_set);

static void get_lv_match_Online_list(uint16_t domain, uint32_t lv, uint32_t prev_online_id, olid_set_t& olid_frd_set, olid_set_t& olid_lv_set);

static int add_online_player_interface(userid_t uid, uint8_t lv, uint8_t vip_lv);
static int del_online_player_interface(userid_t uid);

/******************************************************************************/



/****************private static fields*****************************************/
static uint32_t      s_max_online_id_; // max online id
static uint32_t      s_home_online_cnt; // home online count
static online_info_t s_online_info_[online_num_max]; //array to hold all online servers
static home_online_info_t s_home_online_info_[home_online_num_max]; //array to hold all home online servers
static keepalive_timer_t    s_keepalive_tmr_[online_num_max]; //keepalive timers for all online servers
static statistic_timer_t	 s_statistic_tmr_;
static char* 		 s_statistic_file_;
static uint32_t		 s_domain;
static wr_fd_timer_t	 s_wr_fd_tmr_;
static uint32_t	 		 s_wr_interval;
static fstream			 pre_olid_file;

static fd_map_t      s_fd_map_; // map from fd to online id
static userid_t      s_max_uid_; // max user id
static player_info_t*     s_usr_onoff_;     // hold all current logined users
static uint16_t*     s_prev_logined_online_id_;  // hold previous logined online id of each user
static olid_set_t    s_nochat_olid_set_;  // chat-forbidden online id set
static uint16_t      s_nochat_svrs_[];    // chat-forbidden online ids
static uint8_t       s_pkg_[pkg_size]; // uiniform buffer for sending package

//static uint32_t         m_30_players_num_;
//static uint32_t         m_40_players_num_;
//static uint32_t         m_50_players_num_;
//static uint32_t         m_vip_players_num_;
//static oluser_vec_t     m_ol_30_user_; // save userid of online
//static oluser_vec_t     m_ol_40_user_; // save userid of online
//static oluser_vec_t     m_ol_50_user_; // save userid of online
//static oluser_vec_t     m_ol_vip_user_; // save userid of online
//static GHashTable*      m_all_players_;
/******************************************************************************/

}; //end class online declaration

/**
  * @brief get the online infomation by online id
  * @param online_id online id
  * @return pointer to online_info_t on success, 0 on error
  */
inline const online_info_t*
Online::get_online_info(uint16_t online_id)
{
	int idx = online_id - 1;
	if ((online_id > 0) && (online_id <= online_num_max)
			&& s_online_info_[idx].online_id == online_id) 
	{
		return &(s_online_info_[idx]);
	}
	return 0;
}

/**
  * @brief receive info reported by online server
  * @param online_id online id
  * @param pkg package to be sent to online server 
  * @param pkglen package length
  * @return 0 on success, -1 on error
  */
inline int
Online::send_pkg_to_online(uint16_t online_id, const void* pkg, int pkglen)
{
	const online_info_t* olinfo = get_online_info(online_id);
	if (olinfo) 
	{
		return send_pkg_to_client(olinfo->fdsess, s_pkg_, pkglen);// with details in net_if.h
	} 
	else 
	{
		ERROR_LOG("Online Svr Info Not Found: olid=%d", online_id);
		return -1;
	}
}

void add_single_oluser_arr(uint32_t arr_idx, uint32_t uid);
void del_single_oluser_arr(uint32_t arr_idx, uint32_t vec_idx);
void init_global_data();
void fini_global_data();
void free_ol_player(void *p_ol);
void process_random_event();

#endif // SWITCH_ONLINE_HPP_

