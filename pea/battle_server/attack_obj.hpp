#ifndef ATTACK_OBJ_H
#define ATTACK_OBJ_H

#include "bullet.hpp"
#include "player.hpp"
#include "battle_impl.hpp"
#include "skill.hpp"

#define BULLET_TIME_INTERVAL  (500)
#define AFTER_ATTACK_INTERVAL  (3000)

class attack_obj
{
public:
	attack_obj(uint32_t skill_id, uint32_t skill_lv, uint32_t angle, uint32_t velocity, Player* p, Battle* p_btl);
	virtual ~attack_obj();
public:
	bool init_attack_obj();
	bool final_attack_obj();
public:
	bool get_hit_result();

	void set_hit_result(bool result);

	bool process_attack_obj(struct timeval cur_time,  uint32_t& process_time);

public:
	skill_data* p_skill_data;            	//技能描述数据		

	uint32_t remain_bullet_times;        	//剩余的子弹波次
	uint32_t gun_angle;                  	//枪口角度
	uint32_t muzzle_velocity;             	//子弹初速度
	bool     hit_result;                    //这次攻击是否击中
	
	std::list<bullet*> bullet_list;
	Player*  owner;
	Battle*  btl;
};


attack_obj* create_attack_obj(uint32_t skill_id, uint32_t skill_lv, uint32_t angle, uint32_t velocity,  struct timeval  cur_time,  Player* p);

void        destroy_attack_obj(attack_obj* obj);


#endif
