#ifndef BULLET_HPP
#define BULLET_HPP

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

class Battle;
class Player;
class skill_data;

typedef struct damage_range
{
	damage_range()
	{
		memset(this, 0, sizeof(damage_range));	
	}
	uint32_t begin_odds;
	uint32_t end_odds;
	uint32_t damage_percent;
}damage_range;

typedef struct bullet_data
{
	bullet_data()
	{
		memset(this, 0, sizeof(bullet_data));	
	}
	uint32_t bullet_id;
	uint32_t bullet_type;
	uint32_t bullet_width;
	uint32_t bullet_height;
	uint32_t damage_radius;
	uint32_t bullet_crater_id;
	uint32_t max_damage;
	std::vector<damage_range> ranges;
}bullet_data;

class bullet_mgr
{
private:
	bullet_mgr(){}
	~bullet_mgr(){}
public:
	static bullet_mgr* get_instance();
	bool   init_xml(const char* xml);
	bool   final();
public:
	bool   add_bullet_data(bullet_data* data);
	bullet_data* get_bullet_data_by_id(uint32_t bullet_id);
	bool   is_bullet_data_exist(bullet_data* data);
	bool   is_bullet_data_exist(uint32_t id);
private:
	std::map<uint32_t, bullet_data*> data_maps;
};

///////////////////////////////////////////////////////////////////////////////////

class bullet: public Object, public taomee::EventableObject
{
public:
	bullet(skill_data*	skill_data, uint32_t owner_id, uint32_t bullet_id, uint32_t team, uint32_t bullet_times, uint32_t syn_number);
    virtual ~bullet(){}
public:
	void init_bullet_pos(uint32_t x, uint32_t y, double x_speed, double y_speed, uint32_t dir);
public:
	bool collision(Object* p_obj);
	bool before_collision(Object* p_obj);
	bool after_collision(Object* p_obj);
	bool line_collision(Object* p_obj);
public:
	bool before_collision(map_data* data, void* para);
	bool after_collision(map_data* data, void* para);
	bool line_collision(map_data* data, void* para);
public:
	bool merge_bullet_crater_to_map( map_data* data, Battle* btl);
	bool check_blow_area(map_data* data, Battle* btl);
	uint32_t is_crit(Player* target);
	uint32_t calc_damage(Player* target, uint32_t& crit_flg);
public:	
	uint32_t 			id_;
	uint32_t 			owner_id_;
	uint32_t            team_;
	uint32_t            bullet_times_;
	uint32_t            syn_number_;
	bullet_data* 		p_data;
	bullet_crater_data* p_crater_data;
	skill_data* 		p_skill_data;
};

bullet* create_bullet(skill_data* p_skill_data, uint32_t id, uint32_t owner_id, uint32_t team, uint32_t syn_number, uint32_t bullet_times);

void    destroy_bullet(bullet* p_bullet);








#endif
