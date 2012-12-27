#include <cerrno>
#include <cstdlib>
#include <string>
#include <vector>
#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/inet/byteswap.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>

extern "C" 
{
#include <fcntl.h>
#include <libtaomee/timer.h>
#include <libtaomee/utils.h>
#include <libtaomee/conf_parser/config.h>
#include <async_serv/net_if.h>
#include  <libtaomee/project/stat_agent/msglog.h>
#include <async_serv/mcast.h>
}

#include "dbproxy.hpp"
#include "utils.hpp"
#include "online.hpp"
#include "mcast_proto.hpp"

using namespace std;
using namespace taomee;

#define CHECK_USER_ID(uid_) \
		do { \
			if (!Online::is_valid_uid((uid_))) { \
				ERROR_LOG("invalid uid=%u", (uid_)); \
				return -1; \
			} \
		} while (0)


#pragma pack(1)

/**
  * @brief battle invitation type
  */
struct btl_inv_t {
	userid_t    invitee;
	userid_t    invitor;
	uint32_t	battle_mode;
	uint32_t    grpid;
	uint32_t    len;
	uint8_t     buf[];
};


struct oluser_classify_t {
    uint32_t    player_num;
    oluser_vec_t    ol_user_;
};

#pragma pack()

const uint32_t c_ol_player_size = sizeof(player_ol_t) + 4;

static GHashTable*     m_all_players_;


static const uint32_t oluser_guess_num[4] = {100000, 80000, 70000, 100000};
static oluser_classify_t oluser_arr[4] = {{0},};


/* Initialization of static members */
uint32_t           Online::s_max_online_id_;
uint32_t           Online::s_home_online_cnt;
online_info_t      Online::s_online_info_[online_num_max];
home_online_info_t      Online::s_home_online_info_[home_online_num_max];
keepalive_timer_t	Online::s_keepalive_tmr_[online_num_max];
Online::fd_map_t   Online::s_fd_map_;
Online::olid_set_t Online::s_nochat_olid_set_;
userid_t     Online::s_max_uid_;
player_info_t*    Online::s_usr_onoff_;
uint16_t*    Online::s_prev_logined_online_id_;
uint8_t      Online::s_pkg_[pkg_size];
statistic_timer_t	 Online::s_statistic_tmr_;
wr_fd_timer_t		Online::s_wr_fd_tmr_;
char* 		 Online::s_statistic_file_ = 0;
uint32_t     Online::s_domain = 0;

#ifndef VERSION_KAIXIN
static uint32_t s_lv_recommend[2][3][2] = 
{
	{{1001, 1140},
	 {1201, 1280},
	 {1301, 1320}},
	{{1141, 1200},
	 {1281, 1300},
	 {1321, 1340}}
};
#else
static uint32_t s_lv_recommend[2][3][2] = 
{
	{{1, 10},
	 {11, 20},
	 {21, 30}},
	{{31, 40},
	 {41, 50},
	 {51, 60}}
};

#endif
fstream		Online::pre_olid_file("here.bin");
uint32_t	Online::s_wr_interval;
#if 1
uint16_t   Online::s_nochat_svrs_[] = 
{  // chat-forbidden servers
	0
};
#else
uint16_t   Online::s_nochat_svrs_[] = 
{  // chat-forbidden servers
	2, 4, 6, 112, 115, 120, 129, 132, 143, 146, 148, 151, 154,
	171, 187, 193, 225, 230, 248, 249, 256, 258, 272, 278, 295, 296,
	297, 298, 299, 311, 316, 317, 333, 335, 339, 349, 358, 367, 375,
	383, 385, 395, 396, 420, 427, 430, 442, 445, 447, 450, 454, 459,
	462, 469, 470, 483, 490, 498, 522, 524, 534, 535, 542, 543, 566,
	576, 580, 582, 586, 587, 595, 601, 615, 617, 628, 629, 630, 636
};
#endif
/*! flag for statistic of users number */
//const uint32_t c_stat_overall_usr_num  = 0x05020001;
/*! flag for statistic of telecom users number */
//const uint32_t c_stat_telecom_usr_num  = 0x05020002;
/*! flag for statistic of netcom users number */
//const uint32_t c_stat_netcom_usr_num  = 0x05020003;

/*! flag for statistic of users number*/
const uint32_t c_stat_overall_usr_num = 0x09010002;
const uint32_t c_stat_overall_usr_num_wangtong = 0x0901000E;
const uint32_t c_stat_overall_usr_num_kaixin = 0x0F500001;
const uint32_t c_stat_overall_usr_num_tw = 0x0FA00001;
uint32_t	g_user_num = 0;

const char *version_str="VERSION:2010-07-07";

/**
  * @brief neccessary initialization
  */
void Online::init()
{
	int ok = -1;
	char pre_file[255] = { 0 };
	struct stat st;

	INFO_LOG("switch cur  VERSION:%s", version_str);
	
    srand(time(0));
	s_domain = config_get_intval("domain", 0);
    s_max_uid_ = config_get_intval("max_uid", 0);

	if ((s_statistic_file_ = config_get_strval("statistic_file"))) {
		INIT_LIST_HEAD(&(s_statistic_tmr_.timer_list));
		ADD_TIMER_EVENT(&s_statistic_tmr_, Online::report_user_num, 0, get_now_tv()->tv_sec + 60);
	}

    // init memory to hold the current logined online id of each user
    s_usr_onoff_ = reinterpret_cast<player_info_t*>(calloc(s_max_uid_, sizeof(*s_usr_onoff_)));
    if ((s_max_uid_ <= 0) || (s_usr_onoff_ == 0))
    {
        ERROR_LOG("failed to allocate %u * %lu bytes of space", s_max_uid_, sizeof(*s_usr_onoff_));
        goto fail;
    }

	s_prev_logined_online_id_ = reinterpret_cast<uint16_t*>(malloc(s_max_uid_ * sizeof(*s_prev_logined_online_id_)));
    // init memory to hold the previous logined online id of each user
	memcpy(pre_file, config_get_strval("online_id_file"), strlen(config_get_strval("online_id_file")));
	if (stat(pre_file, &st) == -1) { //file not exists
		pre_olid_file.open(pre_file, fstream::out | fstream::binary);
	} else { //file exists
		//fstream::in avoids original file content being erased
		pre_olid_file.open(pre_file, fstream::in | fstream::out | fstream::binary);
		pre_olid_file.read(reinterpret_cast<char*>(s_prev_logined_online_id_), s_max_online_id_ * sizeof(uint16_t));
	}

	if ((s_wr_interval = config_get_intval("wr_fd_interval", 0))) {
		INIT_LIST_HEAD(&(s_wr_fd_tmr_.timer_list));
		ADD_TIMER_EVENT(&s_wr_fd_tmr_, Online::write_pre_olid_file, s_prev_logined_online_id_, get_now_tv()->tv_sec + s_wr_interval);
	}

    for (size_t i = 0; i != array_elem_num(s_nochat_svrs_); ++i)
    {
        s_nochat_olid_set_.insert(s_nochat_svrs_[i]);
    }

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 3; j++)
			for (int n = 0; n < 2; n++)
				TRACE_LOG("%u ", s_lv_recommend[i][j][n]);

	
  	ok = 0;

fail:
    boot_log(ok, 0, "Online inited");
}

void iterator_fun(void * key, void* value, void* user_data)
{
     ERROR_LOG(" key value -----> [%u] [%p]", *(uint32_t*)key, value);
}

inline void hex_printf(char *buf,int len)
{
	
	char tmpbuf[10000] = "";
	char *pcur = tmpbuf;

	for (int i=0;i<len;i++)
	{
		sprintf(pcur,"%02x ",buf[i]&0xff);
		pcur += 3;
	}
	ERROR_LOG("hex printf: %s \n",tmpbuf);
	
}

int Online::add_online_player_interface(userid_t uid, uint8_t lv, uint8_t vip_lv)
{
    //ERROR_LOG("ON LINE [%u %u %u] size [%u]", uid, lv, vip_lv, g_hash_table_size(m_all_players_));
    player_ol_t* ol_ = reinterpret_cast<player_ol_t*>(g_slice_alloc0(c_ol_player_size));
    ol_->usrid   =   uid;
    ol_->lv      =   lv;
    ol_->vip_lv  =   vip_lv;

    if (lv >= 30) {
        add_single_oluser_arr(0, uid);
        ol_->index[0] = oluser_arr[0].player_num;
    }

    /*
    if (lv >= 40) {
        add_single_oluser_arr(1, uid);
        ol_->index[1] = oluser_arr[1].player_num;
    }
    if (lv >= 50) {
        add_single_oluser_arr(2, uid);
        ol_->index[2] = oluser_arr[2].player_num;
    }

    if (vip_lv > 0) {
        add_single_oluser_arr(3, uid);
        ol_->index[3] = oluser_arr[3].player_num;
    }*/

    if (ol_->index[0] || ol_->index[1] || ol_->index[2] || ol_->index[3]) {
        g_hash_table_insert(m_all_players_, &(ol_->usrid), ol_);
        //g_hash_table_foreach(m_all_players_, (GHFunc)iterator_fun, NULL);
    }
    //ERROR_LOG("---[%u %u %u %u]", oluser_arr[0].player_num, oluser_arr[1].player_num, oluser_arr[2].player_num, oluser_arr[3].player_num);
    return 0;
}

int Online::del_online_player_interface(userid_t uid)
{
    //ERROR_LOG("OFF LINE [%u] size [%u]", uid, g_hash_table_size(m_all_players_));
    //g_hash_table_foreach(m_all_players_, (GHFunc)iterator_fun, NULL);
    player_ol_t *p_ol = reinterpret_cast<player_ol_t*>(g_hash_table_lookup(m_all_players_, &uid));
    if (!p_ol) {
        ERROR_LOG("hash table not found [%u] !!!!!", uid);
        return 0;
    }

    if (p_ol->lv >= 30) {
        del_single_oluser_arr(0, p_ol->index[0]);
    }

    /*
    if (p_ol->lv >= 40) {
        del_single_oluser_arr(1, p_ol->index[1]); 
    }
    if (p_ol->lv >= 50) {
        del_single_oluser_arr(2, p_ol->index[2]);
    }
    if (p_ol->vip_lv > 0) {
        del_single_oluser_arr(3, p_ol->index[3]);
    }*/

    g_hash_table_remove(m_all_players_, &(p_ol->usrid));

    //ERROR_LOG("---[%u %u %u %u]", oluser_arr[0].player_num, oluser_arr[1].player_num, oluser_arr[2].player_num, oluser_arr[3].player_num);
    return 0;
}

//--------------------------------------------------------------------------------
// For Adminer Server
//--------------------------------------------------------------------------------
/**
  * @brief transfer system message from admin server
  * @param pkg package buffer
  * @param bodylen package body length
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int Online::adm_post_msg(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	int idx = 0;
	struct adm_msg_t {
		userid_t	recvid; // 0 stands for broadcasting
		uint32_t	npc_type;
		uint32_t	msg_tm; //msg time
		uint32_t	msglen;
		uint8_t		msg[];
	}__attribute__((packed));

	adm_msg_t* adm_msg = reinterpret_cast<adm_msg_t*>(pkg->body);
	CHECK_USER_ID(adm_msg->recvid);
	DEBUG_LOG("message %u %u %u %u ", adm_msg->recvid, adm_msg->npc_type, adm_msg->msg_tm, adm_msg->msglen);
	CHECK_VAL(adm_msg->msglen, bodylen - sizeof(adm_msg_t));

	//INFO_LOG("ADM MESSAGE\t[recv=%u npc=%u tm=%u msglen=%u]", adm_msg->recvid, adm_msg->npc_type, adm_msg->msg_tm, adm_msg->msglen);

	int offset = sizeof(svr_proto_t);
	*reinterpret_cast<userid_t*>(s_pkg_ + offset) = adm_msg->recvid;
	idx = sizeof(online_cli_proto_t);
	pack(s_pkg_ + offset + sizeof(userid_t), adm_msg->npc_type, idx); //network order
	pack(s_pkg_ + offset + sizeof(userid_t), adm_msg->msg_tm, idx);
	pack(s_pkg_ + offset + sizeof(userid_t), adm_msg->msglen, idx);
	pack(s_pkg_ + offset + sizeof(userid_t), pkg->body + sizeof(adm_msg_t), adm_msg->msglen, idx);

	online_cli_proto_t* ol_cli_pkg = reinterpret_cast<online_cli_proto_t*>(s_pkg_ + offset + sizeof(userid_t));
	ol_cli_pkg->len = bswap(idx);
	ol_cli_pkg->seqno = 0;
	ol_cli_pkg->cmd = bswap((uint16_t)(8002)); // send detailed notification
	ol_cli_pkg->id = 0; //system msg
	ol_cli_pkg->ret = 0;

	int sendlen = offset + sizeof(userid_t) + idx;
	init_proto_head(s_pkg_, 0, sendlen, adm_proto_post_msg, 0, 0);

	//hex_printf((char*)s_pkg_, sendlen);
	
	if (adm_msg->recvid == 0) 
	{ //broadcast msg to all players
		for(uint32_t i = 0; i < s_max_online_id_; i++) 
		{
			if (s_online_info_[i].online_id != 0) 
			{
				send_pkg_to_client(s_online_info_[i].fdsess, s_pkg_, sendlen);
			}
		}
	} 
	else
	{
		uint16_t olid = get_user_online_id(adm_msg->recvid);
		if (olid) //on
		{
			send_pkg_to_client(s_online_info_[olid - 1].fdsess, s_pkg_, sendlen);
		} 
		else  //off 
		{
			*reinterpret_cast<uint32_t*>(s_pkg_ + offset) = sendlen - offset;
			send_request_to_db(adm_msg->recvid, 0, dbcmd_offline_msg, s_pkg_ + offset, sendlen - offset); // no header
		}
	}

	return 0;
}

/**
  * @brief kick user offline
  * @param pkg package buffer
  * @param bodylen package body length
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int Online::adm_kick_user_offline(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	userid_t uid = *reinterpret_cast<userid_t*>(pkg->body);
	uint16_t olid = get_user_online_id(uid);
	if (olid) {
		//init_proto_head(pkg, pkg->sender_id, pkg->len, proto_kick_user_offline, 0, 0);
		/* send package to former online to kick user offline */
		//return send_pkg_to_client(s_online_info_[olid - 1].fdsess, pkg, pkg->len);

		/* build kick user offline package */
		int idx = sizeof(svr_proto_t);
		pack_h(s_pkg_, uid, idx);
		pack_h(s_pkg_, reason_repeated_login, idx);
		init_proto_head(s_pkg_, uid, idx, proto_kick_user_offline, 0, 0);
		
		/* send package to former online to kick user offline */
		send_pkg_to_client(s_online_info_[olid - 1].fdsess, s_pkg_, idx);

	}
	return 0;
}

void Online::get_lv_match_Online_list(uint16_t domain, uint32_t lv, uint32_t prev_online_id, olid_set_t& olid_frd_set, olid_set_t& olid_lv_set)
{
	uint32_t i = (lv - 1) / 10;
	uint32_t begin = s_lv_recommend[domain][i][0];
	uint32_t end   = s_lv_recommend[domain][i][1];
	olid_map_t olid_lv_map;
	TRACE_LOG("%u  %u %u", lv, begin, end);
	for (uint32_t online_id = begin; online_id != end; online_id++) {
		TRACE_LOG("%u  %u %u %u", lv, online_id, (uint32_t)(olid_frd_set.count(online_id)), s_online_info_[online_id - 1].online_id);
		if (online_id && (online_id != prev_online_id) && online_id < s_max_online_id_ 
				&& s_online_info_[online_id - 1].online_id
				&& (s_online_info_[online_id - 1].user_num < online_player_num_max)
				&& (olid_frd_set.count(online_id) == 0)
				&& (s_online_info_[online_id - 1].domain == domain))
		{
			olid_lv_map.insert(olid_map_t::value_type(s_online_info_[online_id - 1].user_num, online_id));
			TRACE_LOG("olid_lv_map insert ol id=%u, domain=%u", online_id, domain);
		}
	}
	uint32_t to_cnt = 6;
	for (olid_map_t::iterator it = olid_lv_map.begin(); it != olid_lv_map.end() && olid_lv_set.size() != to_cnt; ++it) {
		olid_lv_set.insert(it->second);
		TRACE_LOG("olid_lv_set insert ol id=%u %u %u", it->second, (uint32_t)(olid_lv_set.size()), (uint32_t)(olid_lv_map.size()));
	}
}


void Online::do_get_lv_recommended_online_list(userid_t uid, uint32_t lv, friend_list_t* friendlist, uint16_t domain, uint32_t prev_online_id,
										olid_set_t& olid_frd_set, olid_set_t& olid_lv_set, olid_set_t& olid_remain_set, olid_set_t& olid_nochat_set)
{
	uint32_t olid_frd_num = 1;
	uint32_t max_ol_num = prev_online_id == 0 ? 10 : 9;
	for (uint32_t i = 0; (i != friendlist->cnt) && (olid_frd_set.size() != olid_frd_num); ++i) 
	{
		uint16_t online_id = get_user_online_id(friendlist->uid[i]);
		if (online_id && (online_id != prev_online_id)
				//&& (s_online_info_[online_id - 1].user_num < 500)
				//&& (s_online_info_[online_id - 1].user_num < 300)
				//&& (s_online_info_[online_id - 1].user_num < 350)
				//&& (s_online_info_[online_id - 1].user_num < 400)
				&& (s_online_info_[online_id - 1].user_num < online_player_num_max)
				&& (s_nochat_olid_set_.count(online_id) == 0)
				&& (s_online_info_[online_id - 1].domain == domain))
		{
			TRACE_LOG("olid_frd_set insert ol id=%u, domain=%u", online_id, domain);
			olid_frd_set.insert(online_id);
		}
	}

	get_lv_match_Online_list(domain, lv, prev_online_id, olid_frd_set, olid_lv_set);

	uint32_t remain_cnt = max_ol_num - olid_lv_set.size() - olid_frd_set.size();

	for (uint32_t i = 0; (olid_remain_set.size() != remain_cnt) && (i != s_max_online_id_); ++i) 
	{
		if ((s_online_info_[i].online_id != prev_online_id)
				&& (s_online_info_[i].user_num >= online_player_num_hot_lower)
				&& (s_online_info_[i].user_num < online_player_num_hot_upper)
				&& !s_nochat_olid_set_.count(s_online_info_[i].online_id)
				&& !olid_frd_set.count(s_online_info_[i].online_id)
				&& (s_online_info_[i].domain == domain)) 
		{
			TRACE_LOG("olid_remain_set insert ol id=%u, domain=%u", s_online_info_[i].online_id, domain);
			olid_remain_set.insert(s_online_info_[i].online_id);
		}
	}
	
	// get remaining online server IDs to form the server list package
	for (uint32_t i = 0; (olid_remain_set.size() != remain_cnt) && (i != s_max_online_id_); ++i) 
	{
		if (s_online_info_[i].online_id
				&& (s_online_info_[i].online_id != prev_online_id)
				&& (s_online_info_[i].user_num < (online_player_num_max - 10))
				&& !s_nochat_olid_set_.count(s_online_info_[i].online_id)
				&& !olid_frd_set.count(s_online_info_[i].online_id)
				&& !olid_lv_set.count(s_online_info_[i].online_id)
				&& (s_online_info_[i].domain == domain)) 
		{
			TRACE_LOG("olid_remain_set insert ol id=%u, domain=%u", s_online_info_[i].online_id, domain);
			olid_remain_set.insert(s_online_info_[i].online_id);
		}
	}

	return;
}


void Online::do_get_recommended_online_list(userid_t uid, friend_list_t* friendlist, uint16_t domain, uint32_t prev_online_id,
										olid_set_t& olid_frd_set, olid_set_t& olid_remain_set, olid_set_t& olid_nochat_set)
{
	uint32_t olid_frd_num = prev_online_id == 0 ? 8 : 7;
	uint32_t max_ol_num = prev_online_id == 0 ? 10 : 9;
	for (uint32_t i = 0; (i != friendlist->cnt) && (olid_frd_set.size() != olid_frd_num); ++i) 
	{
		uint16_t online_id = get_user_online_id(friendlist->uid[i]);
		if (online_id && (online_id != prev_online_id)
				//&& (s_online_info_[online_id - 1].user_num < 500)
				//&& (s_online_info_[online_id - 1].user_num < 300)
				//&& (s_online_info_[online_id - 1].user_num < 350)
				//&& (s_online_info_[online_id - 1].user_num < 400)
				&& (s_online_info_[online_id - 1].user_num < online_player_num_max)
				&& (s_nochat_olid_set_.count(online_id) == 0)
				&& (s_online_info_[online_id - 1].domain == domain))
		{
			TRACE_LOG("olid_frd_set insert ol id=%u, domain=%u", online_id, domain);
			olid_frd_set.insert(online_id);
		}
	}

	// get hot servers
	uint32_t remain_cnt = max_ol_num - olid_frd_set.size();
	for (uint32_t i = 0; (olid_remain_set.size() != remain_cnt) && (i != s_max_online_id_); ++i) 
	{
		if ((s_online_info_[i].online_id != prev_online_id)
				//&& (s_online_info_[i].user_num >= 300)
				//&& (s_online_info_[i].user_num >= 150)
				//&& (s_online_info_[i].user_num >= 200)
				//&& (s_online_info_[i].user_num >= 250)
				&& (s_online_info_[i].user_num >= online_player_num_hot_lower)
				//&& (s_online_info_[i].user_num < 450)
				//&& (s_online_info_[i].user_num < 250)
				//&& (s_online_info_[i].user_num < 300)
				//&& (s_online_info_[i].user_num < 350)
				&& (s_online_info_[i].user_num < online_player_num_hot_upper)
				&& !s_nochat_olid_set_.count(s_online_info_[i].online_id)
				&& !olid_frd_set.count(s_online_info_[i].online_id)
				&& (s_online_info_[i].domain == domain)) 
		{
			TRACE_LOG("olid_remain_set insert ol id=%u, domain=%u", s_online_info_[i].online_id, domain);
			olid_remain_set.insert(s_online_info_[i].online_id);
		}
	}

	// get remaining online server IDs to form the server list package
	for (uint32_t i = 0; (olid_remain_set.size() != remain_cnt) && (i != s_max_online_id_); ++i) 
	{
		if (s_online_info_[i].online_id
				&& (s_online_info_[i].online_id != prev_online_id)
				//&& (s_online_info_[i].user_num < 300)
				//&& (s_online_info_[i].user_num < 150)
				//&& (s_online_info_[i].user_num < 200)
				//&& (s_online_info_[i].user_num < 250)
				&& (s_online_info_[i].user_num < (online_player_num_max - 10))
				&& !s_nochat_olid_set_.count(s_online_info_[i].online_id)
				&& !olid_frd_set.count(s_online_info_[i].online_id)
				&& (s_online_info_[i].domain == domain)) 
		{
			TRACE_LOG("olid_remain_set insert ol id=%u, domain=%u", s_online_info_[i].online_id, domain);
			olid_remain_set.insert(s_online_info_[i].online_id);
		}
	}
#if 0
	uint16_t saved_olid = 0; // first chat-forbidden online server id
	for (int i = 0, cnt = 0; (cnt != 2) && (i != 10); ++i) 
	{
		int olid_idx = s_nochat_svrs_[rand() % array_elem_num(s_nochat_svrs_)] - 1;
		if ((s_online_info_[olid_idx].online_id != 0)
				&& (s_online_info_[olid_idx].online_id != prev_online_id)
				&& (s_online_info_[olid_idx].online_id != saved_olid)
				//&& (s_online_info_[olid_idx].user_num < 500)) 
				//&& (s_online_info_[olid_idx].user_num < 300)) 
				//&& (s_online_info_[olid_idx].user_num < 350)) 
				&& (s_online_info_[olid_idx].user_num < 400)
				&& (s_online_info_[olid_idx].domain == domain)) 
		{
			TRACE_LOG("olid_nochat_set insert ol id=%u, domain=%u", s_online_info_[olid_idx].online_id, domain);
			olid_nochat_set.insert(s_online_info_[olid_idx].online_id);
			++cnt;
			saved_olid = s_online_info_[olid_idx].online_id;
		}
	}
#endif
	return;
}

/**
  * @brief synchronize vip flag
  * @param pkg package buffer
  * @param bodylen package body length
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int Online::adm_syn_vip(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen, sizeof(vip_all_t));

	vip_all_t* vip_in = reinterpret_cast<vip_all_t*>(pkg->body);
    DEBUG_LOG("SYN VIP VALUE: uid=%u, flag=%u, month=%u", pkg->sender_id, vip_in->vip_is, vip_in->vip_month_cnt);
//    hex_printf((char*)pkg, 38);
    uint16_t online_id = get_user_online_id(pkg->sender_id);
    if (online_id) {
        //sync vip information to online
        init_proto_head(pkg, pkg->sender_id, pkg->len, adm_proto_syn_vip_flag, 0, 0);
        send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
    }
/*
#define FIRST_BECOME_YEAR_VIP_FLAG 8
    if (test_bit_on(vip_type, FIRST_BECOME_YEAR_VIP_FLAG)) {
        string msg =" ";
    }
*/
#ifdef SYS_SEND_MAIL
	string msg;
	if (vip_in->vip_is == 1) {
		msg = "恭喜你开通了超灵侠士!";
	} else if (vip_in->vip_is == 2) {
		msg = "你的NoNo已经充能完毕咯，它重新获得了超能晶体的超级能量，可以陪你一块闯天下了，快召唤它试试吧!"; 
	}

	//send mail
	db_send_mail_req_t* req = reinterpret_cast<db_send_mail_req_t*>(s_pkg_);
	req->from_id = npc_shawn_id;
	memset(req->from_nick, 0, nick_size);
	req->time = get_now_tv()->tv_sec;
	req->template_id = sys_mail_template;
	req->mail_len = strlen(msg.c_str());
	memcpy(req->mail, msg.c_str(), req->mail_len);
	uint32_t pkg_len = sizeof(db_send_mail_req_t) + req->mail_len;
	send_request_to_db(pkg->sender_id, 0, dbcmd_send_mail, req, pkg_len);

	// give noti to client to get mail
	int offset = sizeof(svr_proto_t);
	*reinterpret_cast<userid_t*>(s_pkg_ + offset) = pkg->sender_id;
	int idx = sizeof(online_cli_proto_t);
	online_cli_proto_t* ol_cli_pkg = reinterpret_cast<online_cli_proto_t*>(s_pkg_ + offset + sizeof(userid_t));
	ol_cli_pkg->len = bswap(idx);
	ol_cli_pkg->seqno = 0;
	ol_cli_pkg->cmd = bswap(8008); // new mail noti
	ol_cli_pkg->id = pkg->sender_id; 
	ol_cli_pkg->ret = 0;
	int sendlen = offset + sizeof(userid_t) + idx;
    uint16_t online_id = get_user_online_id(pkg->sender_id);
    if (online_id) { //notify user to get mail if online
        init_proto_head(s_pkg_, 0, sendlen, adm_proto_sys_mail_noti, 0, 0);
        send_pkg_to_client(s_online_info_[online_id - 1].fdsess, s_pkg_, sendlen);
    }
#endif
	return 0;
}

int Online::adm_syn_vip_base_value(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen, sizeof(vip_base_value_t));

	DEBUG_LOG("SYN VIP VALUE: uid=%u", pkg->sender_id);
	uint16_t online_id = get_user_online_id(pkg->sender_id);
	if (online_id) {
		init_proto_head(pkg, pkg->sender_id, pkg->len, adm_proto_syn_vip_value, 0, 0);
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
	}

	return 0;
}

/**
  * @brief synchronize user gold coins
  * @param pkg package buffer
  * @param bodylen package body length
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int Online::adm_syn_user_gcoins(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen, 4);

	uint16_t online_id = get_user_online_id(pkg->sender_id);
	DEBUG_LOG("SYN USR GCOINS: uid=%u gcoins=%u online=%u", pkg->sender_id, *reinterpret_cast<uint32_t*>(pkg->body), online_id);
	if (online_id) {
		init_proto_head(pkg, pkg->sender_id, pkg->len, adm_proto_syn_user_gcoins, 0, 0);
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
	}

	return 0;
}

int Online::mcast_official_interface(char *msg)
{
    int mcast_idx = sizeof(mcast_pkg_t);
    //pack_h(s_pkg_, static_cast<uint32_t>(2),mcast_idx); //1:player_world 2:offical_world
    pack_h(s_pkg_, static_cast<uint32_t>(998), mcast_idx);
    pack_h(s_pkg_, static_cast<uint32_t>(2), mcast_idx);
    pack_h(s_pkg_, static_cast<uint32_t>(2), mcast_idx);
    pack(s_pkg_, msg, 500, mcast_idx);
    init_mcast_pkg_head(s_pkg_, mcast_world_notice, 0);
    return send_mcast_pkg(s_pkg_, mcast_idx);
}

/**
  * @brief mcast official notice
  * @param pkg package buffer
  * @param bodylen package body length
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int Online::mcast_official_world_notice(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen, sizeof(official_msg_t));

    official_msg_t* msg = reinterpret_cast<official_msg_t*>(pkg->body);
    DEBUG_LOG("OFFICIAL NOTICE: index=%u, type=%u, front_id=%u", msg->index, msg->type, msg->frontid);

    int mcast_idx = sizeof(mcast_pkg_t);
    //pack_h(s_pkg_, static_cast<uint32_t>(2),mcast_idx); //1:player_world 2:offical_world
    pack_h(s_pkg_, msg->index, mcast_idx);
    pack_h(s_pkg_, msg->type, mcast_idx);
    pack_h(s_pkg_, msg->frontid, mcast_idx);
    pack(s_pkg_, msg->msg, 500, mcast_idx);
    init_mcast_pkg_head(s_pkg_, mcast_world_notice, 0);
    send_mcast_pkg(s_pkg_, mcast_idx);

	return 0;
}

//-------------------------------------------------------------
// For Online Server
// ------------------------------------------------------------
/**
  * @brief receive info reported by online server
  * @param pkg package buffer
  * @param bodylen package body length
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int Online::report_online_info(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	// define online information 
	struct online_info_req_t 
	{
		uint16_t	domain_id;
		uint32_t	online_id;
		uint8_t		online_ip[16];
		uint16_t	online_port;
		uint32_t	user_num;
	} __attribute__((packed));

	CHECK_VAL(bodylen, sizeof(online_info_req_t));

	online_info_req_t* req = reinterpret_cast<online_info_req_t*>(pkg->body);
	if ((req->online_id > online_num_max) || (req->online_id == 0)) 
	{
		ERROR_LOG("invalid onlineid=%u from fd=%d", req->online_id, fdsess->fd);
		return -1;
	}

	int idx = req->online_id - 1;
	// this can happen on a very special case
	if (s_online_info_[idx].online_id == req->online_id) 
	{
		DEBUG_LOG("DUPLICATE ONLINE ID\t[id=%u]", req->online_id);
		// close previous fd
		close_client_conn(s_online_info_[idx].fdsess->fd);
	}

	/*update element to online info structure online::s_online_info_[online_num_max]*/	
	s_online_info_[idx].domain   = req->domain_id;
	s_online_info_[idx].online_id   = req->online_id;
	memcpy(s_online_info_[idx].online_ip, req->online_ip, sizeof(s_online_info_[0].online_ip));
	s_online_info_[idx].online_port = req->online_port;
	s_online_info_[idx].user_num    = req->user_num;
	s_online_info_[idx].fdsess      = fdsess;
	
 	// make an online connection dead if no package is received in 70 secs
	INIT_LIST_HEAD(&(s_keepalive_tmr_[idx].timer_list));
	s_keepalive_tmr_[idx].tmr = ADD_TIMER_EVENT(&s_keepalive_tmr_[idx], Online::make_online_dead, &s_online_info_[idx], get_now_tv()->tv_sec + 70);
	//DEBUG_LOG("SETUP KEEP ALIVE TIMER olid=%u", req->online_id);

	INIT_LIST_HEAD(&(s_online_info_[idx].player_list));

	s_fd_map_[fdsess->fd] = req->online_id;//map<fd, online_id>

	//update s_max_online_id_
	if (s_max_online_id_ < req->online_id) 
	{
		s_max_online_id_ = req->online_id;
	}

	DEBUG_LOG("ONLINE INFO\t[max=%u fd=%d olid=%u olip=%.16s olport=%d domain=%u usrnum=%u]",
				s_max_online_id_, fdsess->fd, req->online_id, req->online_ip, req->online_port, req->domain_id, req->user_num);

	return 0;
}

/**
  * @brief receive info reported by home online server
  * @param pkg package buffer
  * @param bodylen package body length
  * @param fdsess fd related information of home online
  * @return 0 on success, -1 on error
  */
int Online::report_home_online_info(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen,4); 

	uint32_t online_id = *(reinterpret_cast<uint32_t*>(pkg->body));
	uint32_t idx = online_id - home_online_base;
	if ((online_id < home_online_base) || (idx >= home_online_num_max)) 
	{
		ERROR_LOG("invalid home onlineid=%u from fd=%d", online_id, fdsess->fd);
		return -1;
	}

	//A very special case
	if (s_home_online_info_[idx].online_id == online_id) 
	{
		DEBUG_LOG("DUPLICATED ONLINE ID\t[id=%u]", online_id);
		// close previous fd
		close_client_conn((s_home_online_info_[idx].fdsess)->fd);
	}

	/* initialize home online info */	
	s_home_online_info_[idx].online_id   = online_id;
	s_home_online_info_[idx].fdsess      = fdsess;
	
   	s_fd_map_[fdsess->fd] = online_id; //map<fd, online_id>
	
	//update s_home_online_cnt
	if (s_home_online_cnt < idx + 1) 
	{
		s_home_online_cnt = idx + 1;
	}

	DEBUG_LOG("HOME ONLINE INFO\t[max_home_online=%u fd=%d olid=%u]", s_home_online_cnt, fdsess->fd, online_id);

	return 0;
}
/**
  * @brief close the active online fd in case that the keepalive timer expireds
  * @return 0 on success, -1 on error
  */
int Online::make_online_dead(void* owner, void* data)
{
	online_info_t* on_info = reinterpret_cast<online_info_t*>(data);

	DEBUG_LOG("KEEPALIVE TIMER EXPIRED, MAKE ONLINE DEAD\t[fd=%d id=%u ip=%s port=%d]",
			on_info->fdsess->fd, on_info->online_id,
			on_info->online_ip, on_info->online_port);

	close_client_conn(on_info->fdsess->fd);
	return 0;
}


/**
  * @brief process on user online/offline
  * @param pkg package buffer
  * @param bodylen package body length
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int Online::report_user_onoff(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen, 16);

	uint16_t cur_online_id = s_fd_map_[fdsess->fd]; //online id of the server the sender is going to login to
	int idx = cur_online_id - 1;
	CHECK_VAL_GE(idx, 0);

	userid_t uid = pkg->sender_id;
	uint8_t onoff_flag;
	uint32_t role_tm;
	uint32_t role_type;
	uint32_t vip_lv;
	uint8_t  lv;
	uint8_t  home_lv = 0;
	uint8_t  home_pet_count =0;
	int idx2 = 0;
	unpack_h(pkg->body, onoff_flag, idx2);
	unpack_h(pkg->body, role_tm, idx2);
	unpack_h(pkg->body, role_type, idx2);
	unpack_h(pkg->body, vip_lv, idx2);
	unpack_h(pkg->body, lv, idx2);
	unpack_h(pkg->body, home_lv, idx2);
	unpack_h(pkg->body, home_pet_count, idx2);

    TRACE_LOG("[%u %u %u %u %u %u]", onoff_flag, role_tm, role_type, vip_lv, lv, cur_online_id);
	if (!is_guest(uid)) 
	{ 
		CHECK_USER_ID(uid);

		if (onoff_flag)
		{  //user online action
			uint16_t online_id = get_user_online_id(uid); // The sender's former online id is supposed to be 0(offline)

			if (online_id == cur_online_id)
			{
				ERROR_LOG("impossible error! online id match: former=%d now=%d", online_id, cur_online_id);
				return -1;
			}

			if (online_id != 0) // The user has logined on certain online server (not current online server), repeated logining
			{
				// notify the "certain online server" to kick the user offline
				DEBUG_LOG("MULTIPLE LOGIN OF USER\t[online_id=%u %u userid=%u]", online_id, cur_online_id, uid);	

				list_del_init(&(s_usr_onoff_[uid].online_hook));
				
				/* build kick user offline package */
				int idx = sizeof(svr_proto_t);
				pack_h(s_pkg_, uid, idx);
				pack_h(s_pkg_, reason_repeated_login, idx);
				init_proto_head(s_pkg_, uid, idx, proto_kick_user_offline, 0, 0);

				/* send package to former online to kick user offline */
				send_pkg_to_client(s_online_info_[online_id - 1].fdsess, s_pkg_, idx);


				/* send package to all home online to kick user offline */
				for (uint32_t i = 0; i < s_home_online_cnt; i++) {
					if (s_home_online_info_[i].online_id != 0) {
						send_pkg_to_client(s_home_online_info_[i].fdsess, s_pkg_, idx);
					}
				}
                del_online_player_interface(uid);
			}

			++(s_online_info_[idx].user_num);
			list_add_tail(&(s_usr_onoff_[uid].online_hook), &(s_online_info_[idx].player_list));	

			if (static_cast<int>(uid) < config_get_intval("max_uid", 0)) {
				set_user_online_id(uid, cur_online_id);
				set_user_role_info(uid, role_tm, role_type, vip_lv, lv, home_lv, home_pet_count);
			}

			set_user_prev_online_id(uid, cur_online_id);

            add_online_player_interface(uid, lv, vip_lv);
            //ERROR_LOG("trace on -----> [%u] [%u %u]", uid, online_id, cur_online_id);
			
			DEBUG_LOG("USER ONLINE NOTICE\t[online_id=%u id=%u role_tm=%u role_type=%u usrnum=%u]", 
					cur_online_id, uid, role_tm, role_type, s_online_info_[idx].user_num);	
		} 
		else 
		{ //user offline action
			--(s_online_info_[idx].user_num); //decrease user count in the current online server
			if (get_user_online_id(uid) == cur_online_id) 
			{
				set_user_online_id(uid, 0); //reset online_id 0, with meaning user offline
				list_del_init(&(s_usr_onoff_[uid].online_hook));	

				kick_usr_from_home(uid, 0);
				DEBUG_LOG("USER OFFLINE NOTICE\t[online_id=%u id=%u usrnum=%u]", 
					cur_online_id, uid, s_online_info_[idx].user_num);	
			}
            del_online_player_interface(uid);
            //ERROR_LOG("trace off -----> [%u] [%u %u]", uid, get_user_online_id(uid), cur_online_id);
		}
	} 
	else 
	{  //guest user
		if (onoff_flag)  // on line action
		{
			++(s_online_info_[idx].user_num);
			g_user_num++;
		} 
		else   //off line action
		{
			--(s_online_info_[idx].user_num);
			g_user_num = g_user_num > 0 ? g_user_num-1 : 0;
		}
		DEBUG_LOG("G_USER_NUM: %u", g_user_num);
	}

	//Make sure user number above 0
	if ((static_cast<int>(s_online_info_[idx].user_num)) < 0) 
	{
		ERROR_LOG("impossible error! user num=%d", s_online_info_[idx].user_num);
		return -1;
	}
	if (onoff_flag)  // on line action
	{
		g_user_num++;
	} 
	else   //off line action
	{
		g_user_num = g_user_num > 0 ? g_user_num-1 : 0;
	}
	DEBUG_LOG("G_USER_NUM: %u", g_user_num);

	return 0;
}

/**
  * @brief check the online status of the checkees
  * @param pkg package buffer
  * @param bodylen package body length
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int Online::friends_online_status(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
    // pkg->body includes the user's list
	CHECK_VAL_GE(bodylen, sizeof(friend_list_t));

	friend_list_t* friendlist = reinterpret_cast<friend_list_t*>(pkg->body);

	CHECK_VAL(bodylen, sizeof(friend_list_t) + friendlist->cnt * sizeof(userid_t));

	int idx = sizeof(svr_proto_t) + 4;
	int idx2 = sizeof(svr_proto_t);
	uint32_t on_cnt = 0;

	for (uint32_t i = 0; i != friendlist->cnt; ++i) 
	{
		player_info_t* p = get_user_info(friendlist->uid[i]);
		//uint32_t online_id = get_user_online_id(friendlist->uid[i]);
		if (p && p->online_id) 
		{
			pack_h(s_pkg_, friendlist->uid[i], idx); 
			pack_h(s_pkg_, p->role_tm, idx); 
			pack_h(s_pkg_, p->role_type, idx); 
			pack_h(s_pkg_, p->online_id, idx); 
			pack_h(s_pkg_, static_cast<uint32_t>(p->vip_lv), idx); 
			pack_h(s_pkg_, static_cast<uint32_t>(p->lv), idx);
			pack_h(s_pkg_, static_cast<uint32_t>(p->home_lv), idx);
			pack_h(s_pkg_, static_cast<uint32_t>(p->home_pet_count), idx);
			DEBUG_LOG("PROCESS CHK FRIEND ONLINE STATUS IN SWITCH\t[id=%u role_tm=%u role_type=%d online_id=%u],idx=%u", 
					friendlist->uid[i],p->role_tm,p->role_type,p->online_id,idx);
			//pack_h(s_pkg_, (s_usr_onoff_[friendlist->uid[i]].map_id), idx); 
			on_cnt++;
		}
	}

	pack_h(s_pkg_, on_cnt, idx2); 
	init_proto_head(s_pkg_, 0, idx, pkg->cmd, 0, pkg->seq);// sender_id 0
	//DEBUG_LOG("PROCESS CHK FRIEND ONLINE STATUS IN SWITCH\t[cnt=%u on_cnt=%u ip=%s]", friendlist->cnt, on_cnt, buf);
	
	return send_pkg_to_client(fdsess, s_pkg_, idx);
}

/**
  * @brief chat across different servers 
  * @param pkg package buffer
  * @param bodylen package body length
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int Online::chat_across_svr(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL_GE(bodylen, 4);

	/*parse package*/
	userid_t* p_uid     = reinterpret_cast<userid_t*>(pkg->body);
	userid_t  receiver_id = *(p_uid); 
    //hex_printf((char *)(pkg->body), 30);
	DEBUG_LOG("chat across server: fromid=%u toid=%u type=%u", 
			pkg->sender_id, receiver_id, ntohl(*(uint32_t*)(pkg->body + 4 + 18)));

	uint16_t  online_id = get_user_online_id(receiver_id);
	if (online_id) 
	{
		// receiver's online, dispatch the msg to the corresponding online server
		DEBUG_LOG("send to on recver: fromid=%u toid=%u", pkg->sender_id, receiver_id);
		init_proto_head(pkg, pkg->sender_id, pkg->len, pkg->cmd, 0, 0); //build header
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
	} 
	else 
	{
		// user's offline, send offline msg to db proxy
		*p_uid = bodylen; 
		send_request_to_db(receiver_id, 0, dbcmd_offline_msg, pkg->body, bodylen);
	}

	return 0;
}


/**
  * @brief chat across different servers 
  * @param pkg package buffer
  * @param bodylen package body length
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int Online::chat_across_svr_rltm(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL_GE(bodylen, 4);

	/*parse package*/
	userid_t* p_uid     = reinterpret_cast<userid_t*>(pkg->body);
	userid_t  receiver_id = *(p_uid); 
	DEBUG_LOG("chat across server: fromid=%u toid=%u type=%u", 
			pkg->sender_id, receiver_id, ntohl(*(uint32_t*)(pkg->body + 4 + 17)));

	uint16_t  online_id = get_user_online_id(receiver_id);
	if (online_id) 
	{
		// receiver's online, dispatch the msg to the corresponding online server
		DEBUG_LOG("send to on recver: fromid=%u toid=%u", pkg->sender_id, receiver_id);
		init_proto_head(pkg, pkg->sender_id, pkg->len, pkg->cmd, 0, 0); //build header
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
	} 

	return 0;
}

/**
  * @brief receive user attribute update notification 
  * @param pkg package buffer
  * @param bodylen package body length
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int Online::user_attr_upd_noti(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct attr_upd_noti_t 
	{
		userid_t recver_id;
		uint32_t type;
	} __attribute__((packed));

	CHECK_VAL(bodylen, sizeof(attr_upd_noti_t));

	attr_upd_noti_t* noti = reinterpret_cast<attr_upd_noti_t*>(pkg->body);
	uint16_t online_id = get_user_online_id(noti->recver_id);

	if (online_id) 
	{ //receiver online
		int idx = sizeof(svr_proto_t);
		pack_h(s_pkg_, noti->recver_id, idx);
		pack_h(s_pkg_, noti->type, idx);
		init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, 0);

		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, s_pkg_, idx);
	}

	return 0;
}

/**
  * @brief check user location, say, online server
  * @param pkg package buffer
  * @param bodylen package body length
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int Online::chk_user_location(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct location_req_t 
	{
		userid_t checkee_id;
	} __attribute__((packed));

	CHECK_VAL(bodylen, sizeof(location_req_t));

	location_req_t* req = reinterpret_cast<location_req_t*>(pkg->body);

	int      idx        = sizeof(svr_proto_t);
	uint32_t online_id  = get_user_online_id(req->checkee_id);

	pack_h(s_pkg_, online_id, idx);  // build package body
	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, 0); // build package header

	return send_pkg_to_client(fdsess, s_pkg_, idx);
}


/**
  * @brief keep online alive 
  * @param pkg package buffer 
  * @param bodylen package body length
  * @param fdsess fd related information of sender 
  * @return 0 on success, -1 on error
  */
int Online::online_keepalive(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	int idx = s_fd_map_[fdsess->fd] - 1;
	CHECK_VAL_GE(idx, 0);

	mod_expire_time(s_keepalive_tmr_[idx].tmr, get_now_tv()->tv_sec + 70);

	return 0;
}

/**
  * @brief callback of timer: give the overall user number for statistic
  * @return 0 on success, -1 on error
  */
int Online::report_user_num(void* owner, void* data)
{
	int cnt = 0;
	for(uint32_t i = 0; i < s_max_online_id_; i++) {
		cnt += s_online_info_[i].user_num;
		/*
		if (s_online_info_[i].domain == 0) { //telecom
			cnt_tele += s_online_info_[i].user_num;
		} else { //netcom
			cnt_net += s_online_info_[i].user_num;
		}
		*/
	}

	/* do statistics */
	int32_t now = get_now_tv()->tv_sec;
#ifdef VERSION_KAIXIN
	msglog(s_statistic_file_, c_stat_overall_usr_num_kaixin, now, &cnt, 4);
#elif defined VERSION_TAIWAN
    msglog(s_statistic_file_, c_stat_overall_usr_num_tw, now, &cnt, 4);
#else
	if (s_domain) {
		msglog(s_statistic_file_, c_stat_overall_usr_num_wangtong, now, &cnt, 4);
	} else {
		msglog(s_statistic_file_, c_stat_overall_usr_num, now, &cnt, 4);
	}
#endif	
//	msglog(s_statistic_file_, c_stat_telecom_usr_num, now, &cnt_tele, 4);
//	msglog(s_statistic_file_, c_stat_netcom_usr_num, now, &cnt_net, 4);

	ADD_TIMER_EVENT(&s_statistic_tmr_, Online::report_user_num, 0, get_now_tv()->tv_sec + 60);

    return 0;
}

/**
  * @brief callback of timer: write previous login online id to file 
  * @return 0 on success, -1 on error
  */
int Online::write_pre_olid_file(void* owner, void* data) 
{
	uint32_t length = 200 * 1024;
	char* ptr = reinterpret_cast<char*>(data);
	if (((ptr + length) < (reinterpret_cast<char*>(s_prev_logined_online_id_) + 2 * s_max_uid_))) {
		if (s_wr_interval) {
			DEBUG_LOG("add timer and write");
			pre_olid_file.write(ptr, length);
			pre_olid_file.flush();
			ADD_TIMER_EVENT(&s_wr_fd_tmr_, Online::write_pre_olid_file, ptr + length, get_now_tv()->tv_sec + s_wr_interval);
		}
	} else {
		length = reinterpret_cast<char*>(s_prev_logined_online_id_) + 2 * s_max_uid_ - ptr;
		if (s_wr_interval) {
			DEBUG_LOG("add timer and write");
			pre_olid_file.write(ptr, length);
			pre_olid_file.flush();
			pre_olid_file.seekp(0, fstream::beg);
			ADD_TIMER_EVENT(&s_wr_fd_tmr_, Online::write_pre_olid_file, s_prev_logined_online_id_, get_now_tv()->tv_sec + s_wr_interval);
		}
	}
	return 0;
}

void Online::rewrite_olid_file()
{
	int old_interval = s_wr_interval;
	s_wr_interval = config_get_intval("wr_fd_interval", 0);
	if ((old_interval == 0) && (s_wr_interval != 0) && (list_empty(&(s_wr_fd_tmr_.timer_list)))) {
		DEBUG_LOG("re-add timer");
		ADD_TIMER_EVENT(&s_wr_fd_tmr_, Online::write_pre_olid_file, s_prev_logined_online_id_, get_now_tv()->tv_sec + s_wr_interval);
	}
}

/**
  * @brief notice when user update map
  * @return 0 on success, -1 on error
  */
int Online::user_map_upd_noti(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen, 8); //map id
	//uint64_t map_id = *reinterpret_cast<uint64_t*>(pkg->body);
	if (static_cast<int>(pkg->sender_id) < config_get_intval("max_uid", 0)) {
		//s_usr_onoff_[pkg->sender_id].map_id = map_id;
	} else {
		ERROR_LOG("map update invalid: uid=%u max_uid=%u", pkg->sender_id, config_get_intval("max_uid", 0));
		return -1;
	}
	return 0;
}
/**
  * @brief post message across switch, version 2
  * @return 0 on success, -1 on error
  */
int Online::chat_across_svr_2(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL_GE(bodylen, sizeof(userid_t));

	/*parse package*/
	userid_t receiver_id = *(reinterpret_cast<userid_t*>(pkg->body));

	uint16_t  online_id = get_user_online_id(receiver_id);
	if (online_id) 
	{
		// receiver's online, dispatch the msg to the corresponding online server
		init_proto_head(pkg, pkg->sender_id, pkg->len, pkg->cmd, 0, 0); //build header
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
	} 

	return 0;
}

/**
  * @brief post message across switch, version 3
  *  online server needs to process on the package transferred by switch.
  * @return 0 on success, -1 on error
  */
int	Online::chat_across_svr_mentor_sys(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL_GE(bodylen, 4);

	/*parse package*/
	userid_t* p_uid     = reinterpret_cast<userid_t*>(pkg->body);
	userid_t  receiver_id = *(p_uid); 
	
	uint16_t  online_id = get_user_online_id(receiver_id);
	if (online_id) 
	{
		// receiver's online, dispatch the msg to the corresponding online server
		init_proto_head(pkg, pkg->sender_id, pkg->len, pkg->cmd, 0, 0); //build header
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
	} 
	else 
	{
		// user's offline, send offline msg to db proxy
		*p_uid = bodylen; 
		send_request_to_db(receiver_id, 0, dbcmd_offline_msg, pkg->body, bodylen);
	}

	return 0;
}

/**
  * @brief post message across switch, version 4
  *  online server needs to process on the package transferred by switch.
  * @return 0 on success, -1 on error
  */
int Online::chat_across_svr_team_sys(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL_GE(bodylen, 4);

	/*parse package*/
	userid_t* p_uid     = reinterpret_cast<userid_t*>(pkg->body);
	userid_t  receiver_id = *(p_uid); 

	uint16_t  online_id = get_user_online_id(receiver_id);
	if (online_id) 
	{
		// receiver's online, dispatch the msg to the corresponding online server
		init_proto_head(pkg, pkg->sender_id, pkg->len, pkg->cmd, 0, 0); //build header
		send_pkg_to_client(s_online_info_[online_id - 1].fdsess, pkg, pkg->len);
	} 
	else 
	{
		// user's offline, send offline msg to db proxy
		*p_uid = bodylen; 
		send_request_to_db(receiver_id, 0, dbcmd_offline_msg, pkg->body, bodylen);
	}

	return 0;

}

/**
  * @brief update new invitee count of player
  * @return 0 on success, -1 on error
  */
int	Online::upd_new_invitee_cnt(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	userid_t* invitor = reinterpret_cast<userid_t*>(pkg->body);
	uint16_t olid = get_user_online_id(*invitor);	
	if (olid) {
		return send_pkg_to_client(s_online_info_[olid - 1].fdsess, pkg, pkg->len);
	} 

	return 0;
}

/**
  * @brief transfer battle invitation to invitee, and give response to invitor
  * @return 0 on success, -1 on error
  */
int Online::send_btl_inv(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL_GE(bodylen, sizeof(btl_inv_t));
	int idx = sizeof(svr_proto_t);

	btl_inv_t* inv_pkg = reinterpret_cast<btl_inv_t*>(pkg->body);
	CHECK_VAL(bodylen, sizeof(btl_inv_t) + inv_pkg->len);
	uint16_t olid = get_user_online_id(inv_pkg->invitee);
	uint16_t olid2 = get_user_online_id(inv_pkg->invitor);
	DEBUG_LOG("SEND BATTLE INV\t[invitor %u on %u, invitee %u on %u]", inv_pkg->invitor, olid2, inv_pkg->invitee, olid);
	if (olid) {
		pack_h(s_pkg_, 1, idx);
		init_proto_head(s_pkg_, 0, idx, proto_send_btl_inv, 0, pkg->seq);
		send_pkg_to_client(fdsess, s_pkg_, idx); //response to invitor
		/* transfer invitation to invitee.	note: we appoint with online that the sender_id is used to transfer recverid particularly
			, just for convinence */
		init_proto_head(pkg, inv_pkg->invitee, pkg->len, proto_recv_btl_inv, 0, 0);
		return send_pkg_to_client(s_online_info_[olid - 1].fdsess, pkg, pkg->len);
	}
	pack_h(s_pkg_, 0, idx);
	init_proto_head(s_pkg_, 0, idx, proto_send_btl_inv, 0, pkg->seq);
	return send_pkg_to_client(fdsess, s_pkg_, idx); //response to invitor
}

/**
  * @brief common interface to kick user from the corresponding home online
  * @param kickee user to be kicked
  * @param reason reason of kicking the user
  */
void Online::kick_usr_from_home(userid_t kickee, const uint32_t reason)
{
	int idx = sizeof(svr_proto_t);
	pack_h(s_pkg_, kickee, idx);
	pack_h(s_pkg_, reason, idx);
	init_proto_head(s_pkg_, 0, idx, proto_kick_user_offline, 0, 0);

	for (uint32_t i = 0; i < s_home_online_cnt; i++) 
	{
		if (s_home_online_info_[i].online_id != 0) 
		{
			send_pkg_to_client(s_home_online_info_[i].fdsess, s_pkg_, idx);
		}
	}
}

/**
  * @brief clear the online server infomation 
  * @param pkg package buffer 
  * @param bodylen package body length
  * @param fdsess fd related information of sender 
  * @return 0 on success, -1 on error
  */
void Online::clear_online_info(int fd)
{
	uint16_t online_id = s_fd_map_[fd];

	/* clear online fd */
	s_fd_map_.erase(fd);

	if ((online_id < home_online_base) && (online_id != 0))  // regular online
	{
		int idx = online_id - 1;

		DEBUG_LOG("ONLINE CONNECTION CLOSED\t[id=%u %u ip=%.16s]",
					online_id, s_online_info_[idx].online_id, s_online_info_[idx].online_ip);

				/* clear the keepalive timer */
		s_keepalive_tmr_[idx].tmr = 0;
		REMOVE_TIMERS(&(s_keepalive_tmr_[idx]));


		/* clear the player's list within the server and online_id info */
		player_info_t* pinfo;
		list_head_t *cur, *next;
		list_for_each_safe(cur, next, &(s_online_info_[idx].player_list))
		{
			pinfo = list_entry(cur, player_info_t, online_hook);

            //ERROR_LOG("trace online close ------> [%u] [%u]", pinfo->usrid, pinfo->online_id);
            del_online_player_interface(pinfo->usrid);

			list_del_init(&(pinfo->online_hook));

			// kick user off its home online
			kick_usr_from_home(pinfo->usrid, reason_system_error);
			pinfo->online_id = 0;

		}

		INIT_LIST_HEAD(&(s_online_info_[idx].player_list));

		//reset online info 
		memset(&(s_online_info_[idx]), 0, sizeof(s_online_info_[0]));
		
		// update max online id
		if (online_id == s_max_online_id_) 
		{
			uint32_t i = s_max_online_id_ - 1;
			while (i && (s_online_info_[i - 1].online_id == 0)) 
			{
				--i;
			}
			s_max_online_id_ = i;
		}
	} 
	else if (online_id >= home_online_base)  // home online 
	{
		uint32_t idx = online_id - home_online_base;

		DEBUG_LOG("HOME ONLINE CONNECTION CLOSED\t[id=%u %u]", online_id, s_home_online_info_[idx].online_id);
		memset(&(s_home_online_info_[idx]), 0, sizeof(s_home_online_info_[0]));

		// update max home online id
		if (idx == s_home_online_cnt - 1) 
		{
			uint32_t i = s_home_online_cnt - 1;
			while (i && (s_home_online_info_[i - 1].online_id == 0)) 
			{
				--i;
			}
			s_home_online_cnt = i;
		}

	}
}

int Online::notify_player_new_mail(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL_GE(bodylen, sizeof(new_mail_notify_t));
    new_mail_notify_t* pMail = reinterpret_cast<new_mail_notify_t*>(pkg->body);
	int idx  = sizeof(svr_proto_t);
	pack_h(s_pkg_,  pMail->mail_id, idx);
	pack_h(s_pkg_,  pMail->mail_time, idx);
	pack_h(s_pkg_,  pMail->mail_state, idx);
	pack_h(s_pkg_,  pMail->mail_templet, idx);
	pack_h(s_pkg_,  pMail->mail_type, idx);
	pack_h(s_pkg_,  pMail->sender_id, idx);
	pack_h(s_pkg_,  pMail->sender_role_tm, idx);
	pack(s_pkg_, pMail->mail_title, sizeof(pMail->mail_title), idx);
	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, pkg->seq);
	
	uint16_t  online_id = get_user_online_id(pkg->sender_id);
	if(online_id){
		return send_pkg_to_client(s_online_info_[online_id - 1].fdsess, s_pkg_, idx);
	}
	return 0;
}

//---------------------------------------------------------
//  rand user by lv
//---------------------------------------------------------
enum reward_rank{
    first_rank = 1,
    second_rank,
    third_rank,
    fourth_rank
};
void select_player_fun()
{

}
//----------------------------------------------------------
// For Login Server
//----------------------------------------------------------
/**
  * @brief get recommended servers list
  * @param pkg package buffer
  * @param bodylen package body length
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int Online::get_recommeded_svrlist(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL_GE(pkg->len, sizeof(svr_proto_t) + 11); // DomainID(2)+OM_Server(1)+FriendCnt(4)
	uint16_t req_domain = *reinterpret_cast<uint16_t*>(pkg->body);
	uint32_t lv = *reinterpret_cast<uint32_t*>(pkg->body + 3);
	friend_list_t* friendlist = reinterpret_cast<friend_list_t*>(pkg->body + 7);// FriendCnt + .....
	CHECK_VAL_GE(pkg->len, sizeof(svr_proto_t) + 7 + friendlist->cnt * sizeof(userid_t));
	DEBUG_LOG("get recommended svrlist:lv[%u] req_domain[%u] get[len=%u,frdcnt=%u]", lv, req_domain, pkg->len, friendlist->cnt);
	/*=============================================
	  * return 11 server at most:
	  *   1. One previous logined server;
	  *   2. Servers in which the user's friends are located;
	  *   3. Hot servers;
	  *   4. Two chat-forbidden servers.
	  *=============================================*/


	// previous logined online server id
	uint32_t prev_online_id = get_user_prev_online_id(pkg->sender_id);

	olid_set_t olid_frd_set; // online server id set in which the user's friends are located
	olid_set_t olid_remain_set; // remaining online server id set
	olid_set_t olid_nochat_set; // nochat online server id set

	olid_set_t olid_lv_set; 


#ifndef VERSION_KAIXIN
/*
	if (lv <= 25 && lv > 0) {
		if (req_domain == 0) {
			do_get_lv_recommended_online_list(pkg->sender_id, lv, friendlist, 0, prev_online_id, olid_frd_set, olid_lv_set, olid_remain_set, olid_nochat_set);
			if ((olid_frd_set.size() == 0) && (olid_remain_set.size() == 0)) {
				do_get_lv_recommended_online_list(pkg->sender_id, lv, friendlist, 1, prev_online_id, olid_frd_set, olid_lv_set, olid_remain_set, olid_nochat_set);
			}
		} else {
			do_get_lv_recommended_online_list(pkg->sender_id, lv, friendlist, 1, prev_online_id, olid_frd_set, olid_lv_set, olid_remain_set, olid_nochat_set);
			if ((olid_frd_set.size() == 0) && (olid_remain_set.size() == 0)) {
				do_get_lv_recommended_online_list(pkg->sender_id, lv, friendlist, 0, prev_online_id, olid_frd_set, olid_lv_set, olid_remain_set, olid_nochat_set);
			}
		}
	} else {
	*/
		if (req_domain == 0) {
			do_get_recommended_online_list(pkg->sender_id, friendlist, 0, prev_online_id, olid_frd_set, olid_remain_set, olid_nochat_set);
			if ((olid_frd_set.size() == 0) && (olid_remain_set.size() == 0)) {
				do_get_recommended_online_list(pkg->sender_id, friendlist, 1, prev_online_id, olid_frd_set, olid_remain_set, olid_nochat_set);
			}
		} else {
			do_get_recommended_online_list(pkg->sender_id, friendlist, 1, prev_online_id, olid_frd_set, olid_remain_set, olid_nochat_set);
			if ((olid_frd_set.size() == 0) && (olid_remain_set.size() == 0)) {
				do_get_recommended_online_list(pkg->sender_id, friendlist, 0, prev_online_id, olid_frd_set, olid_remain_set, olid_nochat_set);
			}
		}
//	}
#else
	do_get_recommended_online_list(pkg->sender_id, friendlist, 2, prev_online_id, olid_frd_set, olid_remain_set, olid_nochat_set);
	if ((olid_frd_set.size() == 0) && (olid_remain_set.size() == 0)) {
		do_get_recommended_online_list(pkg->sender_id, friendlist, 2, prev_online_id, olid_frd_set, olid_remain_set, olid_nochat_set);
	}

#endif
	// pack server list and send it to login server
	int idx = sizeof(svr_proto_t);
	int cnt_idx = 0;
	pack_h(s_pkg_, s_max_online_id_, idx);
	pack_h(s_pkg_, static_cast<uint16_t>(0), idx); // domain id
	DEBUG_LOG("get recommended svrlist:get[max_id=%u]",s_max_online_id_);
	cnt_idx = idx; // remember the index to pack online_cnt
	idx += 4; // reserve space for online_cnt

	uint32_t online_cnt = 1; // at least the previous logined online server id is returned
	// pack previous logined online server id
	uint32_t prev_online_idx = prev_online_id - 1;
	if (prev_online_id && (s_online_info_[prev_online_idx].online_id == prev_online_id)) 
	{
		pack_h(s_pkg_, prev_online_id, idx);		
		pack_h(s_pkg_, s_online_info_[prev_online_idx].user_num, idx);
		pack(s_pkg_, s_online_info_[prev_online_idx].online_ip, sizeof(s_online_info_[0].online_ip), idx);
		pack_h(s_pkg_, s_online_info_[prev_online_idx].online_port, idx);
		pack_h(s_pkg_, static_cast<uint32_t>(s_online_info_[prev_online_idx].domain), idx);
		pack_h(s_pkg_, 0, idx); // no friends
		DEBUG_LOG("get recommended svrlist:prev_olineid:[id=%u]",prev_online_id);
	} 
	else 
	{ // previous logined online server not exist
		pack_h(s_pkg_, 0, idx);
		DEBUG_LOG("get recommended svrlist:0:[id=%u]",prev_online_id);
		idx += 30;
	}

	// pack online servers in which the user's friends are located
	for (olid_set_t::iterator it = olid_frd_set.begin(); it != olid_frd_set.end(); ++it) 
	{
		int index = *it - 1;
		pack_h(s_pkg_, s_online_info_[index].online_id, idx);
		pack_h(s_pkg_, s_online_info_[index].user_num, idx);
		pack(s_pkg_, s_online_info_[index].online_ip, sizeof(s_online_info_[0].online_ip), idx);
		pack_h(s_pkg_, s_online_info_[index].online_port, idx);
		pack_h(s_pkg_, static_cast<uint32_t>(s_online_info_[index].domain), idx);
		pack_h(s_pkg_, 1, idx); // has friends
		DEBUG_LOG("get recommended svrlist:olineid:[id=%u]",s_online_info_[index].online_id);
		++online_cnt;
	}

	// pack the lv match online servers
	for (olid_set_t::iterator it = olid_lv_set.begin(); it != olid_lv_set.end(); ++it) 
	{
		int index = *it - 1;
		pack_h(s_pkg_, s_online_info_[index].online_id, idx);
		pack_h(s_pkg_, s_online_info_[index].user_num, idx);
		pack(s_pkg_, s_online_info_[index].online_ip, sizeof(s_online_info_[0].online_ip), idx);
		pack_h(s_pkg_, s_online_info_[index].online_port, idx);
		pack_h(s_pkg_, static_cast<uint32_t>(s_online_info_[index].domain), idx);
		pack_h(s_pkg_, 0, idx); // no friends
		DEBUG_LOG("get recommended svrlist:olineid1:[id=%u]",s_online_info_[index].online_id);
		++online_cnt;
	}


	// pack the remaining online servers
	for (olid_set_t::iterator it = olid_remain_set.begin(); it != olid_remain_set.end(); ++it) 
	{
		int index = *it - 1;
		pack_h(s_pkg_, s_online_info_[index].online_id, idx);
		pack_h(s_pkg_, s_online_info_[index].user_num, idx);
		pack(s_pkg_, s_online_info_[index].online_ip, sizeof(s_online_info_[0].online_ip), idx);
		pack_h(s_pkg_, s_online_info_[index].online_port, idx);
		pack_h(s_pkg_, static_cast<uint32_t>(s_online_info_[index].domain), idx);
		pack_h(s_pkg_, 0, idx); // no friends
		DEBUG_LOG("get recommended svrlist:olineid2:[id=%u]",s_online_info_[index].online_id);
		++online_cnt;
	}

	// pack two chat-forbidden online servers
	for (olid_set_t::iterator it = olid_nochat_set.begin(); it != olid_nochat_set.end(); ++it) 
	{
		int index = *it - 1;
		pack_h(s_pkg_, s_online_info_[index].online_id, idx);
		pack_h(s_pkg_, s_online_info_[index].user_num, idx);
		pack(s_pkg_, s_online_info_[index].online_ip, sizeof(s_online_info_[0].online_ip), idx);
		pack_h(s_pkg_, s_online_info_[index].online_port, idx);
		pack_h(s_pkg_, static_cast<uint32_t>(s_online_info_[index].domain), idx);
		pack_h(s_pkg_, 0, idx); // no friends
		DEBUG_LOG("get recommended svrlist:olineid3:[id=%u]",s_online_info_[index].online_id);
		++online_cnt;
	}

	pack_h(s_pkg_, online_cnt, cnt_idx);// fill up online_cnt 
	DEBUG_LOG("get recommended svrlist:get[online_cnt=%u]",online_cnt);
	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, pkg->seq);

	return send_pkg_to_client(fdsess, s_pkg_, idx);
}

/**
  * @brief get server list in the given range
  * @param pkg package buffer
  * @param bodylen package body length
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int Online::get_ranged_svrlist(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct svrlist_req_t 
	{
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
			|| (svrlist->start_id > svrlist->end_id) || ((svrlist->end_id - svrlist->start_id) > 100)) 
	{
		ERROR_LOG("invalid range: start=%u end=%u", svrlist->start_id, svrlist->end_id);
		return -1;
	}

	// pack server list
	int idx = sizeof(svr_proto_t), cnt_idx;
	pack_h(s_pkg_, static_cast<uint32_t>(0), idx);//max_online_id
	pack_h(s_pkg_, static_cast<uint16_t>(0), idx);//domain_id
	cnt_idx = idx; // remember the index to pack online_cnt
	idx += 4; // reserve space for online_cnt
	uint32_t online_cnt = 0;	
	for (uint32_t i = svrlist->start_id - 1; i != svrlist->end_id; ++i) 
	{
		if (s_online_info_[i].online_id) 
		{
			pack_h(s_pkg_, s_online_info_[i].online_id, idx);
			pack_h(s_pkg_, s_online_info_[i].user_num, idx);
			pack(s_pkg_, s_online_info_[i].online_ip, sizeof(s_online_info_[i].online_ip), idx);
			pack_h(s_pkg_, s_online_info_[i].online_port, idx);
			pack_h(s_pkg_, static_cast<uint32_t>(s_online_info_[i].domain), idx);
			pack_h(s_pkg_, static_cast<uint32_t>(0), idx); // friends count on the online server, set it 0
			TRACE_LOG("rangelist:%u %u",s_online_info_[i].online_id, s_online_info_[i].domain);
			++online_cnt;
		}
	}
	pack_h(s_pkg_, online_cnt, cnt_idx);
	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, pkg->seq);
	return send_pkg_to_client(fdsess, s_pkg_, idx);
}

/**
  * @brief neccessary finilization
  */
void Online::fini()
{
	free(s_usr_onoff_);
	pre_olid_file.seekp(0, fstream::beg);
	pre_olid_file.write(reinterpret_cast<char*>(s_prev_logined_online_id_), sizeof(*s_prev_logined_online_id_) * s_max_uid_);
	DEBUG_LOG("over");
	pre_olid_file.close();
}


//-------------------------------------------------------
// rand reward
//-------------------------------------------------------

int Online::send_reward_msg_to_player(uint32_t recvid, uint32_t reward_id)
{
    player_info_t* player = get_user_info(recvid);
    if (player) //on
    {
        DEBUG_LOG("send reward olid[%u] uid[%u %u] rid[%u]", player->online_id, recvid, player->role_tm, reward_id);
        if (player->online_id == 0) return 0;

        int idx = sizeof(svr_proto_t);
        pack_h(s_pkg_, player->role_tm, idx);
        pack_h(s_pkg_, reward_id, idx);
        init_proto_head(s_pkg_, recvid, idx, proto_rand_reward_notice, 0, 0);
        send_pkg_to_client(s_online_info_[player->online_id - 1].fdsess, s_pkg_, idx);
    } 

    return 0;
}

void do_random_reward_logic_ex()
{

}

void do_random_reward_logic(uint32_t hour)
{
    //ERROR_LOG(" ------- TIME handler ------ ");
    uint32_t odd = 0;
#ifdef VERSION_TAIWAN
    static uint32_t player_num[] = {500, 300, 300, 50, 300, 5, 30};
#else
    static uint32_t player_num[] = {5, 100, 100, 50, 200, 5, 20};
#endif

    //鼓励礼包
    for (uint32_t i = 0; i < player_num[0] && oluser_arr[0].player_num != 0; i++) { //25
        odd = rand() % oluser_arr[0].player_num;
        Online::send_reward_msg_to_player(oluser_arr[0].ol_user_[odd], 3);
    }

    /*
    //鼓励礼包
    for (uint32_t i = 0; i < player_num[0] && oluser_arr[0].player_num != 0; i++) { //50
        odd = rand() % oluser_arr[0].player_num;
        Online::send_reward_msg_to_player(oluser_arr[0].ol_user_[odd], 1);
    }

    //gold dragon pearl
    for (uint32_t i = 0; i < player_num[1] && oluser_arr[0].player_num != 0; i++) { //50
        odd = rand() % oluser_arr[0].player_num;
        Online::send_reward_msg_to_player(oluser_arr[0].ol_user_[odd], 5);
    }
    //fire dragon pearl
    for (uint32_t i = 0; i < player_num[2] && oluser_arr[0].player_num != 0; i++) { //50
        odd = rand() % oluser_arr[0].player_num;
        Online::send_reward_msg_to_player(oluser_arr[0].ol_user_[odd], 6);
    }
    //dragon scales
    for (uint32_t i = 0; i < player_num[3] && oluser_arr[0].player_num != 0; i++) { //25
        odd = rand() % oluser_arr[0].player_num;
        Online::send_reward_msg_to_player(oluser_arr[0].ol_user_[odd], 7);
    }
    
    //进阶礼包
    for (uint32_t i = 0; i < player_num[4] && oluser_arr[1].player_num != 0; i++) { //100
        odd = rand() % oluser_arr[1].player_num;
        Online::send_reward_msg_to_player(oluser_arr[1].ol_user_[odd], 2);
    }
    
    
    if (hour == 14) {
        //冰雪大礼包 
        for (uint32_t i = 0; i < player_num[5] && oluser_arr[2].player_num != 0; i++) { //5
            odd = rand() % oluser_arr[2].player_num;
            Online::send_reward_msg_to_player(oluser_arr[2].ol_user_[odd], 3);
            char msg[500] = {0};
#ifdef VERSION_TAIWAN
            snprintf(msg, 500, "(%d)%s", oluser_arr[2].ol_user_[odd], "幸運的獲得了冰雪女王的禮物:冰雪至尊大禮包--請至陸半仙處領取。");
#else
            snprintf(msg, 500, "(%d)%s", oluser_arr[2].ol_user_[odd], "幸运的获得了冰雪女王的礼物:冰雪至尊大礼包--请至陆半仙处领取。");
#endif
            Online::mcast_official_interface(msg);

        }
    }
    //超灵大礼包
    for (uint32_t i = 0; i < player_num[6] && oluser_arr[3].player_num != 0; i++) { //10
        odd = rand() % oluser_arr[3].player_num;
        Online::send_reward_msg_to_player(oluser_arr[3].ol_user_[odd], 4);
    }
    */
}

void process_random_event()
{
    time_t cur_time = get_now_tv()->tv_sec;
    static uint32_t last_update_tm = 0;
    if (last_update_tm == 0) {
        last_update_tm = cur_time;
    }

    if (last_update_tm == cur_time || cur_time < last_update_tm + 60) {
        return;
    } else {
        last_update_tm = cur_time;
    }

    struct tm _tm;
    localtime_r(&cur_time, &_tm);
    /*
    if (_tm.tm_min % 3 == 0 && _tm.tm_sec == 0){
        char msg[500] = "冰雪狂欢节送礼包活动即将开始，米币、伏魔点、功夫券、百万功夫豆等你拿!";
        Online::mcast_official_interface(msg);
    }*/
    //ERROR_LOG("time -----> [%u %u]", _tm.tm_mon, _tm.tm_mday);
    if ( _tm.tm_mon == 0 && (_tm.tm_mday >= 22 && _tm.tm_mday <= 25) ) {
        if ( (_tm.tm_hour >= 11 && _tm.tm_hour < 21) && _tm.tm_min == 0 )
            do_random_reward_logic(_tm.tm_hour);
    }
    /*
	TRACE_LOG("%u %u %u %u", _tm.tm_wday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
    if (_tm.tm_wday == 1 || _tm.tm_wday == 0 || _tm.tm_wday == 2) {
        if ( (_tm.tm_hour == 13 && _tm.tm_min == 55 && _tm.tm_sec == 0)
            || (_tm.tm_hour == 18 && _tm.tm_min == 55 && _tm.tm_sec == 0) ) {
#ifdef VERSION_TAIWAN
            char msg[500] = "冰雪狂歡節送禮包活動即將開始，伏魔點、功夫券、百萬功夫豆等你拿!";
#else
            char msg[500] = "冰雪狂欢节送礼包活动即将开始，米币、伏魔点、功夫券、百万功夫豆等你拿!";
#endif
            Online::mcast_official_interface(msg);
        }
        if (_tm.tm_hour == 14 || _tm.tm_hour == 19) {
            if (_tm.tm_min < 41 && _tm.tm_min % 10 == 0 && _tm.tm_sec == 0) {
                do_random_reward_logic( _tm.tm_hour );
            }
            //if (_tm.tm_min < 41 && _tm.tm_min % 10 == 0 && _tm.tm_sec < 5) {
            //    do_random_reward_logic_ex();
            //}
        }
    }*/
}

void add_single_oluser_arr(uint32_t arr_idx, uint32_t uid)
{
    if (oluser_arr[arr_idx].player_num < oluser_guess_num[arr_idx]) {
        oluser_arr[arr_idx].ol_user_[oluser_arr[arr_idx].player_num] = uid;
    } else {
        oluser_arr[arr_idx].ol_user_.push_back(uid);
    }

    oluser_arr[arr_idx].player_num++;
}

void del_single_oluser_arr(uint32_t arr_idx, uint32_t vec_idx)
{
    player_ol_t *p_temp = NULL;

    oluser_arr[arr_idx].player_num--;
    if (oluser_arr[arr_idx].player_num > 0) {
        uint32_t last_uid = oluser_arr[arr_idx].ol_user_[oluser_arr[arr_idx].player_num];
        p_temp = reinterpret_cast<player_ol_t*>( g_hash_table_lookup(m_all_players_, &last_uid) );
        if (p_temp) {
            p_temp->index[arr_idx]   =   vec_idx;
            oluser_arr[arr_idx].ol_user_[vec_idx - 1] = last_uid;
        } else {
            ERROR_LOG("impossibility error [%u %u]", last_uid, oluser_arr[arr_idx].player_num);
        }
    }

}

void init_global_data()
{
    //ERROR_LOG("init ------- global data");
    for (uint32_t i = 0; i < 1; i++) {
        oluser_arr[i].player_num = 0;
        oluser_arr[i].ol_user_.resize(oluser_guess_num[i], 0);
    }
    m_all_players_ = g_hash_table_new_full(g_int_hash, g_int_equal, 0, free_ol_player);
}

void fini_global_data()
{
    //ERROR_LOG("fini ------- global data");
    for (uint32_t i = 0; i < 1; i++) {
        oluser_arr[i].player_num = 0;
        oluser_arr[i].ol_user_.clear();
    }
    g_hash_table_destroy(m_all_players_);
}

void free_ol_player(void *p_ol)
{
    //ERROR_LOG("free ol player --------> ");
    g_slice_free1(c_ol_player_size, p_ol);
}

//------------------------------------------------------
