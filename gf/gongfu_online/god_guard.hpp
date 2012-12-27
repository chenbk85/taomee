#ifndef __GOD_GUARD_HPP__
#define __GOD_GUARD_HPP__

#include <stdint.h>
//#include <stdlib.h>
#include <map>

#include "fwd_decl.hpp"
//#include "cli_proto.hpp"
//#include "player.hpp"

using namespace std;

enum {
    max_group_member_num = 10,
};
#pragma pack(1)

//------------------------------------------------------------------
// structs
//------------------------------------------------------------------

struct quality_t {
    quality_t() {
        hp      = 0;
        mp      = 0;
        addhp   = 0;
        addmp   = 0;
        atk     = 0;
        def     = 0;
        hit     = 0;
        dodge   = 0;
        crit    = 0;
    }
    /* ! btl quality */
    uint32_t    hp;
    uint32_t    mp;
    uint32_t    addhp;
    uint32_t    addmp;
    uint32_t    atk;
    uint32_t    def;
    uint32_t    hit;
    uint32_t    dodge;
    uint32_t    crit;
};

struct god_factor {
    uint32_t    id;
    quality_t quality;
};

struct summon_factor {
    uint32_t    id;
    quality_t quality;
    quality_t group_quality;
};

//----------------------------
//     阵法
struct group_t {
    group_t() {
        _level = 1;
        memset(member, 0x00, sizeof(uint32_t) * max_group_member_num);
    }
    uint32_t    _level;
    //uint32_t    _power;
    uint32_t    member[max_group_member_num];
    /*
    uint32_t    summon_1[1];
    uint32_t    summon_2[2];
    uint32_t    summon_3[2];
    uint32_t    summon_4[3];
    uint32_t    god_3;
    uint32_t    god_4;
    */
};

#pragma pack()

class god_guard_mgr
{
public:
    god_guard_mgr() {}
    ~god_guard_mgr(){}
public:
    bool init(const char* xmlflie);
    bool final();

    god_factor *get_god_factor(uint32_t god_id)
    {
        std::map<uint32_t, god_factor*>::iterator pItr = god_datas.find(god_id);
        if (pItr != god_datas.end()) {
            return pItr->second;
        }
        return 0;
    }

    summon_factor *get_summon_factor(uint32_t summon_id)
    {
        std::map<uint32_t, summon_factor*>::iterator pItr = summon_datas.find(summon_id);
        if(pItr != summon_datas.end()) {
            return pItr->second;
        }
        return 0;
    }

    /*
    void set_group_datas(group_t *p_group)
    {
        memcpy(&group_datas, p_group, sizeof(group_t));
    }
    group_t * get_group_datas()
    {
        return &group_datas;
    }
    
    void set_group_level(uint32_t level) 
    {
        group_datas._level = level;
    }

    uint32_t get_group_level()
    {
        return group_datas._level;
    } */

    uint32_t calc_group_quality_and_power(player_t* p, uint32_t lv, quality_t *add_quality);
private:
    bool is_god_exist(uint32_t id);
    bool add_god_data(god_factor* p_god);
    bool is_summon_exist(uint32_t id);
    bool add_summon_data(summon_factor* p_sum);

    uint32_t summon_arithmetic(uint32_t base_value, uint32_t grow_rate, uint32_t lv);
    uint32_t group_arithmetic(uint32_t base_value, uint32_t add_value, uint32_t grow_rate, uint32_t llv,
        uint32_t hlv);
    uint32_t god_arithmetic(uint32_t base_value, uint32_t lv);
    void calc_summon_add_quality(player_t* p, uint32_t *sum, uint32_t sum_cnt, quality_t *add_quality);
    void calc_god_add_quality(uint32_t *god_id, uint32_t lv, quality_t *add_quality);

private:
    std::map<uint32_t, god_factor*> god_datas;
    std::map<uint32_t, summon_factor*> summon_datas;
    //group_t group_datas;
};

//----------------------------------------------------
int save_player_god_guard_info(player_t *p, uint32_t money);

int upgrade_god_guard_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int set_god_guard_position_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int get_god_guard_position_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

#endif

