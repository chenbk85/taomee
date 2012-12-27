#ifndef _ACTIVE_CONFIG_H_
#define _ACTIVE_CONFIG_H_

#include "fwd_decl.hpp"
#include "cli_proto.hpp"
#include <vector>
#include <map>
#include "player.hpp"

using namespace std;


typedef struct active_time_data
{
	active_time_data()
	{
		week_ = -1;
		begin_hour_ = -1;
		begin_min_ = -1;
		end_hour_ = -1;
		end_min_ = -1;
		enter_flag_ = -1;
	}
	int32_t  week_;
	int32_t  begin_hour_;
	int32_t  begin_min_;
	int32_t  end_hour_;
	int32_t  end_min_;
	int32_t  enter_flag_;
}active_time_data;

typedef struct active_data
{
	active_data()
	{
		active_id_  = 0;
		min_lv_     = 0;
		max_lv_     = 0;
        is_team_    = 0;
        team_member_= 0;
        team_coins_ = 0;
        coins_      = 0;
        from_       = 0;
        to_         = 0;
        broadcast_switch_ = 0;
        memset(word1, 0x00, max_official_mcast_size);
        memset(word2, 0x00, max_official_mcast_size);
	}
	void push_active_time_data(active_time_data& data)
	{
		time_datas_.push_back(data);
	}
	bool check_level(player_t* p)
	{
		if( min_lv_ ==0 && max_lv_ == 0)return true;
		return  p->lv >= min_lv_ && p->lv <= max_lv_;
	}
    uint32_t check_team(player_t* p)
    {
        if (is_team_ == 0) return 0;
        if (p->team_info.team_id == 0) return cli_err_not_team_member;

        if (p->team_info.team_member_cnt < team_member_) return cli_err_swap_action_illegal;

        if (p->team_info.coin < team_coins_) return cli_err_team_coin_not_enough;
        return 0;
    }

	active_time_data * get_cur_time_data()
	{
		time_t now_tm = get_now_tv()->tv_sec;
		struct tm * p_cur_tm = localtime(&now_tm);
		for (std::vector<active_time_data>::iterator it = time_datas_.begin();
				it != time_datas_.end(); ++it) {

			if (it->week_ != p_cur_tm->tm_wday) {
				continue;
			}

			bool begin_flag = false;
			if (it->begin_hour_ != -1) {
				if (it->begin_hour_ < p_cur_tm->tm_hour) {
					begin_flag = true;
				} else if (it->begin_hour_ == p_cur_tm->tm_hour
						&& it->begin_min_ <= p_cur_tm->tm_min) {
					begin_flag = true;
				}
			}

			bool end_flag = false;
			if (it->end_hour_ != -1) {
				if (it->end_hour_ > p_cur_tm->tm_hour) {
					end_flag = true;
				} else if (it->end_hour_ == p_cur_tm->tm_hour
						&& it->end_min_ > p_cur_tm->tm_min) {
					end_flag = true;
				}
			}

			if (begin_flag && end_flag) return &(*it);
		}

		return NULL;
	}

    uint32_t active_id_;
	uint32_t min_lv_;
	uint32_t max_lv_;
    uint32_t is_team_;
    uint32_t team_member_;
    uint32_t team_coins_;
	uint32_t coins_;
	uint32_t from_;
	uint32_t to_;
    uint32_t broadcast_switch_;
    char    word1[max_official_mcast_size];
    char    word2[max_official_mcast_size];
	std::vector<active_time_data>  time_datas_;
}active_data;

typedef active_data*  active_data_ptr;

class active_data_mgr
{
public:
	active_data_mgr() {}
public:
	//static active_data_mgr* getInstance();
public:
	bool init(const char* xmlfile);
	bool final();
	bool add_active_data( active_data* pData);
	active_data* get_data_by_active_id(uint32_t active_id);
public:
	std::map<uint32_t,  active_data> active_datas_;
};

uint32_t is_in_active_time_section(uint32_t active_id);

uint32_t check_player_can_join_active(player_t* p, uint32_t active_id, bool is_free = false);

uint32_t is_in_active_time(uint32_t active_id);

uint32_t check_player_enter_active(active_data_mgr* mgr, player_t* p,  uint32_t active_id, bool is_free);

active_time_data * get_now_active_data_by_active_id(uint32_t active_id);

int  get_active_errcode(uint32_t active_id);

void do_active_event_logic();
void do_active_breadcast_logic();

bool is_celebration_goods_full();

//added by cws 0612
void do_active_escort_reward_logic();


int init_server_config(void* owner, void* data);

void init_server_config_timer();

bool is_in_holiday();



//------------------------------------------------------

struct event_block_t{
    uint32_t    stageid;
    uint32_t    diff;
    uint32_t    odds;
};

struct event_node_t {
    event_node_t()
    {
        event_id = 0;
        memset(name, 0x00, 20);
        type = 0;
        memset(time_arr, 0x00, max_event_trigger_time_size);
    }
    uint32_t    event_id;
    char        name[20];
    uint32_t    type;
    uint32_t    time_arr[max_event_trigger_time_size];
    std::vector<event_block_t>  block_vec;
};


class event_trigger_mgr
{
public:
	event_trigger_mgr() {}
public:
	bool init(const char* xmlfile);
	bool final();
	bool add_event_trigger( event_node_t* pData);
	event_node_t* get_data_by_event_id(uint32_t event_id);

    event_block_t*  rand_block_event(uint32_t event_id);
public:
	std::map<uint32_t, event_node_t> event_trigger;
};

int do_event_trigger_logic(player_t* p, uint32_t event_id, uint32_t trigger_times, int reason = -1);

int del_event_trigger_timer(player_t* p);

int send_block_info_to_client(player_t* p, uint32_t eventid, event_block_t* pblock);

int send_block_result_to_client(player_t* p, uint32_t eventid, uint32_t trigger_times, uint32_t result);

#endif
