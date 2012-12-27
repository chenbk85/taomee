/*
 * =====================================================================================
 *
 *       Filename:  plant.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/05/2011 11:13:55 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#ifndef PLANT_HPP
#define PLANT_HPP
#include <string>
#include <map>

#include <libtaomee++/conf_parser/xmlparser.hpp>
#include <libtaomee++/bitmanip/bitmanip.hpp>

#include "object.hpp"

using namespace taomee;
class SummonHome;
enum {
	max_plant_grow_step = 10,
	max_plant_cost_cnt = 10,
};

enum {
	plant_grow_status_dead = 0, //植物死亡
	plant_grow_status_up = 1,//植物升级
	plant_grow_status_down = 2,//植物降级
	plant_grow_status_init = 3,
};

struct plant_elem_t {
	uint32_t plant_id; //种类
	uint32_t plant_tm; // 唯一uid
	uint32_t field; //田地ID
	uint32_t status;//当前状态
	uint32_t status_tm;// 进入当前状态的时间
	//uint32_t water_cnt; //总浇水次数
	uint32_t water_tm;//可浇水的时间
	uint32_t fruit_cnt; //剩余果实数量
	uint32_t speed_per;
	uint32_t output_per;

	plant_elem_t(uint32_t field_id, uint32_t in_id)
	{
		field = field_id;
		plant_id = in_id;
		uint32_t now = get_now_tv()->tv_sec;
		plant_tm = now;
		water_tm = now;	
		status = 1;
		status_tm = now;
		fruit_cnt = 0;
		speed_per = 0;
		output_per = 0;
	}

	plant_elem_t ()
	{
		plant_id = 0;
		plant_tm = 0;
		field = 0;
		status = 0;
		status_tm = 0;
		water_tm = 0;
		fruit_cnt = 0;
		speed_per = 0;
		output_per = 0;
	}
}__attribute__((packed));


////////////////////////////////////////////
//xml file

//植物成长步骤
struct plant_grow_status_t {
	uint32_t id;//步骤id
	uint32_t duration;//持续时间
	uint32_t water_tm; //可浇水的最短间隔时间
	uint32_t fruit; //产出
	uint32_t fruit_cnt; //产出数量
	plant_grow_status_t() {
		id = 0;
		duration = 0;
		water_tm = 0;
		fruit = 0;
		fruit_cnt = 0;
	}
};

/*
struct plant_cost_t {
	uint32_t give_type;
	uint32_t give_id;
	uint32_t count;
}; */

struct plant_t {
	uint32_t id; //植物id，表示哪种植物

	uint32_t seed_id; //种子ID

	uint32_t home_lv[2]; // 小屋的等级范围

	uint32_t e_item; //额外产出，每次采集都有可能
	uint32_t d_odds;//额外产出的概率

	uint32_t status_cnt;
	plant_grow_status_t status_arr[max_plant_grow_step];
	
	plant_t() {
		id = 0;
		seed_id = 0;
		memset(home_lv, 0, sizeof(home_lv));
		memset(status_arr, 0, sizeof(status_arr));
	}	
};

class GobalPlants {
public:
	GobalPlants(const char* xml_file);
	~GobalPlants();

public:
	plant_t* get_plant(uint32_t id) {
		std::map<uint32_t,struct plant_t*>::iterator it = plant_map_.find(id);
		if ( it != plant_map_.end() ) {
			return it->second;
		}
		return 0;
	}
	std::map<uint32_t, plant_t*> plant_map_;
private:
	int init_plants();
private:
	std::string xml_file_name;
};


struct Ferilizer_t {
	uint32_t id;
	uint32_t speed_per;
	uint32_t output_per;
	uint32_t home_speed_per;
	uint32_t home_output_per;
};
class FerilizerMrg {
public:
	FerilizerMrg(const char* xml_file);
	~FerilizerMrg();

	Ferilizer_t* get_ferilizer(uint32_t id) {
		std::map<uint32_t,struct Ferilizer_t*>::iterator it = ferilizer_map_.find(id);
		if ( it != ferilizer_map_.end() ) {
			return it->second;
		}
		return 0;
	}
private:
	std::map<uint32_t, Ferilizer_t*> ferilizer_map_;
	std::string xml_file_name;
	int init_ferilizer();
};

//////////////////////////////
//class

class Plant : public Object {
 public:
	 Plant();

	 ~Plant();

	 Plant(SummonHome * p_home_in, plant_elem_t * plant_in);

	 //Plant(SummonHome* p_home_in, uint32_t plant_id_in, uint32_t plant_tm_in, uint32_t status_in, uint32_t status_tm_in);
public:
	 void update(int time_elapsed);

	 void pack_plant_info(void * pkg, int & idx);
	 void pack_plant_info_h(void * pkg, int & idx);
	 uint32_t get_duration_time();

	 uint32_t get_fruit_count();

	 bool can_water_now();

	 bool can_pick_fruit(Player * p);

	 int save_status_to_db();
	 int notify_plant_status_changed();
	 void change_grow_status(uint32_t status, uint32_t status_tm_passed = 0);
	 plant_grow_status_t* get_cur_grow_statu();
	 bool is_cur_status_user_active();
	 bool is_final_status();
	 uint32_t plant_id() { return plant_id_ ; }
	 uint32_t plant_tm() { return plant_tm_ ; }
	 uint32_t field_id() { return field_id_ ; }

	// uint32_t fruit_id() { return fruit_id ; }

	 bool if_can_be_watered();
	 void water_plant(Ferilizer_t* p_ferilizer);

	 bool if_can_be_ferilizered() { return true ; }
	 void ferilizer_plant(Ferilizer_t* p_ferilizer);
	 void ferilizer_plant(uint32_t speed_per_in, uint32_t output_per_in);

	 uint32_t fruit();

	 uint32_t get_fruit_cnt()
	 {
		 return fruit_cnt;
	 }


	 void reduce_fruit()
	 {
		 fruit_cnt --;
	 }

	 void be_picked(uint32_t cnt);
     bool need_deleted();

	 uint32_t get_total_fruit_cnt();

	 uint32_t fruit_cnt;
 private:
 	 uint32_t plant_id_;
	 uint32_t plant_tm_;
	 uint32_t field_id_; //田地
	 uint32_t status;
	 uint32_t status_tm;

	 uint32_t status_duration;//该阶段到期时间戳;

     uint32_t water_cnt;
	 uint32_t water_tm; //下次可浇水时间

	 uint32_t speed_per;
	 uint32_t output_per;
	 

	 SummonHome* p_home;

};

extern GobalPlants* g_plants;
extern FerilizerMrg* g_ferilizer_mrg;

//////////////////////////////////
//gobal func
//
int sow_plant_seed_cmd(Player *p, uint8_t * body, uint32_t bodylen);

int db_get_plants_list(Player* p, uint32_t uid, uint32_t utm);

int db_get_plants_list_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int get_plants_list_cmd(Player * p, uint8_t * body, uint32_t bodylen);

int set_plant_status_cmd(Player * p, uint8_t * body, uint32_t bodylen);

int pick_fruit_cmd(Player * p, uint8_t * body, uint32_t bodylen);

int db_pick_fruit(Player * p, Plant * p_plant, plant_t * p_xml);

int db_pick_fruit_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

#endif



