#ifndef _PET_ATTR_HPP_
#define _PET_ATTR_HPP_

#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <vector>

using namespace std;

typedef struct attr_data
{
	attr_data()
	{
		memset(this, 0, sizeof(attr_data));
	}
	uint32_t  agility;        //敏捷
	uint32_t  strength;       //力量
	uint32_t  stamina;        //耐力
	uint32_t  body_quality;   //体质
	uint32_t  hp;             //血
	uint32_t  attack;         //攻击
	uint32_t  crit_value;     //暴击值
	uint32_t  hit_value;      //命中值
	uint32_t  def_value;      //防御值
	uint32_t  dodge_value;    //躲闪值
	uint32_t  accurate_value; //精准值  
	void init()
	{
		agility = 0;        //敏捷
		strength = 0;       //力量
		stamina = 0;        //耐力
		body_quality = 0;   //体质
		hp = 0;             //血
		attack = 0;         //攻击
		crit_value = 0;     //暴击值
		hit_value = 0;      //命中值
		def_value = 0;      //防御值
		dodge_value = 0;    //躲闪值
		accurate_value = 0; //精准值  
	}
}attr_data;

typedef struct pet_attr_data
{
	pet_attr_data()
	{
		attr_type = 0;
	}
	uint32_t attr_type;
	attr_data base_data;
	attr_data step_data;
}pet_attr_data;


struct item_attr_add_t 
{
	uint32_t odds_min;
	uint32_t odds_max;
	uint32_t odds;
	uint32_t add_per;
};

enum {
	max_attrs_num = 10
};

struct item_attr_t {
	uint32_t item_id;
	uint32_t per_num;
	item_attr_add_t attrs[max_attrs_num];
};


class pet_attr_data_mgr
{
public:
		pet_attr_data_mgr(){}
		~pet_attr_data_mgr(){}
public:
	bool init(const char* xml);
	bool final();
	bool is_attr_type_exist(uint32_t attr_type);
	bool add_pet_attr_data(pet_attr_data* p_data);
	bool add_item_attr_add_data(item_attr_t * data);
	pet_attr_data* get_pet_attr_data_by_type(uint32_t attr_type);
	int get_attr_per_by_item_id(uint32_t item_id);
private:
	std::map<uint32_t, pet_attr_data*> datas;
	std::map<uint32_t, item_attr_t*> item_datas;
};

void load_pet_attribute(const char * xml);

void calc_summon_attr(attr_data * attr, uint32_t attr_type, uint32_t attr_per, uint32_t lv);

int get_attr_per(uint32_t item_id);

#endif
