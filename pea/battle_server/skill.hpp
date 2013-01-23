#ifndef _SKILL_HPP_
#define _SKILL_HPP_

#include <list>
#include <map>
#include <vector>

#include <boost/intrusive/list.hpp>
#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <libtaomee++/event/eventable_obj.hpp>
#include <libtaomee++/event/event_mgr.hpp>

extern "C"
{
#include <libtaomee/list.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/project/utilities.h>
#include <libtaomee/project/types.h>
#include <async_serv/dll.h>
}

#include "pea_common.hpp"
#include "fwd_decl.hpp"
#include "object.hpp"

#define SKILL_ODDS  (100)

enum skill_target_t
{
	friend_team = 1<<0,
	enemy_team  = 1<<1
};


typedef struct skill_data
{
	skill_data()
	{
		memset(this, 0, sizeof(*this));
	}
	uint32_t skill_id;                    //技能ID
	uint32_t skill_lv;                    //技能级别
	uint32_t skill_type;                  //技能类型
	uint32_t skill_target;                //技能目标
	uint32_t skill_consume_strength;      //消耗体力
	uint32_t skill_consume_anger;         //消耗怒气
	uint32_t damage_percent;              //伤害百分比，实际伤害 = 该值 * 普通伤害/100
	uint32_t recove_hp;                   //每次攻击恢复伤害的百分比血量
	uint32_t recove_hp_target;            //回复血量的目标：1自己，2友军全体
    uint32_t bullet_id;                   //该技能所配置的子弹
	uint32_t bullet_times;                //多少波攻击
	uint32_t bullet_count_per_time;       //一波攻击多少颗子弹
	uint32_t bullet_excursion_angle;      //子弹偏移角度
	uint32_t skill_cd_time;               //技能CD时间，单位回合
}skill_data;




class skill_data_mgr
{
private:
	skill_data_mgr(){}
	~skill_data_mgr(){}
public:
	static skill_data_mgr* get_instance();
	bool   init_xml(const char* xml);
	bool   final();
public:
	bool   add_skill_data(skill_data* data);
	skill_data* get_skill_data_by_id(uint32_t skill_id, uint32_t skill_lv);
	bool   is_skill_data_exist(skill_data* data);
private:
	std::map<uint32_t, skill_data*> data_maps;
};

#endif
