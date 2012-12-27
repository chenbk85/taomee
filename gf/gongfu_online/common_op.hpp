/**
 *============================================================
 *  @file      common_op.hpp
 *  @brief    common operations such as walk, talk...
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_COMMON_OP_HPP_
#define KF_COMMON_OP_HPP_

#include <libtaomee++/inet/pdumanip.hpp>

#include "cli_proto.hpp"
#include "fwd_decl.hpp"
#include "cachesvr.hpp"
extern "C" {
#include <libtaomee/dataformatter/bin_str.h>
}

#pragma pack(1)

struct friend_login_t
{
	uint32_t userid;
	uint8_t  usernick[16];
	uint32_t friend_cnt;
	uint32_t friends[];
};

struct hunter_top_info
{
	uint32_t order;
    uint32_t userid;
    uint32_t roletm;
    uint32_t role_type;
    uint32_t layer;
    uint32_t interval;
    uint32_t userlv;
	uint8_t  nick[16];
};
struct hunter_top_header_t
{
    uint32_t cnt;
    uint8_t data[];
};

struct db_hunter_info_t
{
    uint32_t    layer;
    uint32_t    interval;
    uint32_t    order;
};


struct db_hunter_top_header_t
{
    uint32_t    layer;
    uint32_t    interval;
    uint32_t    order;
    uint32_t    cnt;
	uint8_t data[];
};


struct db_hunter_top_info
{
    uint32_t    userid;
    uint32_t    roletm;
    uint32_t    role_type;
    char        nick[max_nick_size];
    uint32_t    layer;
    uint32_t    interval;
    uint32_t    userlv;
};

struct trade_mcast_t {
	char    nick[max_nick_size];
	uint32_t front_id;
	char   info[max_trade_mcast_size];
};

struct player_mcast_t {
    uint8_t type; //1:player_market 2:player_world
	uint8_t nick[max_nick_size];
	uint32_t front_id;
	uint8_t info[max_trade_mcast_size];
};


struct official_mcast_t {
    uint32_t    index;
    uint32_t    type;//1:realtime 2:loop
	uint32_t front_id;
	char   info[max_official_mcast_size];
};


struct db_power_user_info_t {
    uint32_t    power_user;
    uint32_t    skill_point;
    uint32_t    item_id_1;
    uint32_t    item_id_2;
};

struct db_set_player_fate_t {
    uint32_t    money;
    uint32_t    p_exp;
    uint32_t    m_exp;
};

#pragma pack()

enum player_mcast_type {
    player_market = 1,
    player_world  = 2,
};
//----------------------------------------------------------------

inline int pack_stand_pkg(void* buf, player_t* p, userid_t uid, uint32_t x, uint32_t y, uint8_t dir)
{
	// pack stand info and send it to other players
	int i = sizeof(cli_proto_t);
	taomee::pack(buf, x, i);
	taomee::pack(buf, y, i);
	taomee::pack(buf, dir, i);
	TRACE_LOG("%u %u %u ", p->id, x, y);
	init_cli_proto_head_full(buf, uid, p->seqno, cli_proto_stand, i, 0);
	return i;
}

inline int get_date(time_t t  ) 
{
	struct tm tm_tmp;
	localtime_r(&t, &tm_tmp) ;
	return (tm_tmp.tm_year+1900)*10000+(tm_tmp.tm_mon+1)*100+tm_tmp.tm_mday;
}

inline uint32_t get_today_begin_second(uint32_t sec)
{
    if (sec == 0) sec = get_now_tv()->tv_sec;

    time_t t = sec;
    struct tm tm_tmp;
    localtime_r(&t, &tm_tmp);
    
    tm_tmp.tm_hour = 0;
    tm_tmp.tm_min  = 0;
    tm_tmp.tm_sec  = 0;

    return (uint32_t)mktime( &(tm_tmp) );
}

inline uint32_t get_today_end_second(uint32_t sec)
{
    if (sec == 0) sec = get_now_tv()->tv_sec + 86400;

    time_t t = sec;
    struct tm tm_tmp;
    localtime_r(&t, &tm_tmp);
    
    tm_tmp.tm_hour = 0;
    tm_tmp.tm_min  = 0;
    tm_tmp.tm_sec  = 0;

    return (uint32_t)mktime( &(tm_tmp) );
}

inline uint32_t get_utc_second(int year, int mon, int day, int hour, int minute, int sec)
{
    struct tm _tm = {0};
    _tm.tm_year = year - 1900;
    _tm.tm_mon  = mon  - 1;
    _tm.tm_mday = day;
    _tm.tm_hour = hour;
    _tm.tm_min  = minute;
    _tm.tm_sec  = sec;

    return (uint32_t)mktime( &(_tm) );
}
/**
 * @brief 
 * @Param Input: 2011/08/21 00:12:03
 */
inline uint32_t get_utc_second_ex(const char * input_date)
{
    if (strlen(input_date) != 19) {
        return 0;
    }

    struct tm _tm = {0};
    sscanf( input_date, "%d/%d/%d %d:%d:%d", 
        &(_tm.tm_year), &(_tm.tm_mon), &(_tm.tm_mday),
        &(_tm.tm_hour), &(_tm.tm_min), &(_tm.tm_sec) );

    TRACE_LOG("DATE:[ %d/%d/%d %d:%d:%d ]", _tm.tm_year, _tm.tm_mon, _tm.tm_mday,
        _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
    _tm.tm_year -= 1900;
    _tm.tm_mon  -= 1;

    return (uint32_t)mktime( &(_tm) );
}

//----------------------------------------------------------------

/**
  * @brief for stat log
  */
int stat_log_cmd(player_t* p, uint8_t* body, uint32_t bodylen);


/**
  * @brief player walks
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int walk_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief player moves use keyboard
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int walk_keyboard_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief player stands
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int stand_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief player jump
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int jump_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief player add friend
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int add_friend_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief reply to adding friend request 
  * @param p the player who reply the request
  * @param body protocol body
  * @param bodylen lengh of the protocol body
  * @return 0 on success, -1 on error
  */
int reply_add_friend_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief player del friend
  * @param p the player who decides to delete friends
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int del_friend_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief player add friend to blacklist
  * @param p the player who decides to add friend to blacklist
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int add_blacklist_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief player get blacklist
  * @param p the player who decides to get blacklist
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_blacklist_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief check users onoff status
  * @param p the player who send the checking request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int check_users_onoff_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int get_encryption_session_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief statistics user questionnaire
  * @param p the player who send the checking request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_questionnaire_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief statistics user info for market need
  * @param p the player who send the checking request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int market_user_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief statistics user questionnaire
  * @param p the player who send the checking request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_hunter_top_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief statistics user questionnaire
  * @param p the player who send the checking request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_hunter_self_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief finger guessing
 */
int  finger_guessing_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
 * @brief get finger guessing data
 */
int get_finger_guessing_data_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int player_bet_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int player_fatch_bet_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

//-------------------------------------------------------------------------------
// Callbacks: process package from db and send package to client
//-------------------------------------------------------------------------------
/**
  * @brief callback for handling friend adding action returned from dbproxy
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */  
int db_get_hunter_top_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling friend adding action returned from dbproxy
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */  
int db_get_hunter_self_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);


/**
  * @brief callback for handling friend adding action returned from dbproxy
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */  
int db_add_friend_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling friend deleting returned from dbproxy
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */  
int db_del_friend_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling adding blacklist returned from dbproxy
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */
int db_add_blacklist_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling deleting player in blacklist returned from dbproxy
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */
int db_del_blacklist_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling getting blacklist returned from dbproxy
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */
int db_get_blacklist_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int db_add_session_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

void friend_login_op(const void *body, uint32_t bodylen);

void update_limit_data(const void *body, uint32_t bodylen);

/**
 * @brief Interface: notify player mcast information (loop model)
 */
int notify_official_mcast_info(uint32_t userid, uint32_t mcast_cmd, char* info);


/**
 * @brief Interface: notify player mcast information
 */
int notify_player_mcast_info(player_t* p, uint32_t mcast_cmd, player_mcast_t* info);

void send_trade_mcast_info(uint8_t* buf, uint32_t len);
int mcast_trade(const void * body, uint32_t bodylen);
int world_public_notice(const void * body, uint32_t bodylen);
bool if_version_kaixin();
bool is_btl_time_limited(player_t* p);
int load_char_content(xmlNodePtr cur);

int notify_player_play_fireworks(player_t* p);
//added by cws 0613
int notify_rebuild_gf_fireworks(uint32_t type);
void process_active_broadcast();

//---------------------------------------------------------------
// little function 
//---------------------------------------------------------------

bool can_get_summon_item(player_t* p);

int daily_hatch_summon_cmd(player_t* p, uint8_t* body, uint32_t bodylen);


int get_daily_hatch_summon_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int player_city_team(uint32_t uid, uint32_t role_time);

int get_city_win_team();


int up_power_user_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
  * @brief callback for handling set_power_user_info returned from dbproxy
  * @param p the requester
  * @param id id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */
int db_set_power_user_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int list_simple_role_info_cmd(player_t *p, uint8_t * body, uint32_t bodylen);

int db_list_simple_role_info_callback(player_t *p, userid_t id, void * body, uint32_t bodylen, uint32_t ret);
	
int generate_magic_number_cmd(player_t * p, uint8_t * body, uint32_t bodylen);
int bind_magic_number_cmd(player_t * p, uint8_t * body, uint32_t bodylen);
int list_magic_invite_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int cache_generate_magic_number_callback(player_t *p, cachesvr_proto_t * data);
int cache_bind_magic_number_callback(player_t * p, cachesvr_proto_t * data);
int cache_get_magic_number_creator_callback(player_t * p, cachesvr_proto_t * data);
int get_invitee_player(player_t * p);



void send_shengdan_broad_message();
void send_50_lamp_win_message(player_t* p);

bool is_in_right_hour(uint32_t begin_hour, uint32_t end_hour);

int player_zhanbo_fate_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int player_select_fate_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int db_set_player_fate_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

bool check_login_box(player_t* p, uint32_t index, uint32_t lag);

int notify_dirty_words(player_t* p, char* msg, uint32_t msg_len);

bool is_date(uint32_t mon_day);
#endif
