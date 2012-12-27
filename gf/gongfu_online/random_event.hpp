#ifndef _RANDMON_EVENT_H_
#define _RANDMON_EVENT_H_

#include "fwd_decl.hpp"
#include "cli_proto.hpp"
#include <vector>
#include <map>
#include <list>
#include "player.hpp"

//extern "C" {
//#include <libtaomee/log.h>
//#include <libtaomee/conf_parser/config.h>
//}

using namespace std;

const int max_event_name = 20;
const int max_event_desc = 300;

#pragma pack(1)

struct reward_item
{
    uint32_t    type;
    uint32_t    id;
    char        name[max_nick_size];
    uint32_t    cnt;
};

struct event_data_t
{
	event_data_t()
	{
		event_id_ = 0;
		is_vip_ = 0;
		max_lv_ = 0;
		min_lv_ = 0;
        is_bcast_ = 0;
        memset(name_, 0x00, max_event_name);
        memset(desc_, 0x00, max_event_desc);
	}
	uint32_t  event_id_;
	uint32_t  is_vip_;
	uint32_t  max_lv_;
	uint32_t  min_lv_;
    uint32_t  is_bcast_;
	char      name_[max_event_name];
	char      desc_[max_event_desc];
    std::vector<reward_item> item_vec;
};

struct event_odds_t
{
    uint32_t    id;
    uint32_t    odds;
};

struct sum_odds_data_t
{
    sum_odds_data_t()
    {
        sum_type = 0;
        sum_prior = 0;
        memset(sum_name, 0x00, max_nick_size);
    }
    uint32_t    sum_type;
    uint32_t    sum_prior;
    char     sum_name[max_nick_size];
    std::vector<event_odds_t> odds_vec;
};

#pragma pack()

class sum_event_mgr
{
public:
    sum_event_mgr(){}

    bool init(const char* xmlfile);
	bool final();
    
    /**
     * @brief 
     * @return home summon cnt
     */
    uint32_t get_home_summon_list(player_t* p)
    {
        sum_vec_.clear();
        for(uint32_t i = 0; i < p->summon_mon_num; i++) {
            if (p->summons[i].call_flag == summon_in_home) {
                TRACE_LOG(" HOME summon [%u]", p->summons[i].mon_type);
                sum_vec_.push_back(p->summons[i].mon_type);
            }
        }
        return sum_vec_.size();
    }

    bool is_valid_event(player_t* p, uint32_t event_id);
    event_data_t * list_for_each_summon(player_t* p);

    uint8_t * get_event_summon_name()
    {
        return summon_name;
    }

private:
    uint32_t get_random_event_id(sum_odds_data_t* odds_data);
    bool add_event_to_map(event_data_t* pdata)
    {
        if (pdata == NULL) return false;

        std::map<uint32_t,  event_data_t>::iterator it = event_map_.find(pdata->event_id_);
        if (it != event_map_.end()) {
            throw XmlParseError(std::string("add_event_to_map"));
            return false;
        }
        event_map_[pdata->event_id_] = *pdata;
        return true;
    }
    event_data_t * get_event_data(uint32_t  event_id)
    {
        std::map<uint32_t,  event_data_t>::iterator it = event_map_.find(event_id);
        if (it != event_map_.end()) {
            return &(it->second);
        }
        return NULL;
    }

    bool add_odds_data_to_list(sum_odds_data_t* pdata)
    {
        if (pdata == NULL) return false;
        std::list<sum_odds_data_t>::iterator it = odds_list_.begin();
        if (it == odds_list_.end()) {
            odds_list_.push_back(*pdata);
        } else {
            for (; it != odds_list_.end(); ++it) {
                if (it->sum_prior >= pdata->sum_prior) {
                    odds_list_.insert(it, *pdata);
                    break;
                }
            }
            if (it == odds_list_.end()) {
                odds_list_.push_back(*pdata);
            }
        }
        return true;
    }
private:
    bool load_random_event(xmlNodePtr cur);
    bool load_random_summon(xmlNodePtr cur);

private:
    uint8_t summon_name[max_nick_size];
    std::vector<uint32_t> sum_vec_;
	std::map<uint32_t,  event_data_t> event_map_;
	std::list<sum_odds_data_t> odds_list_;
};


void init_player_random_event(player_t* p);
void process_random_event();

//--------------------------------------------------
#pragma pack(1)
struct reward_player_elem_t {
    uint32_t    id;
    uint32_t    uid;
    uint32_t    role_regtime;
    uint32_t    reward_id;
    uint32_t    reward_tm;
    uint32_t    reward_flag;
};

struct reward_player_cache_t {
    uint32_t    timestamp;
    uint32_t    cnt;
    reward_player_elem_t elem[100];
};

#pragma pack()

void mail_random_reward(player_t* p, uint32_t userid, uint32_t roletm, uint32_t reward_id);
void broadcast_random_reward_to_world(player_t* p, uint32_t userid, uint32_t roletm, uint32_t reward_id);
void save_data_to_db(uint32_t userid, uint32_t roletm, uint32_t reward_id);

int get_reward_player_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
int db_get_reward_player_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int db_set_player_reward_flag(player_t* p, uint32_t reward_tm);
uint32_t get_reward_player_rank(player_t* p, uint32_t reward_tm);
bool is_player_can_get_reward(player_t* p, uint32_t key);

#endif

