#ifndef map_summon_object_h_
#define map_summon_object_h_


extern "C" 
{
#include <stdint.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/constants.h>
}
#include <list>
#include <libtaomee++/time/time.hpp>
#include <libtaomee++/memory/mempool.hpp>
#include "base_effect.hpp"
#include "object.hpp"

class Battle;

using namespace std;
using namespace taomee;

enum target_type
{
	target_begin_type = 0,
	target_enemy_type = 1,      //敌方目标
	target_friend_type = 2,     //友方目标
	target_end_type
};

enum action_type
{
	action_begin_type = 0,
	action_continue_type = 1,    		   //持续触发, 一直触发，直到时间结束
	action_condition_times_type = 2,       //条件次数触发，按次数触发，次数到了后就结束
	action_dead_type = 3,        		   //结束的时候触发
	action_times_type = 4,
	action_end_type = 0
};

enum summon_type
{
	summon_begin_type = 0,
	summon_radish_mine = 1,      //萝卜地雷
	summon_end_type
};

class map_summon_object: public Object, public base_duration_timer
{
public:
		map_summon_object();
		~map_summon_object();
public:
		bool init(uint32_t type_id, uint32_t model, uint32_t action_radius, uint32_t action_x, uint32_t action_y, uint32_t target_type, uint32_t action_type, uint32_t skill_id);
		bool final();
		void set_pos(const Vector3D& v,  map_t* map);
		void set_range(uint32_t radius);
		void set_range(uint32_t x, uint32_t y);
		void set_owner_id(uint32_t owner_id, uint32_t team, Battle* battle);
		void set_base_effect(base_effect* effect);
		void set_buff_id(uint32_t buff_id);
		void set_times(uint32_t time);
		void set_delay_time(uint32_t time);
		void set_call_skill_id(uint32_t skill_id);
public:
		bool check_action_radius(Player* p);	
		bool check_target_type(Player* p);
		bool check_trigger(struct timeval  next_tm);
		bool check_delete(struct timeval  next_tm);		
		bool check_delay_time( struct timeval  next_tm);
		bool process(Player* p, struct timeval next_tm);
		bool process_buff(Player* p);
		uint32_t get_buff_id();
private:	
		base_effect     *effect_;                 //包含的效果	
public:
		uint32_t        id_;                      //唯一ID
		uint32_t 		owner_id_;                //拥有者ID
		uint32_t        team_;                     //类别		
		uint32_t        model_;                   //模型
		char            nick[16];                 //名字
		uint32_t 		action_radius_;           //作用距离
		uint32_t        action_x_;
		uint32_t        action_y_;
		uint32_t        target_type_;		      //作用对象（敌，我）
		uint32_t        action_type_;             //作用方式
		int32_t        action_times_;            //触发次数限制
		Battle*         battle_;                  //属于的BATTLE
		map_t*          cur_map_;                 //属于当前的地图;
		uint32_t        skill_id_;                //客户端特效触发的时候播放的动画ID
		uint32_t        call_skill_id_;           //召唤物在被召唤的时候播放的动画ID
		uint32_t        buff_id_;                 //buff_id
		uint32_t        delay_time_;              //时间延后再做轮询(毫秒)
};

map_summon_object*  create_map_summon_object( uint32_t id);
void                destroy_map_summon_object(map_summon_object* p);



void notify_add_map_summon_object( map_summon_object* obj, Player* p = NULL);
void notify_del_map_summon_object( map_summon_object* obj);

void notify_strigger_map_summon_object(map_summon_object* obj);
void notify_strigger_map_summon_object_by_call(map_summon_object* obj);
void notify_map_summon_object(Player* p);




typedef struct map_summon_data
{
	map_summon_data()
	{
		id = 0;
		type = 0;
		memset(name, 0, sizeof(name));
		model = 0;
		action_radius = 0;
		action_x = 0;
		action_y = 0;
		action_type = 0;
		action_target = 0;
		action_times = 0;
		duration_time = 0;
		show_id = 0;
		effect_id = 0;
		effect_type = 0;
		buff_id = 0;
		delay_time = 0;
		call_show_id = 0;
	}
	uint32_t  id;
	uint32_t  type;
	char      name[16];
	uint32_t  model;
	uint32_t  action_radius;
	uint32_t  action_x;
	uint32_t  action_y;
	uint32_t  action_type;
	uint32_t  action_target;
	uint32_t  action_times;
	uint32_t  duration_time;
	uint32_t  show_id;
	uint32_t  effect_id;
	uint32_t  effect_type;
	uint32_t  buff_id;
	uint32_t  delay_time;
	uint32_t  call_show_id;
}map_summon_data;



class map_summon_data_mgr
{
private:
	map_summon_data_mgr();
	~map_summon_data_mgr();
public:
	static map_summon_data_mgr*  getInstance();
public:
	bool init(const char* xml_name);
	bool final();
	bool add_map_summon_data(  map_summon_data* data);
	bool is_map_summon_data_exist( uint32_t id);
	map_summon_data* get_map_summon_data(uint32_t id);
private:
	std::map<uint32_t, map_summon_data*> m_data_maps;
};







#endif
