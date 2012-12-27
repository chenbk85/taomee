/**
 *=====================================================================
 *  @file     restriction.hpp
 *  @brief    all about player's restrictions of tasks, namely, 
 *            number of times to carry out tasks in one day or one week
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *=====================================================================
 */

#ifndef KF_RESTRICTION_HPP_
#define KF_RESTRICTION_HPP_

#include "player.hpp"

struct swap_item_rsp_t;


/*! max rewards count for a single periodical action */
const uint32_t c_max_reward_count = 40;
/*! max costs count for a single periodical action */
const uint32_t c_max_cost_count = 20;

/*! base type value for weekly action */
//const uint32_t c_weekly_action_type_base = 100000;
/*! base type value for monthly action */
//const uint32_t c_monthly_action_type_base = 200000;
/*! base type value for special wild */
//const uint32_t c_swild_type_base = 10000;

enum {
	daily_active_for_day = 0,
	daily_active_for_week,
	daily_active_for_month,
	daily_active_for_year,
	daily_active_for_ever,
};

enum {
	fumo_top_cannot_reward = 0,
	fumo_top_can_reward	   = 1,
	fumo_top_have_reward   = 2,
};
struct cost_reward_t {
	/*! cost item  */
	uint32_t give_type;
    /*! distinct role if attire */
    uint32_t role_type;
	/*! cost item  */
	uint32_t give_id;
	/*! cost amount */
	uint32_t count;
	/*! odds*/
	uint32_t odds;
} ;


struct restriction_t {
	/*! restriction id */
	uint32_t	id;
	/*! restriction name */
	char		name[MAX_MAIL_TITLE_LEN];
	/*! restriction type */
	uint32_t	type;
	/*! user type 0 for role, 1 for userid*/
	uint8_t		userflg;
	uint32_t    dbpos;
	/*! restriction flag: 0 for daily restriction and 1 for weekly one */
	uint32_t	restr_flag;
	/*! restriction top limit in one day or week */
	uint32_t	toplimit;
    /*! limit use level */
    uint32_t    uselv[2];
	/*! restriction vip limit in one day or week */
	uint32_t	vip;
    /*! year_vip limit flag */
	uint8_t	    year_vip;
	/*! time limit range */
	uint32_t	tm_range[2];
	/*! count of costs */
	uint32_t	cost_count;
	/*! costs */
	cost_reward_t	costs[c_max_cost_count];
	/*! count of rewards */
	uint32_t	rew_count;
	/*! is it have odds*/
	uint32_t odds_flg;
	/*! rewards */
	cost_reward_t	rewards[c_max_reward_count];
};

#pragma pack(1)

/**
 * @brief an restriction's info returned from db
 */
struct db_res_elem_t {
	uint32_t	id;
	uint32_t	cnt;
};

/**
 * @brief restriction's info returned from db
 */
struct query_res_list_rsp_t {
	uint32_t		fumo_reward_flg;
	uint32_t		cnt;
	db_res_elem_t	res[];
};

/**
 * @brief restriction's info returned from db
 */
struct db_add_action_rsp_t {
	uint32_t		id;
};

struct fanfan_data_t {
	uint32_t item_id;
	uint32_t condition_daily_cnt_for_out_put;
	uint32_t out_put_cnt;
	uint32_t max_out_put_cnt;
};

typedef std::map<uint32_t, fanfan_data_t> FanfanDataMap;


class fanfan_mrg {
	public:
		fanfan_mrg()
		{
			fanfan_data_t fanfan_elem;
			fanfan_elem.item_id = 1740015;
			fanfan_elem.condition_daily_cnt_for_out_put = 300;//1;//
			fanfan_elem.max_out_put_cnt = 1;
			fanfan_elem.out_put_cnt = 0;
			out_put_condition.insert(FanfanDataMap::value_type(fanfan_elem.item_id, fanfan_elem));

			fanfan_elem.item_id = 1303020;
			fanfan_elem.condition_daily_cnt_for_out_put = 500;//1;//
			fanfan_elem.max_out_put_cnt = 1;
			fanfan_elem.out_put_cnt = 0;
			out_put_condition.insert(FanfanDataMap::value_type(fanfan_elem.item_id, fanfan_elem));

			fanfan_elem.item_id = 1740023;
			fanfan_elem.condition_daily_cnt_for_out_put = 1000;//1;//
			fanfan_elem.max_out_put_cnt = 1;
			fanfan_elem.out_put_cnt = 0;
			out_put_condition.insert(FanfanDataMap::value_type(fanfan_elem.item_id, fanfan_elem));

			fanfan_elem.item_id = 1740024;
			fanfan_elem.condition_daily_cnt_for_out_put = 2000;//2;//
			fanfan_elem.max_out_put_cnt = 1;
			fanfan_elem.out_put_cnt = 0;
			out_put_condition.insert(FanfanDataMap::value_type(fanfan_elem.item_id, fanfan_elem));

			fanfan_elem.item_id = 1740014;
			fanfan_elem.condition_daily_cnt_for_out_put = 2000;//2;//
			fanfan_elem.max_out_put_cnt = 1;
			fanfan_elem.out_put_cnt = 0;
			out_put_condition.insert(FanfanDataMap::value_type(fanfan_elem.item_id, fanfan_elem));
			
		}
	private:
		FanfanDataMap out_put_condition;
		uint32_t fanfan_daily_cnt;
	public:
		void add_item_out_put_cnt(uint32_t itemid, uint32_t cnt)
		{
			FanfanDataMap::iterator it1 = out_put_condition.find(itemid);
			if (it1 != out_put_condition.end()) {
				it1->second.out_put_cnt += cnt;
			}
		}
		void add_daily_cnt()
		{
			fanfan_daily_cnt++;
		}
		bool judge_item_can_out_put(uint32_t itemid)
		{
			FanfanDataMap::iterator it1 = out_put_condition.find(itemid);
			
			if (it1 != out_put_condition.end()) {
				//printf("%u %u %u %u\n", fanfan_daily_cnt, it1->second.condition_daily_cnt_for_out_put, 
				//it1->second.out_put_cnt,it1->second.max_out_put_cnt);
				if (fanfan_daily_cnt < it1->second.condition_daily_cnt_for_out_put) {
					return false;
				}
				if (it1->second.max_out_put_cnt !=0 && 
					it1->second.out_put_cnt >= it1->second.max_out_put_cnt) {
					return false;
				}
			}
			return true;
		}
		
		void daily_reset()
		{
			fanfan_daily_cnt = 0;
			FanfanDataMap::iterator it = out_put_condition.begin();
			for (; it != out_put_condition.end(); ++it) {
				it->second.out_put_cnt = 0;
			}
		}
};

#pragma pack()

/*! global variable to hold all restriction actions  */
extern restriction_t g_all_restr[max_restr_count];

//------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------
void build_restriction_list(player_t* p, uint32_t count, const struct db_res_elem_t* db_res);

/**
  * @brief parse the "costs" and "rewards" sub node
  * @param chl sub node 
  * @param idx index to indicate certain restriction action
  * @return true on success, false on error
  */
bool parse_restriction_cost_reward(xmlNodePtr cur, uint32_t idx);

/**
  * @brief load restriction config from an xml file
  * @param cur xml node 
  * @return 0 on success, -1 on error
  */
int load_restrictions(xmlNodePtr cur);

void stat_log_relive_item(uint32_t item_id, uint32_t cnt, uint32_t get_or_use = 1) ;

int db_set_unique_item_bit(player_t* p);

void do_stat_log_exchange(uint32_t uid, uint32_t role_type, uint32_t act_id, uint32_t itemid);


/**
  * @brief query number of times to perform periodical action
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int query_restriction_count_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief player performs periodical action
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int restrict_action_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int restrict_action_ex_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int show_restrict_reward_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int recv_restrict_reward_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief get action times performs periodical action
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_restrict_action_times_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

//---------------------------------------------------------------------------------------------
// db_XXX function
//---------------------------------------------------------------------------------------------

/**
  * @brief add action count restriction to db
  * @param p the player who launches the request
  * @param p_restr
  * @return 0 on success, -1 on error
  */
int db_add_action_count(player_t* p, userid_t uid, uint32_t role_tm, uint32_t id, uint32_t flag, uint32_t toplimit, uint8_t userflg);

int db_swap_res_item_callback(player_t * p, const swap_item_rsp_t* rsp, int rsp_len);

int db_swap_item(player_t* p, const restriction_t* p_restr, const restriction_t* p_res_mail);

/**
  * @brief callback for handling query restriction action returned from dbproxy
  * @param p the requester
  * @param uid id of the requester
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */ 
int db_query_restriction_count_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling adding periodical action returned from dbproxy
  * @param p the requester
  * @param uid id of the requester
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */ 
int db_add_action_count_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling query restriction list when login
  * @param p the requester
  * @param uid id of the requester
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */ 
int db_query_restriction_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);


#endif  //KF_RESTRICTION_HPP_

